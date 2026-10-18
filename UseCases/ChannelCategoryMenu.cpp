#include "ChannelCategoryMenu.h"
#include "Entities/ChannelCategory.h"
#include "Utils/SystemHelper.h"
#include "Utils/StringHelper.h"
#include "Utils/TablePrinter.h"
#include "Utils/InputHelper.h"
#include <iostream>

using namespace std;

// CONSTRUCTOR & INITIALIZATION =====================================================================
ChannelCategoryMenu::ChannelCategoryMenu() : 
    repo("Data/channel_categories.txt", "Data/channel_categories_counter.txt", "TL"),
    repoChannel("Data/channels.txt", "Data/channels_counter.txt", "KENH")
{
    repo.loadFromFile();
    repoChannel.loadFromFile();
}

// TABLE PRINTING HELPER =====================================================================
void ChannelCategoryMenu::printCategoryTable(const vector<vector<string>> &data)
{
    vector<int> widths = {6, 20, 45};
    vector<string> headers = {"ID", "Category Name", "Description"};
    TablePrinter::printTable(widths, headers, data);
}

// VALIDATION HELPERS =====================================================================
bool ChannelCategoryMenu::isNameExisted(const string &name, const string &excludeID)
{
    string target = convertToUpper(name);

    for (const auto &category : repo.readAll())
    {
        if (!excludeID.empty() && category.getID() == excludeID)
            continue;

        if (convertToUpper(category.getName()) == target)
        {
            return true;
        }
    }
    return false;
}

bool ChannelCategoryMenu::hasChannelsInCategory(const string &categoryID)
{
    repoChannel.loadFromFile();
    for (const auto &ch : repoChannel.readAll())
    {
        if (ch.getCategoryID() == categoryID)
        {
            return true;
        }
    }
    return false;
}

// CRUD OPERATIONS =====================================================================
void ChannelCategoryMenu::createChannelCategory()
{
    SystemHelper::clearScreen();
    cout << "=========================================\n";
    cout << "       CREATE NEW CHANNEL CATEGORY\n";
    cout << "=========================================\n";

    string name;
    while (true)
    {
        name = InputHelper::getInputString("Category's name (or 0 to cancel): ");
        if (name == "0")
        {
            cout << "[Notice] Category creation cancelled.\n";
            InputHelper::getInputOptionalString("Press Enter to go back...");
            return;
        }

        if (name.empty())
        {
            cout << "[Error] Category name cannot be empty! Please try again.\n";
            continue;
        }

        if (isNameExisted(name))
        {
            cout << "[Error] Category name already exists: \"" << name << "\". Please choose another name.\n";
            continue;
        }

        break;
    }

    string description = InputHelper::getInputOptionalString("Category's description: ");

    ChannelCategory newCategory(repo.generateNextID(), name, description);
    if (repo.create(newCategory))
    {
        repo.writeToFile();
        LOG("[ChannelCategoryMenu] Category created successfully, ID: " + newCategory.getID());
        cout << "\n[Success] Channel category created successfully! ID: " << newCategory.getID() << "\n";
    }
    else
    {
        LOG("[ChannelCategoryMenu] Category creation failed");
        cout << "\n[Error] Failed to create channel category!\n";
    }

    InputHelper::getInputOptionalString("Press Enter to go back...");
}

void ChannelCategoryMenu::readChannelCategory()
{
    while (true)
    {
        SystemHelper::clearScreen();
        cout << "===== View Channel Categories =====\n";
        cout << "1. View all categories\n";
        cout << "2. Search category by ID\n";
        cout << "0. Go back\n";

        int choice = InputHelper::getInputInt("Select an option: ");
        if (choice == 0) break;

        if (choice == 1)
        {
            SystemHelper::clearScreen();
            cout << "===== All Channel Categories =====\n";
            printCategoryTable(repo.exportToVector());
            cout << "Total: " << repo.readAll().size() << " category(ies)\n";
            LOG("[ChannelCategoryMenu] Category table printed successfully");
            InputHelper::getInputOptionalString("Press Enter to go back...");
        }
        else if (choice == 2)
        {
            string inputID = convertToUpper(InputHelper::getInputString("Enter Category ID: "));
            ChannelCategory *category = repo.readByID(inputID);
            if (category == nullptr)
            {
                cout << "[Error] Category not found with ID: " << inputID << "\n";
            }
            else
            {
                vector<int> widths = {6, 20, 45};
                vector<string> headers = {"ID", "Category Name", "Description"};
                TablePrinter::printSingle(widths, headers, category->exportToVector());
            }
            LOG("[ChannelCategoryMenu] Category search completed");
            InputHelper::getInputOptionalString("Press Enter to go back...");
        }
        else
        {
            cout << "[Error] Invalid option! Please choose again.\n";
            InputHelper::getInputOptionalString("Press Enter to go back...");
        }
    }
}

