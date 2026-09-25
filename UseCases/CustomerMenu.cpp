#include "UseCases/CustomerMenu.h"
#include "Utils/Debugger.h"
#include "Utils/TablePrinter.h"
#include <vector>
#include "Utils/SystemHelper.h"
#include "Utils/StringHelper.h"
using namespace std;
CustomerMenu::CustomerMenu() : repo("Data/customers.txt", "Data/customers_counter.txt", "KH") // initializer list
{
    repo.loadFromFile();
}

void CustomerMenu::createCustomer()
{
    SystemHelper::clearScreen();
    cout << "Enter customer information:" << endl;
    string name = InputHelper::getInputString("Customer name:");
    string phone = InputHelper::getInputPhone("Phone:");
    string email = InputHelper::getInputEmail("Email:");
    string address = InputHelper::getInputString("Address:");

    Customer temp(repo.generateNextID(), name, phone, email, address);
    if (repo.create(temp))
    {
        repo.writeToFile();
        LOG("[CustomerMenu] Customer created successfully");
        cout << "Customer created successfulley\n";
    }
    else
    {
        cout << "Customer creation failed, abort\n";
    }
}
void CustomerMenu::readCustomer()
{
    SystemHelper::clearScreen();
    vector<int> widths = {5, 10, 10, 10, 10};
    vector<string> headers = {"ID", "Name", "Phone", "Email", "Address"};

    cout << "=====View customers=====" << endl;
    cout << "1. View all customers" << endl;
    cout << "2. Search by ID" << endl;
    int choice = InputHelper::getInputInt("Select an option (enter 0 to go back): ");

    if (choice == 1)
    {
        TablePrinter::printTable(widths, headers, repo.exportToVector());
        LOG("[CustomerMenu] Customer table printed successfully");
    }
    else if (choice == 2)
    {
        string inputID = convertToUpper(InputHelper::getInputString("Enter customer ID: "));
        Customer *cus = repo.readByID(inputID);
        if (cus == nullptr)
        {
            cout << "[Error] Customer not found, ID: " << inputID << endl;
        }
        else
        {
            TablePrinter::printSingle(widths, headers, cus->exportToVector());
        }
        LOG("[CustomerMenu] Customer search completed");
    }
}
void CustomerMenu::updateCustomer()
{
    SystemHelper::clearScreen();
    vector<int> widths = {5, 10, 10, 10, 10};
    vector<string> headers = {"ID", "Name", "Phone", "Email", "Address"};
    TablePrinter::printTable(widths,headers,repo.exportToVector());

    string inputID = convertToUpper(InputHelper::getInputString("Enter customer ID to edit:"));
    Customer *cus = repo.readByID(inputID);

    if (cus == nullptr)
    {
        cout << "[Error] Customer not found, ID: " << inputID << endl;
        return;
    }
    SystemHelper::clearScreen();
    cout << "=====Customer current information=====" << endl;
    TablePrinter::printSingle(widths, headers, cus->exportToVector());

    cout << "Enter customer new information (skip to keep it as it is)\n";

    string name = InputHelper::getInputOptionalString("Customer name:");
    string phone = InputHelper::getInputOptionalPhone("Phone:");
    string email = InputHelper::getInputOptionalEmail("Email:");
    string address = InputHelper::getInputOptionalString("Address:");

    if(name !="")
    {
        cus->setName(name);
    }
    if(phone !="")
    {
        cus->setPhone(phone);
    }
    if(email!="")
    {
        cus->setEmail(email);
    }
    if(address!="")
    {
        cus->setAddress(address);
    }
    repo.writeToFile();
    LOG("[CustomerMenu] Customer information updated successfully");

}
void CustomerMenu::deleteCustomer()
{
    cout << "\n=====Delete customer=====\n";
    
    string targetID = convertToUpper(InputHelper::getInputString("Enter customer ID to delete: "));

    // 2. Tìm kiếm trong Repo
    Customer* cus = repo.readByID(targetID);
    if (cus == nullptr)
    {
        cout << "[Error] Customer not found: " << targetID << endl;
        return;
    }

    cout << "You are about to remove customer:" << endl;
    vector<int> widths = {5, 10, 10, 10, 10};
    vector<string> headers = {"ID", "Name", "Phone", "Email", "Address"};
    TablePrinter::printSingle(widths, headers, cus->exportToVector());

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
            cout << "[CustomerMenu] Customer deleted successfully, ID: " << targetID << endl;
            LOG("[CustomerMenu] Customer deleted successfully");
        }
        else
        {
            cout << "[Error] Customer deletion failed, please check debug!" << endl;
        }
    }
    else
    {
        cout << "[Abort] Deletion aborted" << endl;
        LOG("[CustomerMenu] Delete operation cancelled by user");
    }
}

void CustomerMenu::printMenu()
{
    
    cout << "=====Customer menu=====\n";
    cout << "1. Create a customer\n";
    cout << "2. Read customers\n";
    cout << "3. Update a customer\n";
    cout << "4. Delete a customer\n";
    // afterprint, base menu will automatically call handle choice
}
void CustomerMenu::handleChoice(int choice)
{
    
    switch (choice)
    {
    case 1:
        createCustomer();
        break;
    case 2:
        readCustomer();
        break;
    case 3:
        updateCustomer();
        break;
    case 4:
        deleteCustomer();
        break;
    default:
        cout << "[Error] Invalid option in Customer menu\n";
    }
}
