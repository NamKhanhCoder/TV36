#include "UseCases/PackageMenu.h"
#include "Utils/Debugger.h"
#include "Utils/TablePrinter.h"
#include "Utils/SystemHelper.h"
#include "Utils/InputHelper.h"
#include "Utils/StringHelper.h"
#include "Utils/Validator.h"
#include <iostream>
#include <vector>
#include <set>
#include <cctype>

using namespace std;

namespace {
    string trimString(const string &s)
    {
        size_t start = 0;
        while (start < s.size() && isspace(static_cast<unsigned char>(s[start])))
        {
            start++;
        }
        size_t end = s.size();
        while (end > start && isspace(static_cast<unsigned char>(s[end - 1])))
        {
            end--;
        }
        return s.substr(start, end - start);
    }
}

PackageMenu::PackageMenu() 
    : repo("Data/packages.txt", "Data/packages_counter.txt", "GOI"),
      repoChannel("Data/channels.txt", "Data/channels_counter.txt", "KENH"),
      repoVOD("Data/noidungvod.txt", "Data/vod_counter.txt", "VOD"),
      repoContract("Data/contracts.txt", "Data/contracts_counter.txt", "HD")
{
    repo.loadFromFile();
    repoChannel.loadFromFile();
    repoVOD.loadFromFile();
    repoContract.loadFromFile();
}

bool PackageMenu::isPackageInActiveContract(const string &packageID)
{
    repoContract.loadFromFile();
    for (const auto &c : repoContract.readAll())
    {
        if (c.get_packageID() == packageID && c.get_status() == ContractStatus::ACTIVE)
        {
            return true;
        }
    }
    return false;
}

PackageStatus PackageMenu::inputPackageStatus(bool isOptional, PackageStatus currentStatus)
{
    while (true)
    {
        cout << "\nSelect Package status:" << endl;
        cout << "1. DANG_CUNG_CAP (Dang cung cap)" << endl;
        cout << "2. NGUNG_DANG_KY (Ngung dang ky)" << endl;
        if (isOptional)
        {
            cout << "(Press Enter to skip, keep current: " << packageStatusToString(currentStatus) << ")" << endl;
        }

        string input = InputHelper::getInputOptionalString("Choice: ");
        if (isOptional && input.empty())
        {
            return currentStatus;
        }
        if (input == "1") return PackageStatus::DANG_CUNG_CAP;
        if (input == "2") return PackageStatus::NGUNG_DANG_KY;

        cout << "Invalid choice! Please try again." << endl;
    }
}

vector<string> PackageMenu::inputChannelIDs(bool isOptional, const vector<string> &currentIDs)
{
    repoChannel.loadFromFile();
    while (true)
    {
        string prompt = isOptional 
            ? "Enter Channel IDs separated by comma (Press Enter to keep current): " 
            : "Enter Channel IDs separated by comma (e.g. KENH001,KENH002): ";
        
        string channelsStr = isOptional ? InputHelper::getInputOptionalString(prompt) : InputHelper::getInputString(prompt);
        
        if (isOptional && channelsStr.empty())
        {
            return currentIDs;
        }

        vector<string> rawList = splitString(channelsStr, ',');
        vector<string> validList;
        set<string> seen;
        bool allValid = true;

        for (string id : rawList)
        {
            string upperID = convertToUpper(trimString(id));
            if (upperID.empty()) continue;

            if (!Validator::isValidID(upperID, "KENH", static_cast<short>(upperID.length())))
            {
                cout << "[Error] ID '" << upperID << "' has invalid format! (Must start with 'KENH' followed by numbers)\n";
                allValid = false;
                break;
            }

            if (repoChannel.readByID(upperID) == nullptr)
            {
                cout << "[Error] Channel ID '" << upperID << "' does not exist in repository!\n";
                allValid = false;
                break;
            }

            if (seen.find(upperID) != seen.end())
            {
                cout << "[Error] Duplicate Channel ID '" << upperID << "' in list!\n";
                allValid = false;
                break;
            }

            seen.insert(upperID);
            validList.push_back(upperID);
        }

        if (!allValid)
        {
            cout << "Please re-enter all Channel IDs.\n\n";
            continue;
        }

        if (validList.size() <= 1)
        {
            cout << "[Error] Package must contain more than 1 channel (> 1 channel)!\n\n";
            continue;
        }

        return validList;
    }
}

