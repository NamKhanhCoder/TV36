"""Integration checks through the real console menus, using isolated Data copies."""
import hashlib
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXE = ROOT / "build" / "TV36.exe"


class CrudTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="tv36-crud-")
        self.work = Path(self.temp.name)
        shutil.copytree(ROOT / "Data", self.work / "Data")
        self.addCleanup(self.temp.cleanup)

    def run_menu(self, menu, *steps):
        commands = "\n".join(map(str, [menu, *steps, 0, 0])) + "\n"
        log = self.work / "console.log"
        with log.open("w", encoding="utf-8") as out:
            result = subprocess.run(
                [str(EXE)], input=commands, text=True, encoding="utf-8",
                errors="replace", cwd=self.work, stdout=out,
                stderr=subprocess.STDOUT, timeout=8,
            )
        output = log.read_text(encoding="utf-8", errors="replace")
        self.assertEqual(result.returncode, 0, output[-3000:])
        self.assertIn("Cam on ban da su dung he thong", output)
        return output

    def path(self, name):
        return self.work / "Data" / name

    def rows(self, name):
        return [line.split("|") for line in self.path(name).read_text(encoding="utf-8").splitlines()]

    def add_channel(self):
        return self.run_menu(7, 1, "Test Channel", "tl004", "11", "hd",
                             "https://example.com/a-very-long-stream-address-for-testing.m3u8",
                             "dang_phat", "")

    def test_channel_crud_and_restart(self):
        self.assertIn("Channel created successfully! ID: KENH004", self.add_channel())
        row = self.rows("channels.txt")[-1]
        self.assertEqual(row[:5], ["KENH004", "Test Channel", "TL004", "11", "HD"])
        output = self.run_menu(7, 2, 2, "kenh004", "")
        self.assertIn(row[5], output)  # detail shows untruncated URL
        self.assertIn("Test Channel", self.run_menu(7, 2, 1, ""))
        self.run_menu(7, 3, "KENH004", "Renamed", "", "12", "", "", "NGUNG_PHAT", "")
        updated = self.rows("channels.txt")[-1]
        self.assertEqual(updated, ["KENH004", "Renamed", "TL004", "12", "HD", row[5], "NGUNG_PHAT"])
        before = self.path("channels.txt").read_bytes()
        self.assertIn("Deletion cancelled", self.run_menu(7, 4, "KENH004", "n", ""))
        self.assertEqual(self.path("channels.txt").read_bytes(), before)
        self.assertIn("Channel deleted successfully", self.run_menu(7, 4, "KENH004", "y", ""))
        self.assertIn("Channel not found", self.run_menu(7, 2, 2, "KENH004", ""))
        self.assertEqual(len(self.rows("channels.txt")), 3)

    def test_schedule_crud_overnight_and_clear_description(self):
        self.assertIn("Schedule created successfully! ID: LPS004",
                      self.run_menu(8, 1, "kenh001", "Late program",
                                    "2026-10-16 23:30", "2026-10-17 00:30", "First note", ""))
        self.assertIn("Late program", self.run_menu(8, 2, 1, ""))
        # Both new endpoints are after the old interval; update must treat them together.
        self.run_menu(8, 3, "LPS004", "", "New program",
                      "2026-10-18 23:30", "2026-10-19 00:30", "", "")
        row = self.rows("schedules.txt")[-1]
        self.assertEqual(row, ["LPS004", "KENH001", "New program",
                              "2026-10-18 23:30", "2026-10-19 00:30", "First note"])
        self.run_menu(8, 3, "LPS004", "", "", "", "", "-", "")
        self.assertEqual(self.rows("schedules.txt")[-1][-1], "")
        self.assertIn("New program", self.run_menu(8, 2, 2, "lps004", ""))
        before = self.path("schedules.txt").read_bytes()
        self.run_menu(8, 4, "LPS004", "n", "")
        self.assertEqual(self.path("schedules.txt").read_bytes(), before)
        self.run_menu(8, 4, "LPS004", "y", "")
        self.assertIn("Schedule not found", self.run_menu(8, 2, 2, "LPS004", ""))

    def test_invalid_channel_updates_do_not_partially_save(self):
        before = self.path("channels.txt").read_bytes()
        invalid = [
            ("Changed name", "", "0"),
            ("Changed name", "", "12abc"),
            ("Changed name", "", "999999999999999999999"),
            ("Changed name", "", "", "8K"),
            ("Changed name", "", "", "", "", "WRONG"),
            ("Bad|name",),
        ]
        for fields in invalid:
            with self.subTest(fields=fields):
                self.assertIn("[Error]", self.run_menu(7, 3, "KENH001", *fields, ""))
                self.assertEqual(self.path("channels.txt").read_bytes(), before)

    def test_invalid_schedule_updates_do_not_partially_save(self):
        before = self.path("schedules.txt").read_bytes()
        cases = [
            ("2026-02-29 19:00", "2026-03-01 20:00"),
            ("2026-10-16 24:00", "2026-10-17 01:00"),
            ("2026-10-16 19:00", "2026-10-16 19:00"),
            ("2026-10-16 19:00", "2026-10-16 18:00"),
            ("2026-1-16 19:00", "2026-10-16 20:00"),
        ]
        for start, end in cases:
            with self.subTest(start=start, end=end):
                output = self.run_menu(8, 3, "LPS001", "", "Changed program", start, end, "")
                self.assertIn("[Error]", output)
                self.assertEqual(self.path("schedules.txt").read_bytes(), before)
        self.assertIn("[Error]", self.run_menu(8, 3, "LPS001", "WRONG001", ""))
        self.assertEqual(self.path("schedules.txt").read_bytes(), before)

    def test_leap_day_and_empty_description(self):
        self.assertIn("Schedule created successfully! ID: LPS004",
                      self.run_menu(8, 1, "KENH001", "Leap day", "2028-02-29 23:00", "2028-03-01 00:00", "", ""))
        self.assertEqual(self.rows("schedules.txt")[-1][-1], "")
        self.assertIn("Leap day", self.run_menu(8, 2, 2, "LPS004", ""))

    def test_unknown_ids(self):
        for menu, prefix, name in [(7, "KENH", "Channel"), (8, "LPS", "Schedule")]:
            for action in [2, 3, 4]:
                steps = [action, 2, prefix + "999", ""] if action == 2 else [action, prefix + "999", ""]
                self.assertIn(name + " not found", self.run_menu(menu, *steps))

    def test_corrupt_or_duplicate_file_is_not_overwritten(self):
        for menu, name in [(7, "channels.txt"), (8, "schedules.txt")]:
            original = self.path(name).read_bytes()
            for corruption in [b"broken|row\n", original.splitlines()[0] + b"\n"]:
                with self.subTest(name=name, corruption=corruption):
                    self.path(name).write_bytes(original + corruption)
                    before = self.path(name).read_bytes()
                    # Constructor pauses once, then attempted add is blocked and pauses.
                    output = self.run_menu(menu, "", 1, "")
                    self.assertIn("Menu locked", output)
                    self.assertEqual(self.path(name).read_bytes(), before)
            self.path(name).write_bytes(original)

    def test_missing_data_file_is_reported(self):
        self.path("channels.txt").unlink()
        output = self.run_menu(7, "")
        self.assertIn("Cannot read Data/channels.txt", output)
        self.assertFalse(self.path("channels.txt").exists())

    def test_stale_counter_does_not_duplicate_ids(self):
        self.path("channels_counter.txt").write_text("0\n", encoding="utf-8")
        self.add_channel()
        ids = [row[0] for row in self.rows("channels.txt")]
        self.assertEqual(len(ids), len(set(ids)))
        self.assertEqual(ids[-1], "KENH004")

    def test_empty_database_and_invalid_create(self):
        self.path("channels.txt").write_text("", encoding="utf-8")
        self.path("channels_counter.txt").write_text("0\n", encoding="utf-8")
        self.assertIn("No channels found", self.run_menu(7, 2, 1, ""))
        self.assertIn("[Error]", self.run_menu(7, 1, "Invalid|name", ""))
        self.assertEqual(self.path("channels_counter.txt").read_text().strip(), "0")
        self.assertEqual(self.path("channels.txt").read_text(), "")
        self.assertIn("ID: KENH001", self.add_channel())


if __name__ == "__main__":
    if not EXE.is_file():
        raise SystemExit("Build first: powershell -NoProfile -ExecutionPolicy Bypass -File build.ps1")
    before = {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in (ROOT / "Data").iterdir() if p.is_file()}
    suite = unittest.defaultTestLoader.loadTestsFromTestCase(CrudTests)
    result = unittest.TextTestRunner(verbosity=2).run(suite)
    after = {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in (ROOT / "Data").iterdir() if p.is_file()}
    if before != after:
        raise SystemExit("FAIL: project Data files changed!")
    print("Project Data files unchanged; tests used temporary copies.")
    raise SystemExit(0 if result.wasSuccessful() else 1)
