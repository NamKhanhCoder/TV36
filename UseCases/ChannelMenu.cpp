#include "ChannelMenu.h"
#include "Utils/StringHelper.h"
#include "Utils/TablePrinter.h"
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <set>
#include <stdexcept>

namespace
{
    const vector<int> widths = {10, 20, 10, 8, 10, 38, 14};
    const vector<string> headers = {"ID", "Name", "Category", "Number", "Resolution", "Stream URL", "Status"};

    // Kiem tra file truoc khi goi Repository cua nhom (parser cu co the bo qua dong loi).
    template<class T>
    void loadChecked(Repository<T>& repo, const string& path, size_t fieldCount, const string& prefix)
    {
        ifstream file(path);
        if (!file) throw runtime_error("Cannot read " + path + ". Run from the project folder.");
        set<string> ids;
        string line;
        size_t lineNumber = 0;
        while (getline(file, line))
        {
            ++lineNumber;
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue;
            try
            {
                vector<string> fields = splitString(line);
                T item;
                if (fields.size() != fieldCount || fields[0].size() < prefix.size() + 3 ||
                    fields[0].size() > 32767 ||
                    !Validator::isValidID(fields[0], prefix, static_cast<short>(fields[0].size())))
                    throw invalid_argument("Invalid fields or ID.");
                item.parseFromString(line);
                if (item.exportToString() != line || !ids.insert(item.getID()).second)
                    throw invalid_argument("Invalid data or duplicate ID.");
            }
            catch (const exception& error)
            {
                throw runtime_error(path + " line " + to_string(lineNumber) + ": " + error.what());
            }
        }
        if (file.bad()) throw runtime_error("Read failed: " + path);
        repo.loadFromFile();
    }

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
      repoSchedule("Data/schedules.txt", "Data/schedules_counter.txt", "LPS"),
      dataLoaded(false)
{
    try { loadChannels(); }
    catch (const exception& error)
    {
        cout << "[Error] " << error.what() << "\n";
        cout << "Menu locked to protect the data file. Fix the file and reopen this menu.\n";
        InputHelper::getInputOptionalString("Press Enter to continue...");
    }
}

void ChannelMenu::loadChannels()
{
    dataLoaded = false;
    loadChecked(repo, "Data/channels.txt", 7, "KENH");
    set<int> numbers;
    for (const Channel& channel : repo.readAll())
        if (!numbers.insert(channel.getChannelNumber()).second)
            throw runtime_error("Duplicate channel number in Data/channels.txt: " +
                                to_string(channel.getChannelNumber()));
    dataLoaded = true;
}

bool ChannelMenu::isNumberUsed(int number, const string& excludeID)
{
    for (const Channel& channel : repo.readAll())
        if (channel.getID() != excludeID && channel.getChannelNumber() == number)
            return true;
    return false;
}

void ChannelMenu::saveChanges()
{
    // Ghi file tam cung thu muc, chi thay file goc khi da ghi/close thanh cong.
    // Khong doi Repository chung cua cac thanh vien khac.
    dataLoaded = false;
    const string path = "Data/channels.txt";
    const string pending = path + ".pending";
    string expected;
    for (const Channel& item : repo.readAll()) expected += item.exportToString() + "\n";
    ofstream output(pending, ios::binary | ios::trunc);
    if (!output) throw runtime_error("Cannot write temporary data file: " + pending);
    output << expected;
    output.close();
    if (!output) throw runtime_error("Write failed. Original data file was preserved: " + path);
    std::filesystem::rename(pending, path);
    ifstream input(path, ios::binary);
    ostringstream actual;
    actual << input.rdbuf();
    if (!input || input.bad() || actual.str() != expected)
        throw runtime_error("Could not verify saving. Check the data file and reopen the menu.");
    dataLoaded = true;
}

bool ChannelMenu::inputFields(Channel& item, bool editing)
{
    loadChecked(repoCategory, "Data/channel_categories.txt", 3, "TL");
    if (repoCategory.readAll().empty())
    {
        cout << "No categories available. Create a category first.\n";
        return false;
    }
    cout << "Enter 0 at Name to cancel. ";
    if (editing) cout << "Leave fields empty to keep their current values.";
    cout << "\n";
    string name = InputHelper::getInputText("Name (0 to cancel): ", editing);
    if (name == "0") return false;
    if (!name.empty()) item.setName(name);

    cout << "\n===== Available Categories =====\n";
    TablePrinter::printTable({8, 22, 45}, {"ID", "Category", "Description"}, repoCategory.exportToVector());
    while (true)
    {
        string id = InputHelper::getInputID("Category ID (e.g. TL004): ", "TL", editing);
        if (id.empty()) id = item.getCategoryID();
        if (repoCategory.readByID(id))
        {
            item.setCategoryID(id);
            break;
        }
        cout << "[Error] Category not found: " << id << ". Choose an existing category.\n";
    }

    while (true)
    {
        int number = InputHelper::getInputPositiveInt("Channel number (> 0): ", editing);
        if (number == 0) number = item.getChannelNumber();
        if (!isNumberUsed(number, item.getID()))
        {
            item.setChannelNumber(number);
            break;
        }
        cout << "[Error] Channel number already exists: " << number << ". Please enter another.\n";
    }
    string resolution = InputHelper::getInputChannelResolution("Resolution (SD/HD/FHD/UHD): ", editing);
    if (!resolution.empty()) item.setResolution(resolution);

    string streamURL = InputHelper::getInputText("Stream URL: ", editing);
    if (!streamURL.empty()) item.setStreamURL(streamURL);

    string status = InputHelper::getInputChannelStatus("Trang thai kenh", editing);
    if (!status.empty()) item.setStatus(status);
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
    saveChanges();
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
    saveChanges();
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
    loadChecked(repoPackage, "Data/packages.txt", 6, "GOI");
    loadChecked(repoSchedule, "Data/schedules.txt", 6, "LPS");
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
    saveChanges();
    cout << "Channel deleted successfully!\n";
}

void ChannelMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "===== Channel Management =====\n";
    cout << "1. Add Channel\n2. View/Search Channel\n3. Update Channel\n"
         << "4. Delete Channel\n5. Sort Channels\n";
    if (!dataLoaded) cout << "[Error] Data unavailable. Enter 0 to return.\n";
}

void ChannelMenu::handleChoice(int choice)
{
    if (dataLoaded)
    {
        try
        {
            loadChannels(); // Tranh dung danh sach cu khi mo nhieu lan.
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
    }
    else cout << "Data unavailable. Return and reopen after fixing the file.\n";
    InputHelper::getInputOptionalString("Press Enter to continue...");
}