vector<string> PackageMenu::inputVodIDs(bool isOptional, const vector<string> &currentIDs)
{
    repoVOD.loadFromFile();
    while (true)
    {
        string prompt = isOptional 
            ? "Enter VOD IDs separated by comma (Press Enter to keep current): " 
            : "Enter VOD IDs separated by comma (e.g. VOD001,VOD002): ";
        
        string vodsStr = isOptional ? InputHelper::getInputOptionalString(prompt) : InputHelper::getInputString(prompt);
        
        if (isOptional && vodsStr.empty())
        {
            return currentIDs;
        }

        vector<string> rawList = splitString(vodsStr, ',');
        vector<string> validList;
        set<string> seen;
        bool allValid = true;

        for (string id : rawList)
        {
            string upperID = convertToUpper(trimString(id));
            if (upperID.empty()) continue;

            if (!Validator::isValidID(upperID, "VOD", static_cast<short>(upperID.length())))
            {
                cout << "[Error] ID '" << upperID << "' has invalid format! (Must start with 'VOD' followed by numbers)\n";
                allValid = false;
                break;
            }

            if (repoVOD.readByID(upperID) == nullptr)
            {
                cout << "[Error] VOD ID '" << upperID << "' does not exist in repository!\n";
                allValid = false;
                break;
            }

            if (seen.find(upperID) != seen.end())
            {
                cout << "[Error] Duplicate VOD ID '" << upperID << "' in list!\n";
                allValid = false;
                break;
            }

            seen.insert(upperID);
            validList.push_back(upperID);
        }

        if (!allValid)
        {
            cout << "Please re-enter all VOD IDs.\n\n";
            continue;
        }

        if (validList.size() <= 1)
        {
            cout << "[Error] Package must contain more than 1 VOD (> 1 VOD)!\n\n";
            continue;
        }

        return validList;
    }
}

void PackageMenu::createPackage()
{
    SystemHelper::clearScreen();
    cout << "===== Create New Package =====" << endl;

    Package temp;

    // 1. Name with setter try-catch
    while (true)
    {
        string name = InputHelper::getInputString("Package Name: ");
        try
        {
            temp.setName(name);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[PackageMenu] Invalid Name input during creation: " + string(e.what()));
        }
    }
    
    // 2. Monthly Price with setter try-catch
    while (true)
    {
        long long monthlyPrice = static_cast<long long>(InputHelper::getInputDouble("Monthly Price (VND > 0): "));
        try
        {
            temp.setMonthlyPrice(monthlyPrice);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[PackageMenu] Invalid Monthly Price input during creation: " + string(e.what()));
        }
    }

    // 3. Status with setter try-catch
    PackageStatus status = inputPackageStatus(false);
    try
    {
        temp.setStatus(status);
    }
    catch (const exception &e)
    {
        cout << "[Validation Error] " << e.what() << endl;
    }

    // 4. Channels configuration with setter try-catch
    cout << "\nChannels configuration (Must have > 1 channel and all must exist):\n";
    while (true)
    {
        vector<string> channelIDs = inputChannelIDs(false);
        try
        {
            temp.setChannelIDs(channelIDs);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[PackageMenu] Invalid Channel IDs input during creation: " + string(e.what()));
        }
    }

    // 5. VODs configuration with setter try-catch
    cout << "\nVODs configuration (Must have > 1 VOD and all must exist):\n";
    while (true)
    {
        vector<string> vodIDs = inputVodIDs(false);
        try
        {
            temp.setVodIDs(vodIDs);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[PackageMenu] Invalid VOD IDs input during creation: " + string(e.what()));
        }
    }

    Package newPkg(repo.generateNextID(), temp.getName(), temp.getMonthlyPrice(), temp.getStatus(), temp.getChannelIDs(), temp.getVodIDs());
    if (repo.create(newPkg))
    {
        repo.writeToFile();
        LOG("[PackageMenu] Package created successfully, ID: " + newPkg.getID());
        cout << "Package created successfully! ID: " << newPkg.getID() << endl;
    }
    else
    {
        cout << "[Error] Package creation failed!\n";
    }

    InputHelper::getInputOptionalString("Press Enter to continue...");
}

