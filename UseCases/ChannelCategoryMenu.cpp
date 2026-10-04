#include "ChannelCategoryMenu.h"
#include "Entities/ChannelCategory.h"
#include "Utils/SystemHelper.h"
#include "Utils/StringHelper.h"
#include "Utils/TablePrinter.h"

using namespace std;

ChannelCategoryMenu::ChannelCategoryMenu() : repo("Data/channel_categories.txt", "Data/channel_categories_counter.txt", "TL")
{
    repo.loadFromFile();
}

void ChannelCategoryMenu::createChannelCategory()
{
    SystemHelper::clearScreen();
    cout << "Enter channel category informations:" << endl;
    string name = InputHelper::getInputString("Category's name:");
    string description = InputHelper::getInputOptionalString("Category's description:");

    ChannelCategory temp(repo.generateNextID(), name, description);
    if(repo.create(temp))
    {
        repo.writeToFile();
        LOG("[ChannelCategoryMenu] Category created successfully");
        cout << "Category created successfulley\n";
    }
    else
    {
        LOG("[ChannelCategoryMenu] Category created unsuccessfully");
        cout << "Category creation failed, abort\n";
    }
}

void ChannelCategoryMenu::readChannelCategory()
{
    SystemHelper::clearScreen();
    vector<int> widths = {5, 15, 50};
    vector<string> headers = {"ID", "Name", "Description"};

    cout << "=====View categories=====" << endl;
    cout << "1. View all categories" << endl;
    cout << "2. Search by ID" << endl;
    int choice = InputHelper::getInputInt("Select an option (enter 0 to go back): ");

    if (choice == 1)
    {
        TablePrinter::printTable(widths, headers, repo.exportToVector());
        LOG("[ChannelCategoryMenu] Category table printed successfully");
    }
    else if (choice == 2)
    {
        string inputID = convertToUpper(InputHelper::getInputString("Enter Category ID: "));
        ChannelCategory *Category = repo.readByID(inputID);
        if (Category == nullptr)
        {
            cout << "[Error] Category not found, ID: " << inputID << endl;
        }
        else
        {
            TablePrinter::printSingle(widths, headers, Category->exportToVector());
        }
        LOG("[ChannelCategoryMenu] Category search completed");
    }
    InputHelper::getInputOptionalString("Press Enter to go back");
}

void ChannelCategoryMenu::updateChannelCategory()
{
    SystemHelper::clearScreen();
    cout << "\n=====Update category=====\n";
    vector<int> widths = {5, 10, 50};
    vector<string> headers = {"ID", "Name", "Description"};
    TablePrinter::printTable(widths,headers,repo.exportToVector());

    string inputID = convertToUpper(InputHelper::getInputString("Enter Category ID to edit:"));
    ChannelCategory *category = repo.readByID(inputID);

    if (category == nullptr)
    {
        cout << "[Error] Category not found, ID: " << inputID << endl;
        return;
    }
    SystemHelper::clearScreen();
    cout << "=====Category current information=====" << endl;
    TablePrinter::printSingle(widths, headers, category->exportToVector());

    cout << "Enter Category new information (skip to keep it as it is)\n";

    string name = InputHelper::getInputOptionalString("Category name:");
    string description = InputHelper::getInputOptionalString("Description:");

    if(name !="")
    {
        category->setName(name);
    }
    if(description !="")
    {
        category->setDescription(description);
    }

    repo.writeToFile();
    LOG("[CategoryMenu] Category information updated successfully");
}

void ChannelCategoryMenu::deleteChannelCategory()
{
    cout << "\n=====Delete category=====\n";
    
    vector<int> widths = {5, 10, 50};
    vector<string> headers = {"ID", "Name", "Description"};
    TablePrinter::printTable(widths, headers, repo.exportToVector());

    string targetID = convertToUpper(InputHelper::getInputString("Enter Category ID to delete: "));

    // 2. Tìm kiếm trong Repo
    ChannelCategory* category = repo.readByID(targetID);
    if (category == nullptr)
    {
        cout << "[Error] Category not found: " << targetID << endl;
        return;
    }

    cout << "You are about to remove Category:" << endl;
    TablePrinter::printSingle(widths, headers, category->exportToVector());

    // =====================================================================
    // [PLACEHOLDER] Delete conditions
    // =====================================================================

    // =====================================================================

    string confirm = InputHelper::getInputString("Confirm deletion? (y/n): ");
    
    if (confirm == "y" || confirm == "Y")
    {
        if (repo.remove(targetID))
        {
            repo.writeToFile();
            cout << "[ChannelCategoryMenu] Category deleted successfully, ID: " << targetID << endl;
            LOG("[ChannelCategoryMenu] Category deleted successfully");
        }
        else
        {
            cout << "[Error] Category deletion failed, please check debug!" << endl;
        }
    }
    else
    {
        cout << "[Abort] Deletion aborted" << endl;
        LOG("[ChannelCategoryMenu] Delete operation cancelled by user");
    }
}

void ChannelCategoryMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "=====Channel Category Menu=====\n";
    cout << "1. Create a Category\n";
    cout << "2. Read Category\n";
    cout << "3. Update a Category\n";
    cout << "4. Delete a Category\n";
}

void ChannelCategoryMenu::handleChoice(int choice)
{
    switch (choice)
        {
        case 1:
            createChannelCategory();
            break;
        case 2:
            readChannelCategory();
            break;
        case 3:
            updateChannelCategory();
            break;
        case 4:
            deleteChannelCategory();
            break;
        default:
            cout << "[Error] Invalid option in Channel Category menu\n";
        }
}