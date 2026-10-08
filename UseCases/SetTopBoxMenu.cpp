#include "UseCases/SetTopBoxMenu.h"
#include "Utils/Debugger.h"
#include "Utils/TablePrinter.h"
#include "Utils/SystemHelper.h"
#include "Utils/InputHelper.h"
#include <vector>
#include <iostream>
#include <ctime>
#include "Utils/StringHelper.h"

using namespace std;

SetTopBoxMenu::SetTopBoxMenu() : repo("Data/settopboxes.txt", "Data/settopboxes_counter.txt", "STB")
{
    repo.loadFromFile();
}

void SetTopBoxMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "===== Set-top Box Management =====" << endl;
    cout << "1. Add Set-top Box" << endl;
    cout << "2. View Set-top Boxes" << endl;
    cout << "3. Update Set-top Box" << endl;
    cout << "4. Delete Set-top Box" << endl;
}

void SetTopBoxMenu::handleChoice(int choice)
{
    switch (choice)
    {
    case 1:
        createSetTopBox();
        break;
    case 2:
        readSetTopBox();
        break;
    case 3:
        updateSetTopBox();
        break;
    case 4:
        deleteSetTopBox();
        break;
    default:
        cout << "Invalid choice! Press Enter to try again." << endl;
        InputHelper::getInputOptionalString("");
        break;
    }
}

TechnicalStatus SetTopBoxMenu::inputTechnicalStatus(bool isOptional, TechnicalStatus defaultStatus)
{
    while (true)
    {
        cout << "\nSelect technical status:" << endl;
        cout << "1. TOT (Tot)" << endl;
        cout << "2. HONG (Hong)" << endl;
        cout << "3. BAO_TRI (Bao tri)" << endl;
        cout << "4. NGUNG_SU_DUNG (Ngung su dung)" << endl;

        string prompt = isOptional ? "Enter choice (1-4) or leave empty to keep current: " : "Enter choice (1-4): ";
        string choiceStr = isOptional ? InputHelper::getInputOptionalString(prompt) : InputHelper::getInputString(prompt);

        if (isOptional && choiceStr.empty())
            return defaultStatus;

        if (choiceStr == "1")
            return TOT;
        if (choiceStr == "2")
            return HONG;
        if (choiceStr == "3")
            return BAO_TRI;
        if (choiceStr == "4")
            return NGUNG_SU_DUNG;

        cout << "Invalid choice! Please try again." << endl;
    }
}

void SetTopBoxMenu::createSetTopBox()
{
    SystemHelper::clearScreen();
    cout << "Enter Set-top Box information:" << endl;

    string serial;
    while (true)
    {
        serial = InputHelper::getInputString("Enter serial (must be unique)");
        bool exists = false;
        for (const auto &stb : repo.readAll())
        {
            if (stb.getSerial() == serial)
            {
                exists = true;
                break;
            }
        }
        if (exists)
        {
            cout << "[Error] Serial already exists! Please try again." << endl;
        }
        else
        {
            break;
        }
    }

    string manufacturer = InputHelper::getInputString("Enter manufacturer");
    string model = InputHelper::getInputString("Model");

    Date importDate;
    time_t now = time(0);
    tm *ltm = localtime(&now);
    Date currentDate(ltm->tm_mday, 1 + ltm->tm_mon, 1900 + ltm->tm_year);

    while (true)
    {
        importDate = InputHelper::getInputDate("Enter import Date");
        if (importDate > currentDate)
        {
            cout << "Import date cannot be in the future! Please try again." << endl;
        }
        else
        {
            break;
        }
    }

    TechnicalStatus status = inputTechnicalStatus(false, TOT);

    SetTopBox temp(repo.generateNextID(), serial, manufacturer, model, importDate, status);
    if (repo.create(temp))
    {
        repo.writeToFile();
        LOG("[SetTopBoxMenu] SetTopBox created successfully");
        cout << "Set-top Box created successfully!" << endl;
    }
    else
    {
        cout << "Set-top Box creation failed, abort!" << endl;
    }
    InputHelper::getInputOptionalString("Go back?");
}

void SetTopBoxMenu::readSetTopBox()
{
    SystemHelper::clearScreen();
    vector<int> widths = {8, 15, 15, 15, 12, 15};
    vector<string> headers = {"ID", "Serial", "Manufacturer", "Model", "Import Date", "Status"};

    cout << "===== View Set-top Boxes =====" << endl;
    cout << "1. View all Set-top Boxes" << endl;
    cout << "2. Search by ID" << endl;
    int choice = InputHelper::getInputInt("Select an option (enter neither to go back): ");
    SystemHelper::clearScreen();

    if (choice == 1)
    {
        cout<<"=====All Set top box=====\n";
        TablePrinter::printTable(widths, headers, repo.exportToVector());
        LOG("[SetTopBoxMenu] SetTopBox table printed successfully");
    }
    else if (choice == 2)
    {
        cout<<"=====Find Set top box=====\n";
        string id = convertToUpper(InputHelper::getInputString("Enter Set-top Box ID to search: "));
        SetTopBox *result = repo.readByID(id);
        if (result != nullptr)
        {
            SystemHelper::clearScreen();
            cout<<"=====Find Set top box=====\n";
            TablePrinter::printSingle(widths, headers, result->exportToVector());
        }
        else
        {
            cout << "[Error] Set-top Box not found!" << endl;
        }
    }
    InputHelper::getInputOptionalString("Go back?");
}

