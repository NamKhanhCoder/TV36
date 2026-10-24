// Nguyen Hong Anh - Quan ly lich thuong va lich VOD (HOURLY).
#include "ScheduleMenu.h"
#include "Utils/StringHelper.h"
#include "Utils/TablePrinter.h"
#include "Utils/LinkedDataWriter.h"
#include <algorithm>
#include <stdexcept>

namespace
{
    const vector<int> widths = {10, 10, 10, 10, 24, 18, 18, 28};
    const vector<string> headers = {"ID", "Type", "VOD ID", "Channel", "Program", "Start", "End", "Description"};

    vector<string> row(const Schedule& item)
    {
        return {item.getID(), item.getVodID().empty() ? "TV" : "VOD",
                item.getVodID(), item.getChannelID(), item.getProgramName(),
                item.getStartTime(), item.getEndTime(), item.getDescription()};
    }

    void printDetail(const Schedule& item)
    {
        vector<string> values = row(item);
        TablePrinter::printSingle(widths, headers, values);
        for (size_t i = 0; i < headers.size(); ++i)
            cout << headers[i] << ": " << values[i] << '\n';
    }

    void printSchedules(const vector<Schedule>& items)
    {
        vector<vector<string>> rows;
        for (const auto& item : items) rows.push_back(row(item));
        if (rows.empty()) cout << "No schedules found.\n";
        else TablePrinter::printTable(widths, headers, rows);
    }

    string inputDateTime(const string& prompt, bool optional)
    {
        while (true)
        {
            string value = InputHelper::getInputOptionalString(prompt + " (0 to cancel): ");
            if (value == "0" || (optional && value.empty())) return value;
            try { return Schedule::normalizeDateTime(value); }
            catch (const invalid_argument& error) { cout << "[Error] " << error.what() << '\n'; }
        }
    }

    bool eligible(const NoiDungVOD& vod)
    {
        return vod.getTrangThai() == "ACTIVE" && vod.getHinhThucXem() == "HOURLY";
    }

    bool byStart(const Schedule& a, const Schedule& b)
    {
        return a.getStartTime() != b.getStartTime() ? a.getStartTime() < b.getStartTime()
                                                   : a.getID() < b.getID();
    }
}

ScheduleMenu::ScheduleMenu()
    : repo("Data/schedules.txt", "Data/schedules_counter.txt", "LPS"),
      repoChannel("Data/channels.txt", "Data/channels_counter.txt", "KENH"),
      repoVod("Data/noidungvod.txt", "Data/vod_counter.txt", "VOD")
{
    loadSchedules();
}

void ScheduleMenu::loadSchedules()
{
    repo.loadFromFile();
    const auto& list = repo.readAll();
    for (size_t i = 0; i < list.size(); ++i)
        for (size_t j = i + 1; j < list.size(); ++j)
            if (list[i].getChannelID() == list[j].getChannelID() &&
                list[i].getStartTime() < list[j].getEndTime() &&
                list[j].getStartTime() < list[i].getEndTime())
                cout << "[Warning] Overlapping schedules: " << list[i].getID()
                     << " and " << list[j].getID() << '\n';
}

string ScheduleMenu::findConflict(const Schedule& item)
{
    for (const auto& other : repo.readAll())
        if (other.getID() != item.getID() && other.getChannelID() == item.getChannelID() &&
            item.getStartTime() < other.getEndTime() && other.getStartTime() < item.getEndTime())
            return other.getID();
    return "";
}