// MODULAR UPDATE SUB-FUNCTIONS =====================================================================
void ChannelCategoryMenu::updateName(ChannelCategory *category)
{
    cout << "\n--- Update Category Name ---\n";
    cout << "Current Name: " << category->getName() << "\n";

    while (true)
    {
        string newName = InputHelper::getInputOptionalString("Enter new name (or leave empty to cancel): ");
        if (newName.empty())
        {
            cout << "[Notice] Name update cancelled.\n";
            return;
        }

        // Neu nhap trung voi chinh ten hien tai
        if (convertToUpper(newName) == convertToUpper(category->getName()))
        {
            cout << "[Notice] Name is unchanged.\n";
            return;
        }

        // Kiem tra trung voi cac the loai khac
        if (isNameExisted(newName, category->getID()))
        {
            cout << "[Error] Category name already exists: \"" << newName << "\". Please try another name.\n";
            continue;
        }

        category->setName(newName);
        cout << "[Success] Category name updated successfully to: \"" << newName << "\"!\n";
        break;
    }
}

void ChannelCategoryMenu::updateDescription(ChannelCategory *category)
{
    cout << "\n--- Update Category Description ---\n";
    cout << "Current Description: " << category->getDescription() << "\n";

    string newDesc = InputHelper::getInputOptionalString("Enter new description (or leave empty to keep current): ");
    if (!newDesc.empty())
    {
        category->setDescription(newDesc);
        cout << "[Success] Description updated successfully!\n";
    }
    else
    {
        cout << "[Notice] Description unchanged.\n";
    }
}

void ChannelCategoryMenu::updateChannelCategory()
{
    SystemHelper::clearScreen();
    cout << "===== Update Channel Category =====\n";
    printCategoryTable(repo.exportToVector());

    string inputID = convertToUpper(InputHelper::getInputString("Enter Category ID to edit: "));
    ChannelCategory *category = repo.readByID(inputID);

    if (category == nullptr)
    {
        cout << "[Error] Category not found with ID: " << inputID << "\n";
        InputHelper::getInputOptionalString("Press Enter to go back...");
        return;
    }

    vector<int> widths = {6, 20, 45};
    vector<string> headers = {"ID", "Category Name", "Description"};

    while (true)
    {
        SystemHelper::clearScreen();
        cout << "===== Current Information for Category: " << category->getID() << " =====\n";
        TablePrinter::printSingle(widths, headers, category->exportToVector());

        cout << "\nWhat would you like to update?\n";
        cout << "1. Update Name\n";
        cout << "2. Update Description\n";
        cout << "0. Save & Exit\n";

        int choice = InputHelper::getInputInt("Select an option (0-2): ");
        if (choice == 0)
        {
            repo.writeToFile();
            cout << "\n[Success] Category changes saved successfully to file!\n";
            LOG("[ChannelCategoryMenu] Category " + category->getID() + " updated and saved");
            InputHelper::getInputOptionalString("Press Enter to go back...");
            break;
        }

        switch (choice)
        {
        case 1:
            updateName(category);
            break;
        case 2:
            updateDescription(category);
            break;
        default:
            cout << "[Error] Invalid option! Please select 0, 1, or 2.\n";
            break;
        }

        InputHelper::getInputOptionalString("Press Enter to continue...");
    }
}

void ChannelCategoryMenu::deleteChannelCategory()
{
    SystemHelper::clearScreen();
    cout << "===== Delete Channel Category =====\n";
    printCategoryTable(repo.exportToVector());

    string targetID = convertToUpper(InputHelper::getInputString("Enter Category ID to delete: "));
    ChannelCategory *category = repo.readByID(targetID);

    if (category == nullptr)
    {
        cout << "[Error] Category not found with ID: " << targetID << "\n";
        InputHelper::getInputOptionalString("Press Enter to go back...");
        return;
    }

    if (hasChannelsInCategory(targetID))
    {
        cout << "\n[Error] Cannot delete category " << targetID << " (\"" << category->getName() << "\")!\n";
        cout << "Reason: There are still channels associated with this category.\n";
        cout << "Please reassign or delete those channels before deleting this category.\n";
        InputHelper::getInputOptionalString("Press Enter to go back...");
        return;
    }

    vector<int> widths = {6, 20, 45};
    vector<string> headers = {"ID", "Category Name", "Description"};
    cout << "\nYou are about to delete this category:\n";
    TablePrinter::printSingle(widths, headers, category->exportToVector());

    string confirm = InputHelper::getInputString("Are you sure you want to delete this category? (y/n): ");
    if (confirm == "y" || confirm == "Y")
    {
        if (repo.remove(targetID))
        {
            repo.writeToFile();
            cout << "[Success] Category deleted successfully, ID: " << targetID << "\n";
            LOG("[ChannelCategoryMenu] Category deleted successfully, ID: " + targetID);
        }
        else
        {
            cout << "[Error] Failed to delete category from database!\n";
        }
    }
    else
    {
        cout << "[Abort] Deletion cancelled by user.\n";
        LOG("[ChannelCategoryMenu] Delete operation cancelled by user");
    }

    InputHelper::getInputOptionalString("Press Enter to go back...");
}

void ChannelCategoryMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "=========================================\n";
    cout << "        CHANNEL CATEGORY MANAGEMENT\n";
    cout << "=========================================\n";
    cout << "1. Create a Category\n";
    cout << "2. Read / Search Categories\n";
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
        cout << "[Error] Invalid option in Channel Category menu!\n";
        InputHelper::getInputOptionalString("Press Enter to go back...");
        break;
    }
}