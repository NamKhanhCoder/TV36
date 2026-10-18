#include "ScheduleMenu.h"
#include "Utils/StringHelper.h"
#include "Utils/TablePrinter.h"
#include <fstream>
#include <sstream>
#include <set>
#include <stdexcept>

namespace
{
    const vector<int> widths = {10, 10, 22, 18, 18, 28};
    const vector<string> headers = {"ID", "Channel", "Program", "Start", "End", "Description"};

    string inputField(const string& prompt, bool editing)
    {
        return editing ? InputHelper::getInputOptionalString(prompt)
                       : InputHelper::getInputString(prompt);
    }
}

ScheduleMenu::ScheduleMenu()
    : repo("Data/schedules.txt", "Data/schedules_counter.txt", "LPS"),
      dataLoaded(false)
{
    try
    {
        ifstream file("Data/schedules.txt");
        if (!file) throw runtime_error("Cannot read Data/schedules.txt. Run from the project folder.");
        repo.loadFromFile();
        set<string> ids;
        for (const Schedule& item : repo.readAll())
        {
            if (!ids.insert(item.getID()).second)
                throw runtime_error("Duplicate ID in Data/schedules.txt: " + item.getID());
        }
        dataLoaded = true;
    }
    catch (const exception& error)
    {
        cout << "[Error] " << error.what() << "\n";
        cout << "Menu locked to protect the data file. Fix the file and reopen this menu.\n";
        InputHelper::getInputOptionalString("Press Enter to continue...");
    }
}

// Repository cua nhom tra void khi ghi: doc lai de khong bao thanh cong neu luu loi.
void ScheduleMenu::saveChanges()
{
    repo.writeToFile();
    ifstream file("Data/schedules.txt");
    ostringstream actual;
    actual << file.rdbuf();
    string expected;
    for (const Schedule& item : repo.readAll())
        expected += item.exportToString() + "\n";
    if (!file || file.bad() || actual.str() != expected)
    {
        dataLoaded = false;
        throw runtime_error("Could not verify saving. Check the data file and reopen the menu.");
    }
}

void ScheduleMenu::inputFields(Schedule& item, bool editing)
{
    if (editing) cout << "Leave a field empty to keep its current value.\n";
    string channelID = InputHelper::getInputID("Channel ID (e.g. KENH001): ", "KENH", editing);
    if (!channelID.empty()) item.setChannelID(channelID);

    string programName = inputField("Program name: ", editing);
    if (!programName.empty()) item.setProgramName(programName);

    string start = inputField("Start (yyyy-mm-dd HH:mm): ", editing);
    string end = inputField("End   (yyyy-mm-dd HH:mm): ", editing);
    if (editing && start.empty()) start = item.getStartTime();
    if (editing && end.empty()) end = item.getEndTime();
    item.setTimeRange(start, end);

    string description = InputHelper::getInputOptionalString(
        editing ? "Description (Enter: keep, '-': clear): " : "Description (optional): ");
    if (!editing || !description.empty())
        item.setDescription(editing && description == "-" ? "" : description);

    // TODO integration: kiem tra kenh ton tai/trang thai va lich khong chong nhau.
}

void ScheduleMenu::createSchedule()
{
    SystemHelper::clearScreen();
    cout << "===== Add Schedule =====\n";
    Schedule draft;
    inputFields(draft, false);

    // Chi sinh ID sau khi nhap hop le; bo qua ID da co neu counter cu.
    string id;
    do { id = repo.generateNextID(); } while (repo.readByID(id) != nullptr);
    Schedule item(id, draft.getChannelID(), draft.getProgramName(), draft.getStartTime(), draft.getEndTime(), draft.getDescription());
    if (!repo.create(item)) throw runtime_error("Could not create schedule.");
    saveChanges();
    cout << "Schedule created successfully! ID: " << id << "\n";
}

void ScheduleMenu::readSchedule()
{
    SystemHelper::clearScreen();
    cout << "1. View all schedules\n2. Search by ID\n";
    int choice = InputHelper::getInputInt("Select an option (0 to go back): ");
    if (choice == 0) return;
    if (choice == 1)
    {
        if (repo.readAll().empty()) cout << "No schedules found.\n";
        else TablePrinter::printTable(widths, headers, repo.exportToVector());
    }
    else if (choice == 2)
    {
        string id = convertToUpper(InputHelper::getInputString("Schedule ID: "));
        const Schedule* item = repo.readByID(id);
        if (!item) cout << "Schedule not found: " << id << "\n";
        else
        {
            TablePrinter::printSingle(widths, headers, item->exportToVector());
            // In tung truong de doc duoc noi dung dai bi rut gon trong bang.
            vector<string> values = item->exportToVector();
            for (size_t i = 0; i < headers.size(); ++i)
                cout << headers[i] << ": " << values[i] << "\n";
        }
    }
    else cout << "Invalid option.\n";
}

void ScheduleMenu::updateSchedule()
{
    SystemHelper::clearScreen();
    cout << "===== Update Schedule =====\n";
    TablePrinter::printTable(widths, headers, repo.exportToVector());
    string id = convertToUpper(InputHelper::getInputString("Schedule ID to update: "));
    const Schedule* found = repo.readByID(id);
    if (!found)
    {
        cout << "Schedule not found: " << id << "\n";
        return;
    }
    TablePrinter::printSingle(widths, headers, found->exportToVector());

    // Sua tren ban sao: neu nhap sai thi ban ghi goc van giu nguyen.
    Schedule candidate = *found;
    inputFields(candidate, true);
    if (!repo.update(candidate)) throw runtime_error("Could not update schedule.");
    saveChanges();
    cout << "Schedule updated successfully!\n";
}

void ScheduleMenu::deleteSchedule()
{
    SystemHelper::clearScreen();
    cout << "===== Delete Schedule =====\n";
    TablePrinter::printTable(widths, headers, repo.exportToVector());
    string id = convertToUpper(InputHelper::getInputString("Schedule ID to delete: "));
    const Schedule* item = repo.readByID(id);
    if (!item)
    {
        cout << "Schedule not found: " << id << "\n";
        return;
    }
    TablePrinter::printSingle(widths, headers, item->exportToVector());

    string confirm = InputHelper::getInputString("Confirm deletion? (y/n): ");
    if (confirm != "y" && confirm != "Y")
    {
        cout << "Deletion cancelled.\n";
        return;
    }
    if (!repo.remove(id)) throw runtime_error("Could not delete schedule.");
    saveChanges();
    cout << "Schedule deleted successfully!\n";
}

void ScheduleMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "===== Schedule Management =====\n";
    cout << "1. Add Schedule\n2. View/Search Schedule\n3. Update Schedule\n4. Delete Schedule\n";
    if (!dataLoaded) cout << "[Error] Data unavailable. Enter 0 to return.\n";
}

void ScheduleMenu::handleChoice(int choice)
{
    if (dataLoaded)
    {
        try
        {
            switch (choice)
            {
            case 1: createSchedule(); break;
            case 2: readSchedule(); break;
            case 3: updateSchedule(); break;
            case 4: deleteSchedule(); break;
            default: cout << "Invalid option.\n"; break;
            }
        }
        catch (const exception& error)
        {
            cout << "[Error] " << error.what() << "\n";
        }
    }
    else cout << "Data unavailable. Return and reopen after fixing the file.\n";
    InputHelper::getInputOptionalString("Press Enter to continue...");
}
