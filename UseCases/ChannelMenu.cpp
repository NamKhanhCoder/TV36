// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Hong Anh (NotaSpookyGhost)
// Use Case 4: Cai dat CRUD kenh va cac rang buoc phu thuoc
// ===============================================================
#include "ChannelMenu.h"
#include "Utils/StringHelper.h"
#include "Utils/TablePrinter.h"
#include <algorithm>
#include <set>
#include <stdexcept>

namespace
{
    const vector<int> widths = {10, 20, 10, 8, 10, 38, 14};
    const vector<string> headers = {"ID", "Name", "Category", "Number", "Resolution", "Stream URL", "Status"};

    void printDetail(const vector<string>& values)
    {
        TablePrinter::printSingle(widths, headers, values);
        for (size_t i = 0; i < headers.size(); ++i)
            cout << headers[i] << ": " << values[i] << "\n";
    }

    string packageChannelID(const string& value)
    {
        size_t first = value.find_first_not_of(" \t");
        if (first == string::npos) throw runtime_error("Empty channel ID in Package data.");
        string id = convertToUpper(value.substr(first, value.find_last_not_of(" \t") - first + 1));
        if (id.size() < 7 || id.size() > 32767 ||
            !Validator::isValidID(id, "KENH", static_cast<short>(id.size())))
            throw runtime_error("Invalid channel ID in Package data: " + value);
        return id;
    }

}

ChannelMenu::ChannelMenu()
    : repo("Data/channels.txt", "Data/channels_counter.txt", "KENH"),
      repoCategory("Data/channel_categories.txt", "Data/channel_categories_counter.txt", "TL"),
      repoPackage("Data/packages.txt", "Data/packages_counter.txt", "GOI"),
      repoSchedule("Data/schedules.txt", "Data/schedules_counter.txt", "LPS")
{
    loadChannels();
}

void ChannelMenu::loadChannels()
{
    repo.loadFromFile();
    set<int> numbers;
    for (const Channel& channel : repo.readAll())
        if (!numbers.insert(channel.getChannelNumber()).second)
            cout << "[Warning] Duplicate channel number in Data/channels.txt: "
                 << channel.getChannelNumber() << "\n";
}

bool ChannelMenu::isNumberUsed(int number, const string& excludeID)
{
    for (const Channel& channel : repo.readAll())
        if (channel.getID() != excludeID && channel.getChannelNumber() == number)
            return true;
    return false;
}

