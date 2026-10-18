#include "ChannelMenu.h"
#include "Utils/StringHelper.h"
#include "Utils/TablePrinter.h"
#include <fstream>
#include <sstream>
#include <set>
#include <stdexcept>

namespace
{
    const vector<int> widths = {10, 20, 10, 8, 10, 38, 14};
    const vector<string> headers = {"ID", "Name", "Category", "Number", "Resolution", "Stream URL", "Status"};

    string inputField(const string& prompt, bool editing)
    {
        return editing ? InputHelper::getInputOptionalString(prompt)
                       : InputHelper::getInputString(prompt);
    }
}

ChannelMenu::ChannelMenu()
    : repo("Data/channels.txt", "Data/channels_counter.txt", "KENH"),
      dataLoaded(false)
{
    try
    {
        ifstream file("Data/channels.txt");
        if (!file) throw runtime_error("Cannot read Data/channels.txt. Run from the project folder.");
        repo.loadFromFile();
        set<string> ids;
        for (const Channel& item : repo.readAll())
        {
            if (!ids.insert(item.getID()).second)
                throw runtime_error("Duplicate ID in Data/channels.txt: " + item.getID());
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
void ChannelMenu::saveChanges()
{
    repo.writeToFile();
    ifstream file("Data/channels.txt");
    ostringstream actual;
    actual << file.rdbuf();
    string expected;
    for (const Channel& item : repo.readAll())
        expected += item.exportToString() + "\n";
    if (!file || file.bad() || actual.str() != expected)
    {
        dataLoaded = false;
        throw runtime_error("Could not verify saving. Check the data file and reopen the menu.");
    }
}

void ChannelMenu::inputFields(Channel& item, bool editing)
{
    if (editing) cout << "Leave a field empty to keep its current value.\n";
    string name = inputField("Name: ", editing);
    if (!name.empty()) item.setName(name);

    string categoryID = InputHelper::getInputID("Category ID (e.g. TL004): ", "TL", editing);
    if (!categoryID.empty()) item.setCategoryID(categoryID);

    int number = InputHelper::getInputPositiveInt("Channel number (> 0): ", editing);
    if (number != 0) item.setChannelNumber(number);

    string resolution = InputHelper::getInputChannelResolution("Resolution (SD/HD/FHD/UHD): ", editing);
    if (!resolution.empty()) item.setResolution(resolution);

    string streamURL = inputField("Stream URL: ", editing);
    if (!streamURL.empty()) item.setStreamURL(streamURL);

    string status = InputHelper::getInputChannelStatus("Trang thai kenh", editing);
    if (!status.empty()) item.setStatus(status);

    // TODO integration: kiem tra categoryID ton tai va channelNumber khong trung.
}

void ChannelMenu::createChannel()
{
    SystemHelper::clearScreen();
    cout << "===== Add Channel =====\n";
    Channel draft;
    inputFields(draft, false);

    // Chi sinh ID sau khi nhap hop le; bo qua ID da co neu counter cu.
    string id;
    do { id = repo.generateNextID(); } while (repo.readByID(id) != nullptr);
    Channel item(id, draft.getName(), draft.getCategoryID(), draft.getChannelNumber(), draft.getResolution(), draft.getStreamURL(), draft.getStatus());
    if (!repo.create(item)) throw runtime_error("Could not create channel.");
    saveChanges();
    cout << "Channel created successfully! ID: " << id << "\n";
}

void ChannelMenu::readChannel()
{
    SystemHelper::clearScreen();
    cout << "1. View all channels\n2. Search by ID\n";
    int choice = InputHelper::getInputInt("Select an option (0 to go back): ");
    if (choice == 0) return;
    if (choice == 1)
    {
        if (repo.readAll().empty()) cout << "No channels found.\n";
        else TablePrinter::printTable(widths, headers, repo.exportToVector());
    }
    else if (choice == 2)
    {
        string id = convertToUpper(InputHelper::getInputString("Channel ID: "));
        const Channel* item = repo.readByID(id);
        if (!item) cout << "Channel not found: " << id << "\n";
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

void ChannelMenu::updateChannel()
{
    SystemHelper::clearScreen();
    cout << "===== Update Channel =====\n";
    TablePrinter::printTable(widths, headers, repo.exportToVector());
    string id = convertToUpper(InputHelper::getInputString("Channel ID to update: "));
    const Channel* found = repo.readByID(id);
    if (!found)
    {
        cout << "Channel not found: " << id << "\n";
        return;
    }
    TablePrinter::printSingle(widths, headers, found->exportToVector());

    // Sua tren ban sao: neu nhap sai thi ban ghi goc van giu nguyen.
    Channel candidate = *found;
    inputFields(candidate, true);
    if (!repo.update(candidate)) throw runtime_error("Could not update channel.");
    saveChanges();
    cout << "Channel updated successfully!\n";
}

void ChannelMenu::deleteChannel()
{
    SystemHelper::clearScreen();
    cout << "===== Delete Channel =====\n";
    TablePrinter::printTable(widths, headers, repo.exportToVector());
    string id = convertToUpper(InputHelper::getInputString("Channel ID to delete: "));
    const Channel* item = repo.readByID(id);
    if (!item)
    {
        cout << "Channel not found: " << id << "\n";
        return;
    }
    TablePrinter::printSingle(widths, headers, item->exportToVector());
    // TODO integration: chan xoa khi Package hoac Schedule dang tham chieu kenh.

    string confirm = InputHelper::getInputString("Confirm deletion? (y/n): ");
    if (confirm != "y" && confirm != "Y")
    {
        cout << "Deletion cancelled.\n";
        return;
    }
    if (!repo.remove(id)) throw runtime_error("Could not delete channel.");
    saveChanges();
    cout << "Channel deleted successfully!\n";
}

void ChannelMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "===== Channel Management =====\n";
    cout << "1. Add Channel\n2. View/Search Channel\n3. Update Channel\n4. Delete Channel\n";
    if (!dataLoaded) cout << "[Error] Data unavailable. Enter 0 to return.\n";
}

void ChannelMenu::handleChoice(int choice)
{
    if (dataLoaded)
    {
        try
        {
            switch (choice)
            {
            case 1: createChannel(); break;
            case 2: readChannel(); break;
            case 3: updateChannel(); break;
            case 4: deleteChannel(); break;
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
