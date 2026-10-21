// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Hong Anh (NotaSpookyGhost)
// Use Case 5: Cai dat CRUD lich va kiem tra trung gio
// ===============================================================
#include "ScheduleMenu.h"
#include "Utils/StringHelper.h"
#include "Utils/TablePrinter.h"
#include <algorithm>
#include <stdexcept>

namespace
{
    const vector<int> widths = {10, 10, 22, 18, 18, 28};
    const vector<string> headers = {"ID", "Channel", "Program", "Start", "End", "Description"};

    // Nguyen Hong Anh: nhap ngay gio ISO la nghiep vu lich, dung Validator chung.
    string inputScheduleDateTime(const string& prompt, bool optional)
    {
        while (true)
        {
            string value = optional ? InputHelper::getInputOptionalString(prompt)
                                    : InputHelper::getInputString(prompt);
            if (optional && value.find_first_not_of(" \t\v\f") == string::npos) return "";
            bool valid = value.size() == 16 && value[4] == '-' && value[7] == '-' &&
                         value[10] == ' ' && value[13] == ':';
            if (valid)
            {
                for (size_t i = 0; i < value.size(); ++i)
                {
                    if (i == 4 || i == 7 || i == 10 || i == 13) continue;
                    if (value[i] < '0' || value[i] > '9') valid = false;
                }
            }
            if (valid && Validator::isValidDate(value.substr(0, 10)) &&
                         Validator::isValidTime(value.substr(11, 5))) return value;
            cout << "[Error] Invalid date/time. Use yyyy-mm-dd HH:mm. Please enter again.\n";
        }
    }

    void printDetail(const vector<string>& values)
    {
        TablePrinter::printSingle(widths, headers, values);
        for (size_t i = 0; i < headers.size(); ++i)
            cout << headers[i] << ": " << values[i] << "\n";
    }

}

ScheduleMenu::ScheduleMenu()
    : repo("Data/schedules.txt", "Data/schedules_counter.txt", "LPS"),
      repoChannel("Data/channels.txt", "Data/channels_counter.txt", "KENH")
{
    loadSchedules();
}

void ScheduleMenu::loadSchedules()
{
    repo.loadFromFile();
    const vector<Schedule>& list = repo.readAll();
    for (size_t i = 0; i < list.size(); ++i)
        for (size_t j = i + 1; j < list.size(); ++j)
            if (list[i].getChannelID() == list[j].getChannelID() &&
                list[i].getStartTime() < list[j].getEndTime() &&
                list[j].getStartTime() < list[i].getEndTime())
                cout << "[Warning] Overlapping schedules in Data/schedules.txt: "
                     << list[i].getID() << " and " << list[j].getID() << "\n";
}

string ScheduleMenu::findConflict(const Schedule& item)
{
    for (const Schedule& other : repo.readAll())
        if (other.getID() != item.getID() && other.getChannelID() == item.getChannelID() &&
            item.getStartTime() < other.getEndTime() && other.getStartTime() < item.getEndTime())
            return other.getID();
    return "";
}

bool ScheduleMenu::inputFields(Schedule& item, bool editing)
{
    repoChannel.loadFromFile();
    if (repoChannel.readAll().empty())
    {
        cout << "No channels available. Create a channel first.\n";
        return false;
    }
    const string oldChannel = item.getChannelID();
    const string oldStart = item.getStartTime(), oldEnd = item.getEndTime();
    cout << "===== Available Channels =====\n";
    vector<vector<string>> channels;
    for (const Channel& channel : repoChannel.readAll())
        channels.push_back({channel.getID(), channel.getName(), channel.getStatus()});
    TablePrinter::printTable({10, 22, 14}, {"ID", "Channel", "Status"}, channels);
    if (editing) cout << "Leave fields empty to keep their current values.\n";
    while (true)
    {
        string id = InputHelper::getInputID("Channel ID (e.g. KENH001): ", "KENH", editing);
        if (id.empty()) id = oldChannel;
        const Channel* channel = repoChannel.readByID(id);
        if (!channel) cout << "[Error] Channel not found: " << id << ". Choose an existing channel.\n";
        else
        {
            try
            {
                item.setChannelID(id);
                break;
            }
            catch (const invalid_argument& error)
            {
                cout << "[Validation Error] " << error.what() << " Please try again.\n";
            }
        }
    }
    while (true)
    {
        string program = editing ? InputHelper::getInputOptionalString("Program name (0 to cancel): ")
                                 : InputHelper::getInputString("Program name (0 to cancel): ");
        if (program == "0") return false;
        if (program.find_first_not_of(" \t\v\f") == string::npos)
        {
            if (editing) break;
            cout << "[Error] Text must not be empty. Please try again.\n";
            continue;
        }
        try
        {
            if (!program.empty()) item.setProgramName(program);
            break;
        }
        catch (const invalid_argument& error)
        {
            cout << "[Validation Error] " << error.what() << " Please try again.\n";
        }
    }

    string start, end;
    while (true)
    {
        start = inputScheduleDateTime("Start (yyyy-mm-dd HH:mm): ", editing);
        if (start.empty()) start = oldStart;
        // Cho nhap lai moc ket thuc, khong mat moc bat dau da nhap dung.
        while (true)
        {
            end = inputScheduleDateTime("End   (yyyy-mm-dd HH:mm): ", editing);
            if (end.empty()) end = oldEnd;
            if (end <= start)
            {
                cout << "[Error] End time must be after start time. Enter the end time again.\n";
                continue;
            }
            try
            {
                item.setTimeRange(start, end);
                break;
            }
            catch (const invalid_argument& error)
            {
                cout << "[Validation Error] " << error.what() << " Enter the end time again.\n";
            }
        }
        string conflict = findConflict(item);
        if (conflict.empty()) break;
        cout << "[Error] Schedule overlaps " << conflict << " on " << item.getChannelID()
             << ". Enter a different start/end range.\n";
    }
    while (true)
    {
        string description = InputHelper::getInputOptionalString(
            editing ? "Description (Enter: keep, '-': clear): " : "Description (optional): ");
        if (description.find_first_not_of(" \t\v\f") == string::npos) description.clear();
        try
        {
            if (!editing || !description.empty())
                item.setDescription(editing && description == "-" ? "" : description);
            break;
        }
        catch (const invalid_argument& error)
        {
            cout << "[Validation Error] " << error.what() << " Please try again.\n";
        }
    }
    return true;
}