bool ChannelMenu::inputFields(Channel& item, bool editing)
{
    repoCategory.loadFromFile();
    if (repoCategory.readAll().empty())
    {
        cout << "No categories available. Create a category first.\n";
        return false;
    }
    cout << "Enter 0 at Name to cancel. ";
    if (editing) cout << "Leave fields empty to keep their current values.";
    cout << "\n";
    while (true)
    {
        string name = InputHelper::getInputText("Name (0 to cancel): ", editing);
        if (name == "0") return false;
        try
        {
            if (!name.empty()) item.setName(name);
            break;
        }
        catch (const invalid_argument& error)
        {
            cout << "[Validation Error] " << error.what() << " Please try again.\n";
        }
    }

    cout << "\n===== Available Categories =====\n";
    TablePrinter::printTable({8, 22, 45}, {"ID", "Category", "Description"}, repoCategory.exportToVector());
    while (true)
    {
        string id = InputHelper::getInputID("Category ID (e.g. TL004): ", "TL", editing);
        if (id.empty()) id = item.getCategoryID();
        if (repoCategory.readByID(id))
        {
            try
            {
                item.setCategoryID(id);
                break;
            }
            catch (const invalid_argument& error)
            {
                cout << "[Validation Error] " << error.what() << " Please try again.\n";
                continue;
            }
        }
        cout << "[Error] Category not found: " << id << ". Choose an existing category.\n";
    }

    while (true)
    {
        int number = InputHelper::getInputPositiveInt("Channel number (> 0): ", editing);
        if (number == 0) number = item.getChannelNumber();
        if (!isNumberUsed(number, item.getID()))
        {
            try
            {
                item.setChannelNumber(number);
                break;
            }
            catch (const invalid_argument& error)
            {
                cout << "[Validation Error] " << error.what() << " Please try again.\n";
                continue;
            }
        }
        cout << "[Error] Channel number already exists: " << number << ". Please enter another.\n";
    }
    while (true)
    {
        string resolution = InputHelper::getInputChannelResolution("Resolution (SD/HD/FHD/UHD): ", editing);
        try
        {
            if (!resolution.empty()) item.setResolution(resolution);
            break;
        }
        catch (const invalid_argument& error)
        {
            cout << "[Validation Error] " << error.what() << " Please try again.\n";
        }
    }
    while (true)
    {
        string streamURL = InputHelper::getInputText("Stream URL: ", editing);
        try
        {
            if (!streamURL.empty()) item.setStreamURL(streamURL);
            break;
        }
        catch (const invalid_argument& error)
        {
            cout << "[Validation Error] " << error.what() << " Please try again.\n";
        }
    }
    while (true)
    {
        string status = InputHelper::getInputChannelStatus("Trang thai kenh", editing);
        try
        {
            if (!status.empty()) item.setStatus(status);
            break;
        }
        catch (const invalid_argument& error)
        {
            cout << "[Validation Error] " << error.what() << " Please try again.\n";
        }
    }
    return true;
}

void ChannelMenu::createChannel()
{
    SystemHelper::clearScreen();
    cout << "===== Add Channel =====\n";
    Channel draft;
    if (!inputFields(draft, false)) { cout << "Creation cancelled.\n"; return; }
    string id;
    do { id = repo.generateNextID(); } while (repo.readByID(id));
    Channel item(id, draft.getName(), draft.getCategoryID(), draft.getChannelNumber(),
                 draft.getResolution(), draft.getStreamURL(), draft.getStatus());
    if (!repo.create(item)) throw runtime_error("Could not create channel.");
    repo.writeToFile();
    cout << "Channel created successfully! ID: " << id << "\n";
}

void ChannelMenu::readChannel()
{
    SystemHelper::clearScreen();
    cout << "1. View all channels\n2. Search by ID\n3. Search by name\n"
         << "4. Filter by category\n5. Filter by status\n";
    int choice = InputHelper::getInputInt("Select an option (0 to go back): ");
    if (choice == 0) return;
    if (choice == 2)
    {
        string id = InputHelper::getInputID("Channel ID: ", "KENH");
        const Channel* item = repo.readByID(id);
        if (!item) cout << "Channel not found: " << id << "\n";
        else printDetail(item->exportToVector());
        return;
    }
    if (choice < 1 || choice > 5) { cout << "Invalid option.\n"; return; }
    string query;
    if (choice == 3) query = convertToUpper(InputHelper::getInputText("Name contains: "));
    if (choice == 4) query = InputHelper::getInputID("Category ID: ", "TL");
    if (choice == 5) query = InputHelper::getInputChannelStatus("Trang thai kenh");
    vector<vector<string>> rows;
    for (const Channel& item : repo.readAll())
        if (choice == 1 ||
            (choice == 3 && convertToUpper(item.getName()).find(query) != string::npos) ||
            (choice == 4 && item.getCategoryID() == query) ||
            (choice == 5 && item.getStatus() == query))
            rows.push_back(item.exportToVector());
    if (rows.empty()) cout << "No channels found.\n";
    else TablePrinter::printTable(widths, headers, rows);
}

