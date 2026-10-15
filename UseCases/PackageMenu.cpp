#include "UseCases/PackageMenu.h"
#include "Utils/Debugger.h"
#include "Utils/TablePrinter.h"
#include "Utils/SystemHelper.h"
#include "Utils/InputHelper.h"
#include "Utils/StringHelper.h"
#include <iostream>
#include <vector>

using namespace std;

PackageMenu::PackageMenu() : repo("Data/packages.txt", "Data/packages_counter.txt", "GOI")
{
    repo.loadFromFile();
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
            cout << "0. Skip (Keep current: " << packageStatusToString(currentStatus) << ")" << endl;
        }

        int choice = InputHelper::getInputInt("Choice: ");
        if (isOptional && choice == 0)
        {
            return currentStatus;
        }
        if (choice == 1) return PackageStatus::DANG_CUNG_CAP;
        if (choice == 2) return PackageStatus::NGUNG_DANG_KY;

        cout << "Invalid choice! Please try again." << endl;
    }
}

void PackageMenu::createPackage()
{
    SystemHelper::clearScreen();
    cout << "===== Create New Package =====" << endl;

    string name = InputHelper::getInputString("Package Name: ");
    
    long long monthlyPrice = 0;
    while (true)
    {
        monthlyPrice = static_cast<long long>(InputHelper::getInputDouble("Monthly Price (VND > 0): "));
        if (monthlyPrice > 0)
            break;
        cout << "[Error] Price must be greater than 0! Please enter again.\n";
    }

    PackageStatus status = inputPackageStatus(false);

    cout << "\nEnter Channel IDs separated by comma (e.g. KENH001,KENH002):" << endl;
    string channelsStr = "";
    vector<string> channelIDs;
    while (true)
    {
        channelsStr = InputHelper::getInputString("Channel IDs (at least 1 required): ");
        if (!channelsStr.empty())
        {
            channelIDs = splitString(channelsStr, ',');
            if (!channelIDs.empty() && !channelIDs[0].empty())
                break;
        }
        cout << "[Error] Package must contain at least 1 channel!\n";
    }

    cout << "\nEnter VOD IDs separated by comma (optional, leave empty if none):" << endl;
    string vodsStr = InputHelper::getInputOptionalString("VOD IDs: ");
    vector<string> vodIDs;
    if (!vodsStr.empty())
    {
        vodIDs = splitString(vodsStr, ',');
    }

    Package temp(repo.generateNextID(), name, monthlyPrice, status, channelIDs, vodIDs);
    if (repo.create(temp))
    {
        repo.writeToFile();
        LOG("[PackageMenu] Package created successfully, ID: " + temp.getID());
        cout << "Package created successfully! ID: " << temp.getID() << endl;
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

    cout << "\nLeave field empty to keep current value.\n";

    string newName = InputHelper::getInputOptionalString("New Name [" + pkg->getName() + "]: ");
    if (!newName.empty())
        pkg->setName(newName);

    string newPriceStr = InputHelper::getInputOptionalString("New Monthly Price [" + to_string(pkg->getMonthlyPrice()) + "]: ");
    if (!newPriceStr.empty())
    {
        try {
            long long newPrice = stoll(newPriceStr);
            if (newPrice > 0)
                pkg->setMonthlyPrice(newPrice);
            else
                cout << "[Notice] Invalid price, keeping current price.\n";
        } catch (...) {
            cout << "[Notice] Invalid price format, keeping current price.\n";
        }
    }

    PackageStatus newStatus = inputPackageStatus(true, pkg->getStatus());
    pkg->setStatus(newStatus);

    string newChannelsStr = InputHelper::getInputOptionalString("New Channel IDs [" + pkg->getChannelIDsString() + "]: ");
    if (!newChannelsStr.empty())
    {
        vector<string> channels = splitString(newChannelsStr, ',');
        if (!channels.empty() && !channels[0].empty())
            pkg->setChannelIDs(channels);
    }

    string newVodsStr = InputHelper::getInputOptionalString("New VOD IDs [" + pkg->getVodIDsString() + "] (enter '-' to clear): ");
    if (!newVodsStr.empty())
    {
        if (newVodsStr == "-")
        {
            pkg->setVodIDs({});
        }
        else
        {
            vector<string> vods = splitString(newVodsStr, ',');
            pkg->setVodIDs(vods);
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

    // =====================================================================
    // [PLACEHOLDER] Delete conditions
    // Kiem tra hop dong tham chieu truoc khi xoa se tich hop o IntegrityValidator
    // =====================================================================

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

