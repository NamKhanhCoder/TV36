#include "ContractMenu.h"
#include "Entities/Contract.h"
#include "Utils/SystemHelper.h"
#include "Utils/StringHelper.h"
#include "Utils/TablePrinter.h"
#include "Utils/InputHelper.h"

using namespace std;

ContractMenu::ContractMenu() : repo("Data/contracts.txt", "Data/contracts_counter.txt", "HD")
{
    repo.loadFromFile();    
}

ContractStatus ContractMenu::inputStatus()
{
    while(1)
    {
        cout << "Enter the contract status: " << endl;
        cout << "1. ACTIVE" << endl << "2. SUSPENDED" << endl << "3. EXPIRED" << endl << "4. CANCELED" << endl;
        int s = InputHelper::getInputInt("Contract's status: ");
        if(s == 1) return ContractStatus::ACTIVE;
        if(s == 2) return ContractStatus::SUSPENDED;
        if(s == 3) return ContractStatus::EXPIRED;
        if(s == 4) return ContractStatus::CANCELED;
        else cout << "## INPUT ERROR ##" << endl << "Try again" << endl;
    }
}

ContractStatus ContractMenu::inputOptionalStatus(ContractStatus currentStatus)
{
    while(1)
    {
        cout << "Enter the contract status: " << endl;
        cout << "1. ACTIVE" << endl << "2. SUSPENDED" << endl << "3. EXPIRED" << endl << "4. CANCELED" << endl << "0. Skip" << endl;
        int s = InputHelper::getInputInt("Contract's status: ");
        if(s == 1) return ContractStatus::ACTIVE;
        if(s == 2) return ContractStatus::SUSPENDED;
        if(s == 3) return ContractStatus::EXPIRED;
        if(s == 4) return ContractStatus::CANCELED;
        if(s == 0) return currentStatus;
        else cout << "## INPUT ERROR ##" << endl << "Try again" << endl;
    }
}

void ContractMenu::createContract()
{
    SystemHelper::clearScreen();
    cout << "Enter Contract's Informations: " << endl;
    string customerID = InputHelper::getInputString("Customer's ID:");
    string packageID = InputHelper::getInputString("Package's ID:");
    string stbID = InputHelper::getInputString("Set-Top Box's ID:");
    Date signDate = InputHelper::getInputDate("Sign Date:");
    Date startDate = InputHelper::getInputDate("Start Date:");
    Date endDate = InputHelper::getInputDate("End Date:");
    string installationAddress = InputHelper::getInputString("Installation Address:");
    string monthlyFee = InputHelper::getInputString("Monthly Fee: ");
    ContractStatus status = ContractMenu::inputStatus();

    Contract temp(repo.generateNextID(), customerID, packageID, stbID, signDate, startDate, endDate, installationAddress, monthlyFee, status);
    if(repo.create(temp))
    {
        repo.writeToFile();
        LOG("[ContractMenu] Contract created successfully");
        cout << "Contract created successfulley\n";
    }
    else
    {
        LOG("[ContractMenu] Contract created unsuccessfully");
        cout << "Contract creation failed, abort\n";
    }
}

void ContractMenu::readContract()
{
    SystemHelper::clearScreen();
    cout << "=====View contracts=====" << endl;
    cout << "1. View all contracts" << endl;
    cout << "2. Search by ID" << endl;

    vector<int> widths = {5, 12, 12, 12, 12, 12, 12, 20, 15, 15};
    vector<string> headers = {"ID", "Customer's ID", "Package's ID", "Set-Top Box's ID", "Sign Date", "Start Date", "End Date", "Installation Address", "Monthly Fee", "Status"};

    int choice = InputHelper::getInputInt("Select an option (enter 0 to go back): ");
    if(choice == 1)
    {
        TablePrinter::printTable(widths, headers, repo.exportToVector());
        LOG("[ContractMenu] Contract table printed successfully");
    }
    else if(choice == 2)
    {
        string inputID = convertToUpper(InputHelper::getInputString("Enter Contract ID: "));
        Contract *Contract = repo.readByID(inputID);
        if (Contract == nullptr)
        {
            cout << "[Error] Contract not found, ID: " << inputID << endl;
        }
        else
        {
            TablePrinter::printSingle(widths, headers, Contract->exportToVector());
        }
        LOG("[ContractMenu] Contract search completed");
    }
    InputHelper::getInputOptionalString("Press Enter to go back");
}