void PackageMenu::readPackage()
{
    SystemHelper::clearScreen();
    cout << "===== View Packages =====" << endl;
    cout << "1. View all Packages" << endl;
    cout << "2. Search Package by ID" << endl;

    vector<int> widths = {8, 20, 15, 16, 25, 20};
    vector<string> headers = {"ID", "Name", "Monthly Price", "Status", "Channels", "VODs"};

    int choice = InputHelper::getInputInt("Select an option (0 to go back): ");
    if (choice == 1)
    {
        SystemHelper::clearScreen();
        cout << "===== All Packages =====\n";
        TablePrinter::printTable(widths, headers, repo.exportToVector());
        LOG("[PackageMenu] Package table printed successfully");
    }
    else if (choice == 2)
    {
        string inputID = convertToUpper(InputHelper::getInputString("Enter Package ID to search: "));
        Package *pkg = repo.readByID(inputID);
        SystemHelper::clearScreen();
        if (pkg == nullptr)
        {
            cout << "[Error] Package not found, ID: " << inputID << endl;
        }
        else
        {
            cout << "===== Package Detail =====\n";
            TablePrinter::printSingle(widths, headers, pkg->exportToVector());
        }
        LOG("[PackageMenu] Search package completed");
    }

    InputHelper::getInputOptionalString("Press Enter to continue...");
}

void PackageMenu::updatePackage()
{
    SystemHelper::clearScreen();
    cout << "===== Update Package =====" << endl;
    vector<int> widths = {8, 20, 15, 16, 25, 20};
    vector<string> headers = {"ID", "Name", "Monthly Price", "Status", "Channels", "VODs"};
    TablePrinter::printTable(widths, headers, repo.exportToVector());

    string inputID = convertToUpper(InputHelper::getInputString("Enter Package ID to update: "));
    Package *pkg = repo.readByID(inputID);

    if (pkg == nullptr)
    {
        cout << "[Error] Package not found, ID: " << inputID << endl;
        InputHelper::getInputOptionalString("Press Enter to continue...");
        return;
    }

    SystemHelper::clearScreen();
    cout << "===== Current Package Information =====" << endl;
    TablePrinter::printSingle(widths, headers, pkg->exportToVector());

    bool inActiveContract = isPackageInActiveContract(pkg->getID());
    if (inActiveContract)
    {
        cout << "\n[Notice] This package is currently in an ACTIVE contract.\n";
        cout << "[Rule] You can ONLY change its Status.\n\n";

        PackageStatus newStatus = inputPackageStatus(true, pkg->getStatus());
        try
        {
            pkg->setStatus(newStatus);
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << endl;
        }
    }
    else
    {
        cout << "\nLeave field empty (Press Enter) to keep current value.\n";

        while (true)
        {
            string newName = InputHelper::getInputOptionalString("New Name [" + pkg->getName() + "]: ");
            if (newName.empty())
                break;
            try
            {
                pkg->setName(newName);
                break;
            }
            catch (const exception &e)
            {
                cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            }
        }

        while (true)
        {
            string newPriceStr = InputHelper::getInputOptionalString("New Monthly Price [" + to_string(pkg->getMonthlyPrice()) + "]: ");
            if (newPriceStr.empty())
                break;
            try
            {
                long long newPrice = stoll(newPriceStr);
                pkg->setMonthlyPrice(newPrice);
                break;
            }
            catch (const exception &e)
            {
                cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            }
        }

        PackageStatus newStatus = inputPackageStatus(true, pkg->getStatus());
        try
        {
            pkg->setStatus(newStatus);
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << endl;
        }

        cout << "\nNew Channels [" << pkg->getChannelIDsString() << "]:\n";
        while (true)
        {
            vector<string> newChannels = inputChannelIDs(true, pkg->getChannelIDs());
            try
            {
                pkg->setChannelIDs(newChannels);
                break;
            }
            catch (const exception &e)
            {
                cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            }
        }

        cout << "\nNew VODs [" << pkg->getVodIDsString() << "]:\n";
        while (true)
        {
            vector<string> newVods = inputVodIDs(true, pkg->getVodIDs());
            try
            {
                pkg->setVodIDs(newVods);
                break;
            }
            catch (const exception &e)
            {
                cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            }
        }
    }

    if (repo.update(*pkg))
    {
        repo.writeToFile();
        LOG("[PackageMenu] Package updated successfully, ID: " + pkg->getID());
        cout << "Package updated successfully!\n";
    }
    else
    {
        cout << "[Error] Update failed!\n";
    }

    InputHelper::getInputOptionalString("Press Enter to continue...");
}