bool ScheduleMenu::inputFields(Schedule& item, bool editing)
{
    repoChannel.loadFromFile();
    repoVod.loadFromFile();
    const Schedule old = item;
    if (repoChannel.readAll().empty())
    {
        cout << "No channels available. Create a channel first.\n";
        return false;
    }
    if (editing) cout << "Leave fields empty to keep current values.\n";
    bool isVod = !item.getVodID().empty();
    while (true)
    {
        string value = InputHelper::getInputOptionalString(
            "Schedule type (1. TV / 2. VOD; 0 to cancel): ");
        if (value == "0") return false;
        if (editing && value.empty()) break;
        if (value == "1" || value == "2") { isVod = value == "2"; break; }
        cout << "[Error] Choose 1 or 2.\n";
    }

    vector<vector<string>> channels;
    for (const auto& channel : repoChannel.readAll())
        channels.push_back({channel.getID(), channel.getName(), channel.getStatus()});
    TablePrinter::printTable({10, 24, 14}, {"ID", "Channel", "Status"}, channels);
    while (true)
    {
        string id = convertToUpper(InputHelper::getInputOptionalString("Channel ID (0 to cancel): "));
        if (id == "0") return false;
        if (editing && id.empty()) id = old.getChannelID();
        const Channel* channel = repoChannel.readByID(id);
        if (!channel) { cout << "[Error] Channel not found.\n"; continue; }
        if ((!editing || id != old.getChannelID()) && channel->getStatus() != "DANG_PHAT")
        { cout << "[Error] Choose a broadcasting channel (DANG_PHAT).\n"; continue; }
        item.setChannelID(id);
        break;
    }

    if (isVod)
    {
        vector<vector<string>> vods;
        for (const auto& vod : repoVod.readAll())
            if (eligible(vod)) vods.push_back({vod.getID(), vod.getTenNoiDung(), to_string(vod.getThoiLuong())});
        cout << "===== Available ACTIVE / HOURLY VOD =====\n";
        if (!vods.empty()) TablePrinter::printTable({10, 32, 16}, {"VOD ID", "Content", "Duration (min)"}, vods);
        else if (!editing || old.getVodID().empty())
        { cout << "No eligible VOD available.\n"; return false; }
        while (true)
        {
            string id = convertToUpper(InputHelper::getInputOptionalString("VOD ID (0 to cancel): "));
            if (id == "0") return false;
            if (editing && id.empty()) id = old.getVodID();
            const NoiDungVOD* vod = repoVod.readByID(id);
            if (!vod) { cout << "[Error] VOD not found.\n"; continue; }
            if ((!editing || id != old.getVodID()) && !eligible(*vod))
            { cout << "[Error] VOD must be ACTIVE and HOURLY.\n"; continue; }
            item.setVodID(id);
            if (!editing || id != old.getVodID()) item.setProgramName(vod->getTenNoiDung());
            break;
        }
    }
    else
    {
        item.setVodID("");
        while (true)
        {
            string program = InputHelper::getInputOptionalString("Program name (0 to cancel): ");
            if (program == "0") return false;
            if (editing && program.empty()) break;
            try { item.setProgramName(program); break; }
            catch (const invalid_argument& error) { cout << "[Error] " << error.what() << '\n'; }
        }
    }

    while (true)
    {
        string start = inputDateTime("Start yyyy-mm-dd HH:mm", editing);
        if (start == "0") return false;
        if (start.empty()) start = old.getStartTime();
        string end;
        try
        {
            if (isVod)
            {
                const auto* vod = repoVod.readByID(item.getVodID());
                const bool recalculate = !editing || item.getVodID() != old.getVodID() || start != old.getStartTime();
                end = recalculate ? Schedule::endAfterMinutes(start, vod->getThoiLuong()) : old.getEndTime();
                cout << "VOD end time: " << end << '\n';
            }
            else
            {
                end = inputDateTime("End yyyy-mm-dd HH:mm", editing);
                if (end == "0") return false;
                if (end.empty()) end = old.getEndTime();
            }
            item.setTimeRange(start, end);
            const bool placementChanged = !editing || item.getChannelID() != old.getChannelID() ||
                item.getStartTime() != old.getStartTime() || item.getEndTime() != old.getEndTime() ||
                item.getVodID() != old.getVodID();
            if (placementChanged && repoChannel.readByID(item.getChannelID())->getStatus() != "DANG_PHAT")
                throw invalid_argument("Cannot place/reschedule on a stopped channel. Cancel and choose another channel.");
            if (isVod && placementChanged && !eligible(*repoVod.readByID(item.getVodID())))
                throw invalid_argument("VOD must be ACTIVE and HOURLY to place/reschedule. Cancel and choose another VOD.");
            string conflict = findConflict(item);
            if (!conflict.empty()) throw invalid_argument("Schedule overlaps " + conflict + ". Enter another time range.");
            break;
        }
        catch (const invalid_argument& error) { cout << "[Error] " << error.what() << '\n'; }
    }
    while (true)
    {
        string description = InputHelper::getInputOptionalString(
            editing ? "Description (Enter: keep, '-': clear, 0: cancel): " : "Description (optional, 0: cancel): ");
        if (description == "0") return false;
        try
        {
            if (!editing || !description.empty()) item.setDescription(editing && description == "-" ? "" : description);
            break;
        }
        catch (const invalid_argument& error) { cout << "[Error] " << error.what() << '\n'; }
    }
    return true;
}

void ScheduleMenu::createSchedule()
{
    cout << "===== Add Schedule =====\n";
    Schedule draft;
    if (!inputFields(draft, false)) { cout << "Creation cancelled.\n"; return; }
    string id;
    do { id = repo.generateNextID(); } while (repo.readByID(id));
    Schedule item(id, draft.getChannelID(), draft.getProgramName(), draft.getStartTime(),
                  draft.getEndTime(), draft.getDescription(), draft.getVodID());
    const auto before = repo;
    try
    {
        if (!repo.create(item)) throw runtime_error("Could not create schedule.");
        writeLinkedData(repo, "Data/schedules.txt");
    }
    catch (...) { repo = before; throw; }
    cout << "Schedule created successfully! ID: " << id << '\n';
}