void ContractMenu::updateContract()
{
    SystemHelper::clearScreen();
    vector<int> widths = {5, 12, 12, 12, 12, 12, 12, 20, 15, 15};
    vector<string> headers = {"ID", "Customer's ID", "Package's ID", "Set-Top Box's ID", "Sign Date", "Start Date", "End Date", "Installation Address", "Monthly Fee", "Status"}; 
    TablePrinter::printTable(widths,headers,repo.exportToVector());

    string inputID = convertToUpper(InputHelper::getInputString("Enter Contract ID to edit:"));
    Contract *Contract = repo.readByID(inputID);

    if (Contract == nullptr)
    {
        cout << "[Error] Contract not found, ID: " << inputID << endl;
        return;
    }
    SystemHelper::clearScreen();
    cout << "=====Contract current information=====" << endl;
    TablePrinter::printSingle(widths, headers, Contract->exportToVector());

    cout << "Enter Contract new information (skip to keep it as it is)\n";

    string customerID = InputHelper::getInputOptionalString("Customer's ID:");
    string packageID = InputHelper::getInputOptionalString("Package's ID:");
    string stbID = InputHelper::getInputOptionalString("Set-Top Box's ID:");
    Date signDate = InputHelper::getInputOptionalDate("Sign date:");
    Date startDate = InputHelper::getInputOptionalDate("Start date:");
    Date endDate = InputHelper::getInputOptionalDate("End date:");
    string installationAddress = InputHelper::getInputOptionalString("Installation Address:");
    string monthlyFee = InputHelper::getInputOptionalString("Monthly Fee:");
    ContractStatus status = ContractMenu::inputOptionalStatus(Contract->get_status());

    // Update
    if (!packageID.empty()) Contract->set_packageID(packageID);
    if (!customerID.empty()) Contract->set_customerID(customerID);
    if (!stbID.empty()) Contract->set_stbID(stbID);
    if (!signDate.isEmpty()) Contract->set_signDate(signDate);
    if (!startDate.isEmpty()) Contract->set_startDate(startDate);
    if (!endDate.isEmpty()) Contract->set_endDate(endDate);
    if (!installationAddress.empty()) Contract->set_installationAddress(installationAddress);
    if (!monthlyFee.empty()) Contract->set_monthlyFee(monthlyFee);
    Contract->set_status(status);

    repo.writeToFile();
    LOG("[ContractMenu] Contract information updated successfully");
}

void ContractMenu::deleteContract()
{
    cout << "\n=====Delete contract=====\n";
    vector<int> widths = {5, 12, 12, 12, 12, 12, 12, 20, 15, 15};
    vector<string> headers = {"ID", "Customer's ID", "Package's ID", "Set-Top Box's ID", "Sign Date", "Start Date", "End Date", "Installation Address", "Monthly Fee", "Status"};
    TablePrinter::printTable(widths, headers, repo.exportToVector());

    string targetID = convertToUpper(InputHelper::getInputString("Enter Contract ID to delete: "));

    Contract* Contract = repo.readByID(targetID);
    if (Contract == nullptr)
    {
        cout << "[Error] Contract not found: " << targetID << endl;
        return;
    }

    cout << "You are about to remove Contract:" << endl;
    TablePrinter::printSingle(widths, headers, Contract->exportToVector());

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
            cout << "[ContractMenu] Contract deleted successfully, ID: " << targetID << endl;
            LOG("[ContractMenu] Contract deleted successfully");
        }
        else
        {
            cout << "[Error] Contract deletion failed, please check debug!" << endl;
        }
    }
    else
    {
        cout << "[Abort] Deletion aborted" << endl;
        LOG("[ContractMenu] Delete operation cancelled by user");
    }
}

void ContractMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "=====Contract Menu=====\n";
    cout << "1. Create a Contract\n";
    cout << "2. Read Contract\n";
    cout << "3. Update a Contract\n";
    cout << "4. Delete a Contract\n";    
}

void ContractMenu::handleChoice(int choice)
{
    switch (choice)
    {
    case 1:
        createContract();
        break;
    case 2:
        readContract();
        break;
    case 3:
        updateContract();
        break;
    case 4:
        deleteContract();
        break;
    default:
        cout << "[Error] Invalid option in Contract menu\n";
    }
}