void ChannelMenu::sortChannels()
{
    cout << "1. Channel number (ascending)\n2. Name (A-Z)\n";
    int choice = InputHelper::getInputInt("Select an option (0 to go back): ");
    if (choice == 0) return;
    if (choice != 1 && choice != 2) { cout << "Invalid option.\n"; return; }
    // Sap xep ban sao de xem, khong doi thu tu du lieu luu.
    vector<Channel> list = repo.readAll();
    sort(list.begin(), list.end(), [choice](const Channel& a, const Channel& b)
    {
        if (choice == 1) return a.getChannelNumber() < b.getChannelNumber();
        string left = convertToUpper(a.getName()), right = convertToUpper(b.getName());
        return left == right ? a.getID() < b.getID() : left < right;
    });
    vector<vector<string>> rows;
    for (const Channel& item : list) rows.push_back(item.exportToVector());
    if (rows.empty()) cout << "No channels found.\n";
    else TablePrinter::printTable(widths, headers, rows);
}

void ChannelMenu::updateChannel()
{
    SystemHelper::clearScreen();
    cout << "===== Update Channel =====\n";
    TablePrinter::printTable(widths, headers, repo.exportToVector());
    string id = InputHelper::getInputID("Channel ID to update: ", "KENH");
    const Channel* found = repo.readByID(id);
    if (!found) { cout << "Channel not found: " << id << "\n"; return; }
    printDetail(found->exportToVector());
    Channel candidate = *found;
    if (!inputFields(candidate, true)) { cout << "Update cancelled.\n"; return; }
    if (!repo.update(candidate)) throw runtime_error("Could not update channel.");
    repo.writeToFile();
    cout << "Channel updated successfully!\n";
}

void ChannelMenu::deleteChannel()
{
    SystemHelper::clearScreen();
    cout << "===== Delete Channel =====\n";
    TablePrinter::printTable(widths, headers, repo.exportToVector());
    string id = InputHelper::getInputID("Channel ID to delete: ", "KENH");
    const Channel* item = repo.readByID(id);
    if (!item) { cout << "Channel not found: " << id << "\n"; return; }
    printDetail(item->exportToVector());
    if (!InputHelper::getInputConfirmation("Confirm deletion? (y/n): "))
    {
        cout << "Deletion cancelled.\n";
        return;
    }
    // Nap lai ngay truoc khi kiem tra, ke ca kenh da NGUNG_PHAT va goi ngung dang ky.
    repoPackage.loadFromFile();
    repoSchedule.loadFromFile();
    bool used = false;
    for (const Package& package : repoPackage.readAll())
    {
        bool contains = false;
        for (const string& reference : package.getChannelIDs())
            if (packageChannelID(reference) == id) contains = true;
        if (contains)
        {
            cout << "[Error] Cannot delete channel: referenced by Package " << package.getID() << ".\n";
            used = true;
        }
    }
    for (const Schedule& schedule : repoSchedule.readAll())
        if (schedule.getChannelID() == id)
        {
            cout << "[Error] Cannot delete channel: referenced by Schedule " << schedule.getID() << ".\n";
            used = true;
        }
    if (used) return;
    if (!repo.remove(id)) throw runtime_error("Could not delete channel.");
    repo.writeToFile();
    cout << "Channel deleted successfully!\n";
}

void ChannelMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "===== Channel Management =====\n";
    cout << "1. Add Channel\n2. View/Search Channel\n3. Update Channel\n"
         << "4. Delete Channel\n5. Sort Channels\n";
}

void ChannelMenu::handleChoice(int choice)
{
    try
    {
        loadChannels(); // Nap lai de thay cac thay doi cua thanh vien khac.
        switch (choice)
        {
        case 1: createChannel(); break;
        case 2: readChannel(); break;
        case 3: updateChannel(); break;
        case 4: deleteChannel(); break;
        case 5: sortChannels(); break;
        default: cout << "Invalid option.\n"; break;
        }
    }
    catch (const exception& error) { cout << "[Error] " << error.what() << "\n"; }
    InputHelper::getInputOptionalString("Press Enter to continue...");
}