void ScheduleMenu::readSchedule()
{
    cout << "1. View all schedules\n2. Search by ID\n3. Filter by channel\n4. Filter by date\n5. Search schedules by VOD ID\n";
    int choice = InputHelper::getInputInt("Select an option (0 to go back): ");
    if (choice == 0) return;
    if (choice == 2)
    {
        string id = InputHelper::getInputID("Schedule ID: ", "LPS");
        const auto* item = repo.readByID(id);
        if (!item) cout << "Schedule not found: " << id << '\n';
        else printDetail(*item);
        return;
    }
    if (choice < 1 || choice > 5) { cout << "Invalid option.\n"; return; }
    string query;
    if (choice == 3) query = InputHelper::getInputID("Channel ID: ", "KENH");
    if (choice == 4)
    {
        string value = InputHelper::getInputDate("Broadcast date").toFileString();
        query = value.substr(6, 4) + "-" + value.substr(3, 2) + "-" + value.substr(0, 2);
    }
    if (choice == 5)
    {
        query = InputHelper::getInputID("VOD ID: ", "VOD");
        repoVod.loadFromFile();
        const auto* vod = repoVod.readByID(query);
        if (!vod) { cout << "VOD not found: " << query << '\n'; return; }
        if (vod->getHinhThucXem() == "NON_HOURLY")
        { cout << "VOD is NON_HOURLY (on demand); it has no broadcast schedule.\n"; return; }
    }
    vector<Schedule> found;
    for (const auto& item : repo.readAll())
    {
        const bool onDate = choice == 4 && item.getStartTime().substr(0, 10) <= query &&
                            item.getEndTime() > query + " 00:00";
        if (choice == 1 || (choice == 3 && item.getChannelID() == query) || onDate ||
            (choice == 5 && item.getVodID() == query)) found.push_back(item);
    }
    if (choice == 5)
    {
        sort(found.begin(), found.end(), byStart);
        if (found.empty()) { cout << "VOD has not been scheduled yet: " << query << '\n'; return; }
    }
    printSchedules(found);
}

void ScheduleMenu::sortSchedules()
{
    auto list = repo.readAll();
    sort(list.begin(), list.end(), byStart);
    printSchedules(list);
}

void ScheduleMenu::updateSchedule()
{
    cout << "===== Update Schedule =====\n";
    printSchedules(repo.readAll());
    string id = InputHelper::getInputID("Schedule ID to update: ", "LPS");
    const auto* found = repo.readByID(id);
    if (!found) { cout << "Schedule not found: " << id << '\n'; return; }
    printDetail(*found);
    Schedule candidate = *found;
    if (!inputFields(candidate, true)) { cout << "Update cancelled.\n"; return; }
    const auto before = repo;
    try
    {
        if (!repo.update(candidate)) throw runtime_error("Could not update schedule.");
        writeLinkedData(repo, "Data/schedules.txt");
    }
    catch (...) { repo = before; throw; }
    cout << "Schedule updated successfully!\n";
}

void ScheduleMenu::deleteSchedule()
{
    cout << "===== Delete Schedule =====\n";
    printSchedules(repo.readAll());
    string id = InputHelper::getInputID("Schedule ID to delete: ", "LPS");
    const auto* item = repo.readByID(id);
    if (!item) { cout << "Schedule not found: " << id << '\n'; return; }
    printDetail(*item);
    while (true)
    {
        string answer = convertToUpper(InputHelper::getInputString("Confirm deletion? (y/n): "));
        if (answer == "N") { cout << "Deletion cancelled.\n"; return; }
        if (answer == "Y") break;
        cout << "[Error] Please enter y or n.\n";
    }
    const auto before = repo;
    try
    {
        if (!repo.remove(id)) throw runtime_error("Could not delete schedule.");
        writeLinkedData(repo, "Data/schedules.txt");
    }
    catch (...) { repo = before; throw; }
    cout << "Schedule deleted successfully!\n";
}

void ScheduleMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "===== Schedule Management (TV / VOD) =====\n"
         << "1. Add Schedule\n2. View/Search Schedule\n3. Update Schedule\n"
         << "4. Delete Schedule\n5. Sort by start time\n";
}

void ScheduleMenu::handleChoice(int choice)
{
    try
    {
        loadSchedules();
        switch (choice)
        {
        case 1: createSchedule(); break;
        case 2: readSchedule(); break;
        case 3: updateSchedule(); break;
        case 4: deleteSchedule(); break;
        case 5: sortSchedules(); break;
        default: cout << "Invalid option.\n"; break;
        }
    }
    catch (const exception& error) { cout << "[Error] " << error.what() << '\n'; }
    InputHelper::getInputOptionalString("Press Enter to continue...");
}