void ScheduleMenu::createSchedule()
{
    SystemHelper::clearScreen();
    cout << "===== Add Schedule =====\n";
    Schedule draft;
    if (!inputFields(draft, false)) { cout << "Creation cancelled.\n"; return; }
    string id;
    do { id = repo.generateNextID(); } while (repo.readByID(id));
    Schedule item(id, draft.getChannelID(), draft.getProgramName(), draft.getStartTime(),
                  draft.getEndTime(), draft.getDescription());
    if (!repo.create(item)) throw runtime_error("Could not create schedule.");
    repo.writeToFile();
    cout << "Schedule created successfully! ID: " << id << "\n";
}

void ScheduleMenu::readSchedule()
{
    SystemHelper::clearScreen();
    cout << "1. View all schedules\n2. Search by ID\n3. Filter by channel\n4. Filter by date\n";
    int choice = InputHelper::getInputInt("Select an option (0 to go back): ");
    if (choice == 0) return;
    if (choice == 2)
    {
        string id = InputHelper::getInputID("Schedule ID: ", "LPS");
        const Schedule* item = repo.readByID(id);
        if (!item) cout << "Schedule not found: " << id << "\n";
        else printDetail(item->exportToVector());
        return;
    }
    if (choice < 1 || choice > 4) { cout << "Invalid option.\n"; return; }
    string query;
    if (choice == 3) query = InputHelper::getInputID("Channel ID: ", "KENH");
    if (choice == 4)
    {
        Date date = InputHelper::getInputDate("Broadcast date");
        // Date::toFileString la dd-mm-yyyy: doi thu tu sang yyyy-mm-dd.
        string value = date.toFileString();
        query = value.substr(6, 4) + "-" + value.substr(3, 2) + "-" + value.substr(0, 2);
    }
    vector<vector<string>> rows;
    for (const Schedule& item : repo.readAll())
    {
        // Khoang nua mo [start,end): lich ket thuc dung 00:00 khong tinh vao ngay sau.
        bool onDate = choice == 4 && item.getStartTime().substr(0, 10) <= query &&
                      item.getEndTime() > query + " 00:00";
        if (choice == 1 || (choice == 3 && item.getChannelID() == query) || onDate)
            rows.push_back(item.exportToVector());
    }
    if (rows.empty()) cout << "No schedules found.\n";
    else TablePrinter::printTable(widths, headers, rows);
}

void ScheduleMenu::sortSchedules()
{
    vector<Schedule> list = repo.readAll();
    sort(list.begin(), list.end(), [](const Schedule& a, const Schedule& b)
    {
        if (a.getStartTime() != b.getStartTime()) return a.getStartTime() < b.getStartTime();
        return a.getID() < b.getID();
    });
    vector<vector<string>> rows;
    for (const Schedule& item : list) rows.push_back(item.exportToVector());
    if (rows.empty()) cout << "No schedules found.\n";
    else TablePrinter::printTable(widths, headers, rows);
}

void ScheduleMenu::updateSchedule()
{
    SystemHelper::clearScreen();
    cout << "===== Update Schedule =====\n";
    TablePrinter::printTable(widths, headers, repo.exportToVector());
    string id = InputHelper::getInputID("Schedule ID to update: ", "LPS");
    const Schedule* found = repo.readByID(id);
    if (!found) { cout << "Schedule not found: " << id << "\n"; return; }
    printDetail(found->exportToVector());
    Schedule candidate = *found;
    if (!inputFields(candidate, true)) { cout << "Update cancelled.\n"; return; }
    if (!repo.update(candidate)) throw runtime_error("Could not update schedule.");
    repo.writeToFile();
    cout << "Schedule updated successfully!\n";
}

void ScheduleMenu::deleteSchedule()
{
    SystemHelper::clearScreen();
    cout << "===== Delete Schedule =====\n";
    TablePrinter::printTable(widths, headers, repo.exportToVector());
    string id = InputHelper::getInputID("Schedule ID to delete: ", "LPS");
    const Schedule* item = repo.readByID(id);
    if (!item) { cout << "Schedule not found: " << id << "\n"; return; }
    printDetail(item->exportToVector());
    string confirmation;
    while (true)
    {
        confirmation = convertToUpper(InputHelper::getInputString("Confirm deletion? (y/n): "));
        if (confirmation == "Y" || confirmation == "N") break;
        cout << "[Error] Please enter y or n.\n";
    }
    if (confirmation == "N")
    {
        cout << "Deletion cancelled.\n";
        return;
    }
    if (!repo.remove(id)) throw runtime_error("Could not delete schedule.");
    repo.writeToFile();
    cout << "Schedule deleted successfully!\n";
}

void ScheduleMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "===== Schedule Management =====\n";
    cout << "1. Add Schedule\n2. View/Search Schedule\n3. Update Schedule\n"
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
    catch (const exception& error) { cout << "[Error] " << error.what() << "\n"; }
    InputHelper::getInputOptionalString("Press Enter to continue...");
}