void SetTopBoxMenu::updateSetTopBox()
{
    SystemHelper::clearScreen();
    cout << "===== Update Set-top Box =====" << endl;
    vector<int> widths = {8, 15, 15, 15, 12, 15};
    vector<string> headers = {"ID", "Serial", "Manufacturer", "Model", "Import Date", "Status"};
    TablePrinter::printTable(widths, headers, repo.exportToVector());

    string id = convertToUpper(InputHelper::getInputString("Enter Set-top Box ID to update: "));

    SetTopBox *stb = repo.readByID(id);
    if (stb == nullptr)
    {
        cout << "[Error] Set-top Box not found!" << endl;
        InputHelper::getInputOptionalString("Go back?");
        return;
    }

    // Print current info
    SystemHelper::clearScreen();
    cout << "===== Set-top Box Current Information =====" << endl;
    TablePrinter::printSingle(widths, headers, stb->exportToVector());

    cout << "\nLeave field empty if you don't want to update it." << endl;

    string newSerial = InputHelper::getInputOptionalString("New Serial [" + stb->getSerial() + "]: ");
    if (!newSerial.empty() && newSerial != stb->getSerial())
    {
        bool exists = false;
        for (const auto &item : repo.readAll())
        {
            if (item.getSerial() == newSerial)
            {
                exists = true;
                break;
            }
        }
        if (exists)
        {
            cout << "[Warning] Serial already exists! Keeping old serial." << endl;
        }
        else
        {
            stb->setSerial(newSerial);
        }
    }

    string newManufacturer = InputHelper::getInputOptionalString("New Manufacturer [" + stb->getManufacturer() + "]: ");
    if (!newManufacturer.empty())
        stb->setManufacturer(newManufacturer);

    string newModel = InputHelper::getInputOptionalString("New Model [" + stb->getModel() + "]: ");
    if (!newModel.empty())
        stb->setModel(newModel);

    // Date update
    time_t now = time(0);
    tm *ltm = localtime(&now);
    Date currentDate(ltm->tm_mday, 1 + ltm->tm_mon, 1900 + ltm->tm_year);

    while (true)
    {
        Date newDate = InputHelper::getInputOptionalDate("New Import Date [" + stb->getImportDate().toString() + "]: ");
        if (newDate.isEmpty())
        {
            break;
        }
        if (newDate > currentDate)
        {
            cout << "Import date cannot be in the future! Please try again." << endl;
        }
        else
        {
            stb->setImportDate(newDate);
            break;
        }
    }

    cout << "Current status: " << stb->getTechnicalStatusString() << endl;
    TechnicalStatus newStatus = inputTechnicalStatus(true, stb->getTechnicalStatus());
    stb->setTechnicalStatus(newStatus);

    if (repo.update(*stb))
    {
        repo.writeToFile();
        LOG("[SetTopBoxMenu] SetTopBox updated successfully");
        cout << "Set-top Box updated successfully!" << endl;
    }
    else
    {
        cout << "Update failed!" << endl;
    }
    InputHelper::getInputOptionalString("Go back?");
}

void SetTopBoxMenu::deleteSetTopBox()
{
    SystemHelper::clearScreen();
    cout << "===== Delete Set-top Box =====" << endl;
    vector<int> widths = {8, 15, 15, 15, 12, 15};
    vector<string> headers = {"ID", "Serial", "Manufacturer", "Model", "Import Date", "Status"};
    TablePrinter::printTable(widths, headers, repo.exportToVector());

    string id = convertToUpper(InputHelper::getInputString("Enter Set-top Box ID to delete: "));

    SetTopBox *stb = repo.readByID(id);
    if (stb == nullptr)
    {
        cout << "[Error] Set-top Box not found!" << endl;
        InputHelper::getInputOptionalString("Go back?");
        return;
    }

    SystemHelper::clearScreen();
    cout << "You are about to remove Set-top Box:" << endl;
    TablePrinter::printSingle(widths, headers, stb->exportToVector());


    //Check remove conditions here
    //============================
    string confirm = InputHelper::getInputString("Are you sure you want to delete this Set-top Box? (y/n): ");
    if (confirm == "y" || confirm == "Y")
    {
        if (repo.remove(id))
        {
            repo.writeToFile();
            LOG("[SetTopBoxMenu] SetTopBox removed successfully");
            cout << "Set-top Box deleted successfully!" << endl;
        }
        else
        {
            cout << "[Error] Failed to delete Set-top Box!" << endl;
        }
    }
    else
    {
        cout << "Deletion cancelled." << endl;
    }
    InputHelper::getInputOptionalString("Go back?");
}
