"""Console integration tests on temporary data. Does not modify project Data."""
import hashlib
import shutil
import subprocess
import sys
import tempfile
import time
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXE = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else ROOT / "build/TV36-complete.exe"
LONG_URL = "https://example.com/a-very-long-stream-address-for-testing.m3u8"


class CrudTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="tv36-complete-")
        self.work = Path(self.temp.name)
        self.addCleanup(self.temp.cleanup)
        shutil.copytree(ROOT / "Data", self.work / "Data")
        # Fixtures are independent of sample sizes/counters in the submission.
        self.write_rows("channels.txt", [
            ["KENH001", "VTV1", "TL004", "1", "HD", "https://example.com/1.m3u8", "DANG_PHAT"],
            ["KENH002", "Sports", "TL001", "2", "FHD", "https://example.com/2.m3u8", "DANG_PHAT"],
            ["KENH003", "Education", "TL002", "7", "HD", "https://example.com/3.m3u8", "DANG_PHAT"],
            ["KENH004", "Unused", "TL004", "69", "SD", "https://example.com/4.m3u8", "DANG_PHAT"],
        ])
        self.write_rows("schedules.txt", [
            ["LPS001", "KENH001", "News", "2026-10-16 19:00", "2026-10-16 19:45", "Evening news"],
            ["LPS002", "KENH002", "Sports tonight", "2026-10-16 20:00", "2026-10-16 21:00", ""],
            ["LPS003", "KENH003", "Night class", "2026-10-16 23:30", "2026-10-17 00:30", ""],
        ])
        self.path("channels_counter.txt").write_text("4\n")
        self.path("schedules_counter.txt").write_text("3\n")
        self.write_rows("packages.txt", [
            ["GOI001", "Basic", "80000", "DANG_CUNG_CAP", "KENH001,KENH002", "VOD001,VOD002"],
        ])

    def path(self, name):
        return self.work / "Data" / name

    def rows(self, name):
        return [line.split("|") for line in self.path(name).read_text(encoding="utf-8").splitlines() if line]

    def write_rows(self, name, rows):
        self.path(name).write_text("".join("|".join(row) + "\n" for row in rows), encoding="utf-8")

    def run_menu(self, menu, *steps):
        commands = "\n".join(map(str, [menu, *steps, 0, 0])) + "\n"
        log = self.work / "console.log"
        with log.open("w", encoding="utf-8") as out:
            result = subprocess.run([str(EXE)], input=commands, text=True, encoding="utf-8",
                                    errors="replace", cwd=self.work, stdout=out,
                                    stderr=subprocess.STDOUT, timeout=6)
        output = log.read_text(encoding="utf-8", errors="replace")
        self.assertEqual(result.returncode, 0, output[-3000:])
        self.assertIn("Cam on ban da su dung he thong", output)
        return output

    def add_channel(self, number="11", status="1"):
        return self.run_menu(4, 1, "Test Channel", "tl004", number, "hd", LONG_URL, status, "")

    def add_schedule(self, channel="KENH001", start="2030-10-16 23:30", end="2030-10-17 00:30", desc="Note"):
        return self.run_menu(5, 1, channel, "Test program", start, end, desc, "")

    def test_main_opens_all_ten_menus(self):
        # Menu navigation uses valid temporary fixtures for the updated parsers.
        # The team's sample files still contain the previous status spellings.
        self.write_rows("hoadon.txt", [["HDN001", "HD001", "KH001", "100000",
                                       "01/10/2026", "31/10/2026", "NONE", "UnPaid"]])
        self.write_rows("noidungvod.txt", [["VOD001", "News", "News", "30", "",
                                           "2026", "https://example.com/vod", "ACTIVE"]])
        steps = [0]
        for menu in range(2, 11):
            steps.extend([menu, 0])
        out = self.run_menu(1, *steps)
        for number in range(1, 11):
            self.assertRegex(out, rf"(?m)^\s*{number}\. Quan ly ")
        for heading in ["CUSTOMER MANAGEMENT", "SET-TOP BOX MANAGEMENT", "CHANNEL CATEGORY MANAGEMENT",
                        "===== Channel Management =====", "===== Schedule Management =====",
                        "===== Package Management =====", "CONTRACT MANAGEMENT", "Invoice Management",
                        "VOD Content Management", "===== Support Ticket Management ====="]:
            self.assertIn(heading, out)
        self.assertNotIn("[Error]", out)

    def test_channel_crud_restart_and_long_detail(self):
        self.assertIn("ID: KENH005", self.add_channel())
        self.assertIn(LONG_URL, self.run_menu(4, 2, 2, "kenh005", ""))
        self.run_menu(4, 3, "KENH005", "Renamed", "", "12", "", "", "0", "")
        row = self.rows("channels.txt")[-1]
        self.assertEqual(row, ["KENH005", "Renamed", "TL004", "12", "HD", LONG_URL, "NGUNG_PHAT"])
        before = self.path("channels.txt").read_bytes()
        self.assertIn("Deletion cancelled", self.run_menu(4, 4, "KENH005", "n", ""))
        self.assertEqual(before, self.path("channels.txt").read_bytes())
        self.assertIn("deleted successfully", self.run_menu(4, 4, "KENH005", "y", ""))
        self.assertIn("Channel not found", self.run_menu(4, 2, 2, "KENH005", ""))

    def test_channel_status_zero_one_and_blank(self):
        for number, status, expected in [("15", "0", "NGUNG_PHAT"), ("16", "1", "DANG_PHAT")]:
            self.assertIn("created successfully", self.add_channel(number, status))
            self.assertEqual(self.rows("channels.txt")[-1][-1], expected)
        before = self.rows("channels.txt")
        self.run_menu(4, 3, "KENH001", "", "", "", "", "", "", "")
        self.assertEqual(before, self.rows("channels.txt"))
        self.run_menu(4, 3, "KENH001", "", "", "", "", "", "0", "")
        self.assertEqual(self.rows("channels.txt")[0][-1], "NGUNG_PHAT")
        self.run_menu(4, 3, "KENH001", "", "", "", "", "", "1", "")
        self.assertEqual(self.rows("channels.txt")[0][-1], "DANG_PHAT")

    def test_channel_invalid_fields_retry_at_same_prompt(self):
        out = self.run_menu(
            4, 1, "Bad|name", "\t", "Good name",
            "", "khenh", "KENH001", "TL04", "TL00A", "TL999", "tl004",
            "-1", "0", "1.5", "12abc", "01", "999999999999999999999", "1", "20",
            "8K", "hd", "bad|url", LONG_URL, "", "2", "DANG_PHAT", " 0 ", "")
        for message in ["Text must not be empty", "Ma khong hop le", "Category not found: TL999",
                        "So kenh phai la so nguyen", "Channel number already exists: 1",
                        "Do phan giai chi nhan", "Chi nhap 0 hoac 1"]:
            self.assertIn(message, out)
        self.assertEqual(self.rows("channels.txt")[-1],
                         ["KENH005", "Good name", "TL004", "20", "HD", LONG_URL, "NGUNG_PHAT"])
        self.assertEqual(len(self.rows("channels.txt")), 5)

    def test_edit_category_unknown_then_blank_and_self_number(self):
        before = self.rows("channels.txt")
        out = self.run_menu(4, 3, "KENH001", "", "TL999", "", "1", "", "", "", "")
        self.assertIn("Category not found", out)
        self.assertIn("updated successfully", out)
        self.assertEqual(self.rows("channels.txt"), before)

    def test_edit_duplicate_number_retries_and_keeps_valid_name(self):
        out = self.run_menu(4, 3, "KENH004", "New unused", "", "1", "2", "70", "", "", "", "")
        self.assertEqual(out.count("Channel number already exists"), 2)
        self.assertEqual(self.rows("channels.txt")[-1][1:4], ["New unused", "TL004", "70"])

    def test_cancel_create_and_update_does_not_save_or_increment_id(self):
        before = self.path("channels.txt").read_bytes()
        self.assertIn("cancelled", self.run_menu(4, 1, "0", ""))
        self.run_menu(4, 3, "KENH001", "0", "")
        self.assertEqual(self.path("channels.txt").read_bytes(), before)
        self.assertEqual(self.path("channels_counter.txt").read_text().strip(), "4")
        before = self.path("schedules.txt").read_bytes()
        self.run_menu(5, 1, "KENH001", "0", "")
        self.run_menu(5, 3, "LPS001", "", "0", "")
        self.assertEqual(self.path("schedules.txt").read_bytes(), before)
        self.assertEqual(self.path("schedules_counter.txt").read_text().strip(), "3")

    def test_delete_channel_blocks_both_package_and_schedule(self):
        before = self.path("channels.txt").read_bytes()
        out = self.run_menu(4, 4, "KENH001", "y", "")
        self.assertIn("referenced by Package GOI001", out)
        self.assertIn("referenced by Schedule LPS001", out)
        self.assertEqual(before, self.path("channels.txt").read_bytes())

    def test_delete_channel_blocks_schedule_alone(self):
        self.write_rows("packages.txt", [])
        before = self.path("channels.txt").read_bytes()
        self.assertIn("referenced by Schedule", self.run_menu(4, 4, "KENH003", "y", ""))
        self.assertEqual(before, self.path("channels.txt").read_bytes())

    def test_delete_stopped_channel_in_inactive_package(self):
        rows = self.rows("channels.txt")
        rows[-1][-1] = "NGUNG_PHAT"
        self.write_rows("channels.txt", rows)
        self.write_rows("packages.txt", [["GOI001", "Stopped package", "10", "NGUNG_DANG_KY", "KENH001,KENH004", "VOD001,VOD002"]])
        before = self.path("channels.txt").read_bytes()
        self.assertIn("referenced by Package", self.run_menu(4, 4, "KENH004", "y", ""))
        self.assertEqual(before, self.path("channels.txt").read_bytes())

    def test_delete_channel_confirmation_retries(self):
        out = self.run_menu(4, 4, "KENH004", "maybe", "1", "n", "")
        self.assertEqual(out.count("Please enter y or n"), 2)
        self.assertEqual(len(self.rows("channels.txt")), 4)

    def test_missing_category_file_is_created_and_creation_waits_for_category(self):
        before = self.path("channels.txt").read_bytes()
        self.path("channel_categories.txt").unlink()
        self.assertIn("No categories available", self.run_menu(4, 1, ""))
        self.assertTrue(self.path("channel_categories.txt").exists())
        self.assertEqual(self.path("channel_categories.txt").read_text(), "")
        self.assertEqual(self.path("channels.txt").read_bytes(), before)

    def test_corrupt_category_row_is_skipped_and_later_row_is_used(self):
        path = self.path("channel_categories.txt")
        path.write_text("broken|row\n" + path.read_text(encoding="utf-8"), encoding="utf-8")
        out = self.run_menu(4, 1, "", "0", "")
        self.assertIn("[Warning]", out)
        self.assertIn("Enter 0 at Name to cancel", out)

    def test_empty_category_list_cancels_creation(self):
        self.write_rows("channel_categories.txt", [])
        before = self.path("channels.txt").read_bytes()
        self.assertIn("No categories available", self.run_menu(4, 1, ""))
        self.assertEqual(before, self.path("channels.txt").read_bytes())

    def test_category_menu_still_blocks_category_used_by_channel(self):
        before = self.path("channel_categories.txt").read_bytes()
        output = self.run_menu(3, 4, "TL004", "")
        self.assertIn("[Error]", output)
        self.assertEqual(before, self.path("channel_categories.txt").read_bytes())

    def test_schedule_crud_overnight_description_and_restart(self):
        self.assertIn("ID: LPS004", self.add_schedule())
        self.assertIn("Test program", self.run_menu(5, 2, 2, "lps004", ""))
        self.run_menu(5, 3, "LPS004", "", "New program",
                      "2031-10-18 23:30", "2031-10-19 00:30", "", "")
        self.assertEqual(self.rows("schedules.txt")[-1], [
            "LPS004", "KENH001", "New program", "2031-10-18 23:30", "2031-10-19 00:30", "Note"])
        self.run_menu(5, 3, "LPS004", "", "", "", "", "-", "")
        self.assertEqual(self.rows("schedules.txt")[-1][-1], "")
        before = self.path("schedules.txt").read_bytes()
        self.run_menu(5, 4, "LPS004", "n", "")
        self.assertEqual(before, self.path("schedules.txt").read_bytes())
        self.run_menu(5, 4, "LPS004", "y", "")
        self.assertIn("Schedule not found", self.run_menu(5, 2, 2, "LPS004", ""))

    def test_schedule_retries_unknown_channel_and_accepts_stopped_channel(self):
        rows = self.rows("channels.txt")
        rows[-1][-1] = "NGUNG_PHAT"
        self.write_rows("channels.txt", rows)
        out = self.run_menu(5, 1, "wrong", "KENH999", "KENH004", "Valid",
                            "2030-01-01 10:00", "2030-01-01 11:00", "", "")
        self.assertIn("Ma khong hop le", out)
        self.assertIn("Channel not found: KENH999", out)
        self.assertIn("created successfully", out)
        self.assertEqual(self.rows("schedules.txt")[-1][1], "KENH004")

    def test_schedule_dates_and_text_reprompt(self):
        out = self.run_menu(5, 1, "KENH001", "Bad|program", "Valid",
                            "2030-02-29 10:00", "2030-01-01 24:00", "2030-1-01 10:00",
                            "2030-01-01 10:00", "2030-01-01 10:00",
                            "2030-01-01 09:00", "2030-01-01 11:00", "bad|description", "", "")
        self.assertEqual(out.count("Invalid date/time:"), 3)
        self.assertEqual(out.count("End time must be after start time"), 2)
        self.assertIn("created successfully", out)
        self.assertEqual(self.rows("schedules.txt")[-1][3:],
                         ["2030-01-01 10:00", "2030-01-01 11:00", ""])

    def test_overlap_all_shapes_and_retry_without_losing_program(self):
        original = self.rows("schedules.txt")
        shapes = [
            ("18:50", "19:10"), ("19:30", "20:00"), ("19:10", "19:20"),
            ("18:00", "20:00"), ("19:00", "19:45"),
        ]
        for start, end in shapes:
            with self.subTest(start=start, end=end):
                self.write_rows("schedules.txt", original)
                out = self.run_menu(5, 1, "KENH001", "Keep program",
                                    "2026-10-16 " + start, "2026-10-16 " + end,
                                    "2026-10-16 19:45", "2026-10-16 20:00", "", "")
                self.assertIn("Schedule overlaps LPS001", out)
                self.assertEqual(len(self.rows("schedules.txt")), 4)
                self.assertEqual(self.rows("schedules.txt")[-1][2:5],
                                 ["Keep program", "2026-10-16 19:45", "2026-10-16 20:00"])

    def test_midnight_overlap_and_adjacent_before(self):
        out = self.run_menu(5, 1, "KENH003", "Night",
                            "2026-10-17 00:00", "2026-10-17 01:00",
                            "2026-10-16 23:00", "2026-10-16 23:30", "", "")
        self.assertIn("Schedule overlaps LPS003", out)
        self.assertEqual(self.rows("schedules.txt")[-1][3:5],
                         ["2026-10-16 23:00", "2026-10-16 23:30"])

    def test_same_time_different_channel_allowed(self):
        self.assertIn("created successfully", self.add_schedule(
            "KENH002", "2026-10-16 19:00", "2026-10-16 19:45"))

    def test_schedule_update_ignores_itself_and_checks_new_channel(self):
        before = self.rows("schedules.txt")
        self.run_menu(5, 3, "LPS001", "", "", "", "", "", "")
        self.assertEqual(before, self.rows("schedules.txt"))
        self.run_menu(5, 3, "LPS001", "", "Moved",
                      "2026-10-16 20:00", "2026-10-16 21:00", "", "")
        out = self.run_menu(5, 3, "LPS001", "KENH002", "", "", "",
                            "2026-10-16 21:00", "2026-10-16 22:00", "", "")
        self.assertIn("Schedule overlaps LPS002", out)
        self.assertEqual(self.rows("schedules.txt")[0][1:5],
                         ["KENH002", "Moved", "2026-10-16 21:00", "2026-10-16 22:00"])

    def test_stopped_channel_can_be_rescheduled_when_slot_is_free(self):
        rows = self.rows("channels.txt")
        rows[0][-1] = "NGUNG_PHAT"
        self.write_rows("channels.txt", rows)
        self.run_menu(5, 3, "LPS001", "", "Historical correction", "", "", "", "")
        self.assertEqual(self.rows("schedules.txt")[0][2], "Historical correction")
        out = self.run_menu(5, 3, "LPS001", "", "", "2030-01-01 10:00", "2030-01-01 11:00", "", "")
        self.assertIn("updated successfully", out)
        self.assertEqual(self.rows("schedules.txt")[0][3:5], ["2030-01-01 10:00", "2030-01-01 11:00"])
        self.assertIn("Historical correction", self.run_menu(5, 2, 2, "LPS001", ""))

    def test_stopped_channels_allow_schedule_but_empty_or_missing_list_prevents_it(self):
        rows = self.rows("channels.txt")
        for row in rows: row[-1] = "NGUNG_PHAT"
        self.write_rows("channels.txt", rows)
        self.assertIn("created successfully", self.add_schedule())
        before = self.path("schedules.txt").read_bytes()
        self.write_rows("channels.txt", [])
        self.assertIn("No channels available", self.run_menu(5, 1, ""))
        self.path("channels.txt").unlink()
        self.assertIn("No channels available", self.run_menu(5, 1, ""))
        self.assertTrue(self.path("channels.txt").exists())
        self.assertEqual(before, self.path("schedules.txt").read_bytes())

    def test_leap_day_and_blank_description(self):
        self.assertIn("created successfully", self.add_schedule(
            start="2028-02-29 23:00", end="2028-03-01 00:00", desc=""))
        self.assertEqual(self.rows("schedules.txt")[-1][-1], "")
        self.assertIn("Test program", self.run_menu(5, 2, 2, "LPS004", ""))

    def test_orphan_schedule_can_be_moved_to_an_existing_channel(self):
        rows = self.rows("channels.txt")[1:]
        for row in rows: row[-1] = "NGUNG_PHAT"
        self.write_rows("channels.txt", rows)
        out = self.run_menu(5, 3, "LPS001", "KENH002", "", "", "", "", "")
        self.assertIn("updated successfully", out)
        self.assertEqual(self.rows("schedules.txt")[0][1], "KENH002")

    def test_primary_bad_row_is_skipped_and_later_rows_are_read(self):
        for menu, name in [(4, "channels.txt"), (5, "schedules.txt")]:
            original = self.path(name).read_bytes()
            with self.subTest(name=name):
                self.path(name).write_bytes(b"broken|row\n" + original)
                before = self.path(name).read_bytes()
                out = self.run_menu(menu, "", 2, "", 1, "")
                self.assertIn("[Warning]", out)
                self.assertIn("KENH001" if menu == 4 else "LPS001", out)
                self.assertEqual(before, self.path(name).read_bytes())
            self.path(name).write_bytes(original)

    def test_duplicate_numbers_and_overlaps_in_file_are_reported(self):
        rows = self.rows("channels.txt")
        rows[-1][3] = "1"
        self.write_rows("channels.txt", rows)
        self.assertIn("Duplicate channel number", self.run_menu(4))
        rows = self.rows("schedules.txt")
        rows.append(["LPS004", "KENH001", "Bad overlap", "2026-10-16 19:10", "2026-10-16 19:20", ""])
        self.write_rows("schedules.txt", rows)
        self.assertIn("Overlapping schedules", self.run_menu(5))

    def test_invalid_category_in_primary_file_is_reported(self):
        rows = self.rows("channels.txt")
        rows[0][2] = "KHENH"
        self.write_rows("channels.txt", rows)
        out = self.run_menu(4, "", 2, "", 1, "")
        self.assertIn("[Warning]", out)
        self.assertIn("KENH002", out)
        self.assertNotIn("| KENH001", out)

    def test_missing_primary_file_is_created(self):
        for menu, name in [(4, "channels.txt"), (5, "schedules.txt")]:
            self.path(name).unlink()
            out = self.run_menu(menu, 2, 1, "")
            self.assertIn("No channels found" if menu == 4 else "No schedules found", out)
            self.assertTrue(self.path(name).exists())
            self.assertEqual(self.path(name).read_text(), "")

    def test_stale_counters_skip_existing_ids(self):
        self.path("channels_counter.txt").write_text("0")
        self.assertIn("ID: KENH005", self.add_channel())
        self.path("schedules_counter.txt").write_text("0")
        self.assertIn("ID: LPS004", self.add_schedule())
        for name in ["channels.txt", "schedules.txt"]:
            ids = [row[0] for row in self.rows(name)]
            self.assertEqual(len(ids), len(set(ids)))

    def test_last_record_deletes_then_reload_empty(self):
        self.write_rows("packages.txt", [])
        self.write_rows("schedules.txt", [])
        self.write_rows("channels.txt", [self.rows("channels.txt")[-1]])
        self.run_menu(4, 4, "KENH004", "y", "")
        self.assertEqual(self.rows("channels.txt"), [])
        self.assertIn("No channels found", self.run_menu(4, 2, 1, ""))
        self.write_rows("schedules.txt", [
            ["LPS001", "KENH001", "History", "2026-10-16 19:00", "2026-10-16 19:45", ""]])
        self.run_menu(5, 4, "LPS001", "y", "")
        self.assertEqual(self.rows("schedules.txt"), [])
        self.assertIn("No schedules found", self.run_menu(5, 2, 1, ""))

    def test_channel_filters_and_sort_do_not_write_files(self):
        before = self.path("channels.txt").read_bytes()
        self.assertIn("VTV1", self.run_menu(4, 2, 3, "vtv", ""))
        self.assertIn("No channels found", self.run_menu(4, 2, 3, "nonexistent", ""))
        out = self.run_menu(4, 2, 4, "TL004", "")
        self.assertIn("VTV1", out)
        self.assertIn("Unused", out)
        self.assertNotIn("Education", out)
        self.assertIn("No channels found", self.run_menu(4, 2, 5, "0", ""))
        for sort_choice in [1, 2]:
            out = self.run_menu(4, 5, sort_choice, "")
            table_lines = [line for line in out.splitlines() if line.startswith("| KENH")]
            ids = [line.split("|")[1].strip() for line in table_lines]
            expected = ["KENH001", "KENH002", "KENH003", "KENH004"] if sort_choice == 1 else [
                "KENH003", "KENH002", "KENH004", "KENH001"]
            self.assertEqual(ids, expected)
        self.assertEqual(before, self.path("channels.txt").read_bytes())

    def test_schedule_filters_include_midnight_and_sort(self):
        before = self.path("schedules.txt").read_bytes()
        out = self.run_menu(5, 2, 3, "KENH002", "")
        self.assertIn("LPS002", out)
        self.assertNotIn("LPS001", out)
        out = self.run_menu(5, 2, 4, "17/10/2026", "")
        self.assertIn("LPS003", out)
        self.assertNotIn("LPS001", out)
        rows = self.rows("schedules.txt")
        rows[-1][4] = "2026-10-17 00:00"
        self.write_rows("schedules.txt", rows)
        self.assertIn("No schedules found", self.run_menu(5, 2, 4, "17/10/2026", ""))
        self.path("schedules.txt").write_bytes(before)
        out = self.run_menu(5, 5, "")
        ids = [line.split("|")[1].strip() for line in out.splitlines() if line.startswith("| LPS")]
        self.assertEqual(ids, ["LPS001", "LPS002", "LPS003"])
        self.assertEqual(before, self.path("schedules.txt").read_bytes())

    def test_unknown_ids(self):
        for menu, prefix, name in [(4, "KENH", "Channel"), (5, "LPS", "Schedule")]:
            for action in [2, 3, 4]:
                steps = [action, 2, prefix + "999", ""] if action == 2 else [action, prefix + "999", ""]
                self.assertIn(name + " not found", self.run_menu(menu, *steps))

    def test_package_references_with_spaces_and_lowercase_still_block_delete(self):
        self.write_rows("packages.txt", [["GOI001", "Spaced", "10", "DANG_CUNG_CAP", "kenh001, kenh004 ", "VOD001,VOD002"]])
        before = self.path("channels.txt").read_bytes()
        self.assertIn("referenced by Package GOI001", self.run_menu(4, 4, "KENH004", "y", ""))
        self.assertEqual(before, self.path("channels.txt").read_bytes())

    def test_invalid_package_row_is_warned_and_skipped(self):
        self.write_rows("packages.txt", [["GOI001", "Invalid", "10", "DANG_CUNG_CAP", "khenh", "VOD001,VOD002"]])
        out = self.run_menu(4, 4, "KENH004", "y", "", "")
        self.assertIn("[Warning]", out)
        self.assertIn("Channel deleted successfully", out)
        self.assertNotIn("KENH004", [row[0] for row in self.rows("channels.txt")])

    def test_delete_reads_package_changes_after_confirmation_prompt(self):
        log = self.work / "live-console.log"
        with log.open("w", encoding="utf-8") as out:
            proc = subprocess.Popen([str(EXE)], cwd=self.work, stdin=subprocess.PIPE,
                                    stdout=out, stderr=subprocess.STDOUT, text=True, encoding="utf-8")
            try:
                proc.stdin.write("4\n4\nKENH004\n")
                proc.stdin.flush()
                deadline = time.monotonic() + 6
                while time.monotonic() < deadline:
                    if "Confirm deletion?" in log.read_text(encoding="utf-8", errors="replace"):
                        break
                    time.sleep(0.02)
                else:
                    self.fail("Did not reach delete confirmation prompt")
                self.write_rows("packages.txt", [["GOI001", "New reference", "10", "DANG_CUNG_CAP", "KENH001,KENH004", "VOD001,VOD002"]])
                proc.stdin.write("y\n\n0\n0\n")
                proc.stdin.flush()
                proc.stdin.close()
                self.assertEqual(proc.wait(timeout=6), 0)
            finally:
                if proc.poll() is None:
                    proc.kill()
                    proc.wait()
        output = log.read_text(encoding="utf-8", errors="replace")
        self.assertIn("referenced by Package GOI001", output)
        self.assertEqual(len(self.rows("channels.txt")), 4)

    def test_submission_samples_have_ten_records_and_valid_links(self):
        def rows(name):
            return [line.split("|") for line in (ROOT / "Data" / name).read_text(encoding="utf-8").splitlines() if line]
        channels, schedules = rows("channels.txt"), rows("schedules.txt")
        categories = {row[0] for row in rows("channel_categories.txt")}
        self.assertGreaterEqual(len(channels), 10)
        self.assertGreaterEqual(len(schedules), 10)
        self.assertEqual(len({row[0] for row in channels}), len(channels))
        self.assertEqual(len({row[3] for row in channels}), len(channels))
        channel_ids = {row[0] for row in channels}
        for row in channels: self.assertIn(row[2], categories)
        for row in schedules:
            self.assertIn(row[1], channel_ids)
            self.assertLess(row[3], row[4])
        for i, a in enumerate(schedules):
            for b in schedules[i+1:]:
                if a[1] == b[1]:
                    self.assertFalse(a[3] < b[4] and b[3] < a[4])
        self.assertGreaterEqual(int((ROOT / "Data/channels_counter.txt").read_text()),
                                max(int(row[0][4:]) for row in channels))
        self.assertGreaterEqual(int((ROOT / "Data/schedules_counter.txt").read_text()),
                                max(int(row[0][3:]) for row in schedules))


if __name__ == "__main__":
    if not EXE.is_file():
        raise SystemExit("Build first: build.ps1 -OutputName TV36-complete.exe")
    before = {p.name: hashlib.sha256(p.read_bytes()).hexdigest()
              for p in (ROOT / "Data").iterdir() if p.is_file()}
    result = unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(CrudTests))
    after = {p.name: hashlib.sha256(p.read_bytes()).hexdigest()
             for p in (ROOT / "Data").iterdir() if p.is_file()}
    if before != after:
        raise SystemExit("FAIL: project Data changed!")
    print("Project Data unchanged; all menu tests used temporary copies.")
    raise SystemExit(0 if result.wasSuccessful() else 1)
