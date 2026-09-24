#include "UseCases/CustomerMenu.h"
#include "Utils/Debugger.h"
CustomerMenu::CustomerMenu() : repo("Data/customers.txt") // initializer list
{
    repo.loadFromFile();
}

void CustomerMenu::createCustomer()
{
    LOG("Create customer called");
}
void CustomerMenu::readCustomer()
{
    LOG("Read customer called");
}
void CustomerMenu::updateCustomer()
{
    LOG("Update customer called");
}
void CustomerMenu::deleteCustomer()
{
    LOG("Delete customer called");
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
        cout<<"[Error] Invalid option in Customer menu";
    }
    //
}