void PackageMenu::deletePackage()
{
    SystemHelper::clearScreen();
    cout << "===== Delete Package =====" << endl;
    vector<int> widths = {8, 20, 15, 16, 25, 20};
    vector<string> headers = {"ID", "Name", "Monthly Price", "Status", "Channels", "VODs"};
    TablePrinter::printTable(widths, headers, repo.exportToVector());

    string targetID = convertToUpper(InputHelper::getInputString("Enter Package ID to delete: "));
    Package *pkg = repo.readByID(targetID);

    if (pkg == nullptr)
    {
        cout << "[Error] Package not found, ID: " << targetID << endl;
        InputHelper::getInputOptionalString("Press Enter to continue...");
        return;
    }

    SystemHelper::clearScreen();
    cout << "You are about to delete Package:" << endl;
    TablePrinter::printSingle(widths, headers, pkg->exportToVector());

    // Check Removal dependency: Must not be in any active contract
    if (isPackageInActiveContract(targetID))
    {
        cout << "\n[Error] Cannot delete Package " << targetID << " because it is currently assigned to an ACTIVE contract!\n";
        cout << "Please update or terminate the active contract before deleting this package.\n";
        InputHelper::getInputOptionalString("Press Enter to continue...");
        return;
    }

    string confirm = InputHelper::getInputString("Are you sure you want to delete this Package? (y/n): ");
    if (confirm == "y" || confirm == "Y")
    {
        if (repo.remove(targetID))
        {
            repo.writeToFile();
            LOG("[PackageMenu] Package deleted successfully, ID: " + targetID);
            cout << "Package deleted successfully!\n";
        }
        else
        {
            cout << "[Error] Failed to delete Package!\n";
        }
    }
    else
    {
        cout << "Deletion cancelled.\n";
        LOG("[PackageMenu] Package deletion cancelled by user");
    }

    InputHelper::getInputOptionalString("Press Enter to continue...");
}

void PackageMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "===== Package Management =====" << endl;
    cout << "1. Add Package" << endl;
    cout << "2. View Packages" << endl;
    cout << "3. Update Package" << endl;
    cout << "4. Delete Package" << endl;
}

void PackageMenu::handleChoice(int choice)
{
    switch (choice)
    {
    case 1:
        createPackage();
        break;
    case 2:
        readPackage();
        break;
    case 3:
        updatePackage();
        break;
    case 4:
        deletePackage();
        break;
    default:
        cout << "Invalid choice! Press Enter to try again." << endl;
        InputHelper::getInputOptionalString("");
        break;
    }
}
