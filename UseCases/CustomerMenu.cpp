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
    cout << "Enter Customer information:" << endl;
    string name = InputHelper::getInputString("Enter customer name:");
    string phone = InputHelper::getInputPhone("Enter phone:");
    string email = InputHelper::getInputEmail("Enter email:");
    string address = InputHelper::getInputString("Enter address:");

    Customer temp(repo.generateNextID(), name, phone, email, address);
    if (repo.create(temp))
    {
        repo.writeToFile();
        LOG("[CustomerMenu] Customer created successfully");
        cout << "Customer created successfully!" << endl;
    }
    else
    {
        cout << "Customer creation failed, abort!" << endl;
    }
    InputHelper::getInputOptionalString("Go back?");
}

void CustomerMenu::readCustomer()
{
    SystemHelper::clearScreen();
    vector<int> widths = {8, 22, 12, 25, 20};
    vector<string> headers = {"ID", "Name", "Phone", "Email", "Address"};
    
    cout << "===== View Customers =====" << endl;
    cout << "1. View all Customers" << endl;
    cout << "2. Search by ID" << endl;
    int choice = InputHelper::getInputInt("Select an option (enter neither to go back): ");
    SystemHelper::clearScreen();

    if (choice == 1)
    {
        cout << "===== All Customers =====\n";
        TablePrinter::printTable(widths, headers, repo.exportToVector());
        LOG("[CustomerMenu] Customer table printed successfully");
    }
    else if (choice == 2)
    {
        cout << "===== Find Customer =====\n";
        string inputID = convertToUpper(InputHelper::getInputString("Enter Customer ID to search: "));
        Customer *cus = repo.readByID(inputID);
        if (cus != nullptr)
        {
            SystemHelper::clearScreen();
            cout << "===== Find Customer =====\n";
            TablePrinter::printSingle(widths, headers, cus->exportToVector());
        }
        else
        {
            cout << "[Error] Customer not found!" << endl;
        }
        LOG("[CustomerMenu] Customer search completed");
    }
    InputHelper::getInputOptionalString("Go back?");
}

void CustomerMenu::updateCustomer()
{
    SystemHelper::clearScreen();
    cout << "===== Update Customer =====" << endl;
    vector<int> widths = {8, 22, 12, 25, 20};
    vector<string> headers = {"ID", "Name", "Phone", "Email", "Address"};
    TablePrinter::printTable(widths, headers, repo.exportToVector());

    string inputID = convertToUpper(InputHelper::getInputString("Enter Customer ID to update: "));
    Customer *cus = repo.readByID(inputID);

    if (cus == nullptr)
    {
        cout << "[Error] Customer not found!" << endl;
        InputHelper::getInputOptionalString("Go back?");
        return;
    }

    SystemHelper::clearScreen();
    cout << "===== Customer Current Information =====" << endl;
    TablePrinter::printSingle(widths, headers, cus->exportToVector());

    cout << "\nLeave field empty if you don't want to update it." << endl;

    string name = InputHelper::getInputOptionalString("New Name [" + cus->getName() + "]: ");
    if (!name.empty())
        cus->setName(name);

    string phone = InputHelper::getInputOptionalPhone("New Phone [" + cus->getPhone() + "]: ");
    if (!phone.empty())
        cus->setPhone(phone);

    string email = InputHelper::getInputOptionalEmail("New Email [" + cus->getEmail() + "]: ");
    if (!email.empty())
        cus->setEmail(email);

    string address = InputHelper::getInputOptionalString("New Address [" + cus->getAddress() + "]: ");
    if (!address.empty())
        cus->setAddress(address);

    if (repo.update(*cus))
    {
        repo.writeToFile();
        LOG("[CustomerMenu] Customer information updated successfully");
        cout << "Customer updated successfully!" << endl;
    }
    else
    {
        cout << "Update failed!" << endl;
    }
    InputHelper::getInputOptionalString("Go back?");
}

void CustomerMenu::deleteCustomer()
{
    SystemHelper::clearScreen();
    cout << "===== Delete Customer =====" << endl;
    vector<int> widths = {8, 22, 12, 25, 20};
    vector<string> headers = {"ID", "Name", "Phone", "Email", "Address"};
    TablePrinter::printTable(widths, headers, repo.exportToVector());
    
    string targetID = convertToUpper(InputHelper::getInputString("Enter Customer ID to delete: "));

    // 2. Tìm kiếm trong Repo
    Customer* cus = repo.readByID(targetID);
    if (cus == nullptr)
    {
        cout << "[Error] Customer not found!" << endl;
        InputHelper::getInputOptionalString("Go back?");
        return;
    }

    SystemHelper::clearScreen();
    cout << "You are about to remove Customer:" << endl;
    TablePrinter::printSingle(widths, headers, cus->exportToVector());

    // =====================================================================
    // [PLACEHOLDER] Delete conditions
    // =====================================================================
    // Delete customer phụ thuộc vào việc khách hàng có đang kí hợp đồng nào còn hạn không (sẽ cập nhật sau)

    // =====================================================================

    string confirm = InputHelper::getInputString("Are you sure you want to delete this Customer? (y/n): ");
    
    if (confirm == "y" || confirm == "Y")
    {
        if (repo.remove(targetID))
        {
            repo.writeToFile();
            LOG("[CustomerMenu] Customer deleted successfully");
            cout << "Customer deleted successfully!" << endl;
        }
        else
        {
            cout << "[Error] Failed to delete Customer!" << endl;
        }
    }
    else
    {
        cout << "Deletion cancelled." << endl;
        LOG("[CustomerMenu] Delete operation cancelled by user");
    }
    InputHelper::getInputOptionalString("Go back?");
}

void CustomerMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "===== Customer Management =====" << endl;
    cout << "1. Add Customer" << endl;
    cout << "2. View Customers" << endl;
    cout << "3. Update Customer" << endl;
    cout << "4. Delete Customer" << endl;
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
        cout << "Invalid choice! Press Enter to try again." << endl;
        InputHelper::getInputOptionalString("");
        break;
    }
}
