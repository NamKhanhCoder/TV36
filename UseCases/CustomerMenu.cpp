// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193)
// Use Case 1: Quan ly Khach hang (Customer Menu)
// ===============================================================
#include "UseCases/CustomerMenu.h"
#include "Utils/Debugger.h"
#include "Utils/TablePrinter.h"
#include "Utils/SystemHelper.h"
#include "Utils/StringHelper.h"
#include "Utils/InputHelper.h"
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

namespace
{
    const vector<int> tableWidths = {8, 22, 12, 25, 22};
    const vector<string> tableHeaders = {"ID", "Name", "Phone", "Email", "Address"};
}

CustomerMenu::CustomerMenu()
    : repoCustomer("Data/customers.txt", "Data/customers_counter.txt", "KH"),
      repoContract("Data/contracts.txt", "Data/contracts_counter.txt", "HD"),
      repoHoaDon("Data/hoadon.txt", "Data/hdn_counter.txt", "HDN")
{
    repoCustomer.loadFromFile();
    repoContract.loadFromFile();
    repoHoaDon.loadFromFile();
}

void CustomerMenu::createCustomer()
{
    SystemHelper::clearScreen();
    cout << "=========================================\n";
    cout << "           ADD NEW CUSTOMER\n";
    cout << "=========================================\n";

    Customer temp;

    // 1. Full Name
    while (true)
    {
        string name = InputHelper::getInputString("Enter customer full name: ");
        try
        {
            temp.setName(name);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[CustomerMenu] Invalid Name input during creation: " + string(e.what()));
        }
    }

    // 2. Phone Number
    while (true)
    {
        string phone = InputHelper::getInputPhone("Enter phone number (10 digits, starts with 0): ");
        try
        {
            temp.setPhone(phone);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[CustomerMenu] Invalid Phone input during creation: " + string(e.what()));
        }
    }

    // 3. Email
    while (true)
    {
        string email = InputHelper::getInputEmail("Enter email address: ");
        try
        {
            temp.setEmail(email);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[CustomerMenu] Invalid Email input during creation: " + string(e.what()));
        }
    }

    // 4. Address
    while (true)
    {
        string address = InputHelper::getInputString("Enter address: ");
        try
        {
            temp.setAddress(address);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[CustomerMenu] Invalid Address input during creation: " + string(e.what()));
        }
    }

    try
    {
        string newID = repoCustomer.generateNextID();
        Customer newCustomer(newID, temp.getName(), temp.getPhone(), temp.getEmail(), temp.getAddress());

        if (repoCustomer.create(newCustomer))
        {
            try
            {
                repoCustomer.writeToFile();
                LOG("[CustomerMenu] Customer created successfully, ID: " + newID);
                cout << "\n[Success] Customer created successfully! Assigned ID: " << newID << "\n";
            }
            catch (const exception &e)
            {
                repoCustomer.loadFromFile();
                LOG("[CustomerMenu] File error while saving new customer: " + string(e.what()));
                cout << "\n[File Error] " << e.what() << "\n";
                cout << "Changes discarded. Customer was not saved to disk.\n";
            }
        }
        else
        {
            LOG("[CustomerMenu] Failed to create customer, duplicate ID: " + newID);
            cout << "\n[Error] Failed to create customer (duplicate ID)!\n";
        }
    }
    catch (const exception &e)
    {
        LOG("[CustomerMenu] Exception caught during customer creation: " + string(e.what()));
        cout << "\n[Exception] " << e.what() << "\n";
    }

    InputHelper::getInputOptionalString("\nPress Enter to return...");
}

void CustomerMenu::readCustomer()
{
    SystemHelper::clearScreen();
    cout << "=========================================\n";
    cout << "             CUSTOMER LIST\n";
    cout << "=========================================\n";

    if (repoCustomer.readAll().empty())
    {
        cout << "[Notice] Customer list is currently empty!\n";
    }
    else
    {
        TablePrinter::printTable(tableWidths, tableHeaders, repoCustomer.exportToVector());
        LOG("[CustomerMenu] Customer table printed successfully");
    }

    InputHelper::getInputOptionalString("\nPress Enter to return...");
}

void CustomerMenu::searchCustomer()
{
    while (true)
    {
        SystemHelper::clearScreen();
        cout << "=========================================\n";
        cout << "            SEARCH CUSTOMERS\n";
        cout << "=========================================\n";
        cout << "1. Exact search by Customer ID\n";
        cout << "2. Fuzzy search by Full Name\n";
        cout << "3. Search by Phone Number\n";
        cout << "0. Go back\n";

        int opt = InputHelper::getInputInt("Select an option (0-3): ");
        if (opt == 0) break;

        if (opt == 1)
        {
            string id = convertToUpper(InputHelper::getInputString("Enter Customer ID to search (e.g. KH001): "));
            LOG("[CustomerMenu] Search customer by ID: " + id);
            Customer *cus = repoCustomer.readByID(id);
            SystemHelper::clearScreen();
            cout << "===== Search Results for ID: " << id << " =====\n";
            if (cus != nullptr)
            {
                TablePrinter::printSingle(tableWidths, tableHeaders, cus->exportToVector());
            }
            else
            {
                cout << "[Notice] No customer found with ID: " << id << "\n";
            }
            InputHelper::getInputOptionalString("\nPress Enter to continue...");
        }
        else if (opt == 2)
        {
            string keyword = convertToUpper(InputHelper::getInputString("Enter name keyword: "));
            LOG("[CustomerMenu] Search customer by name: " + keyword);
            vector<vector<string>> matchedData;
            for (const Customer &c : repoCustomer.readAll())
            {
                if (convertToUpper(c.getName()).find(keyword) != string::npos)
                {
                    matchedData.push_back(c.exportToVector());
                }
            }

            SystemHelper::clearScreen();
            cout << "===== Search Results for Name: \"" << keyword << "\" =====\n";
            if (!matchedData.empty())
            {
                TablePrinter::printTable(tableWidths, tableHeaders, matchedData);
            }
            else
            {
                cout << "[Notice] No customers matched the keyword!\n";
            }
            InputHelper::getInputOptionalString("\nPress Enter to continue...");
        }
        else if (opt == 3)
        {
            string phoneQuery = InputHelper::getInputString("Enter phone number to search: ");
            LOG("[CustomerMenu] Search customer by phone: " + phoneQuery);
            vector<vector<string>> matchedData;
            for (const Customer &c : repoCustomer.readAll())
            {
                if (c.getPhone().find(phoneQuery) != string::npos)
                {
                    matchedData.push_back(c.exportToVector());
                }
            }

            SystemHelper::clearScreen();
            cout << "===== Search Results for Phone: \"" << phoneQuery << "\" =====\n";
            if (!matchedData.empty())
            {
                TablePrinter::printTable(tableWidths, tableHeaders, matchedData);
            }
            else
            {
                cout << "[Notice] No customers found with that phone number!\n";
            }
            InputHelper::getInputOptionalString("\nPress Enter to continue...");
        }
        else
        {
            cout << "[Error] Invalid option! Please try again.\n";
            InputHelper::getInputOptionalString("Press Enter to continue...");
        }
    }
}

void CustomerMenu::sortCustomer()
{
    while (true)
    {
        SystemHelper::clearScreen();
        cout << "=========================================\n";
        cout << "             SORT CUSTOMERS\n";
        cout << "=========================================\n";
        cout << "1. Sort by Full Name ascending (A - Z)\n";
        cout << "2. Sort by Full Name descending (Z - A)\n";
        cout << "0. Go back\n";

        int opt = InputHelper::getInputInt("Select an option (0-2): ");
        if (opt == 0) break;

        vector<Customer> list = repoCustomer.readAll();
        if (list.empty())
        {
            cout << "[Notice] Customer list is currently empty!\n";
            InputHelper::getInputOptionalString("Press Enter to continue...");
            break;
        }

        if (opt == 1)
        {
            sort(list.begin(), list.end());
            LOG("[CustomerMenu] Customers sorted by name (A - Z)");
            vector<vector<string>> data;
            for (const auto &c : list) data.push_back(c.exportToVector());

            SystemHelper::clearScreen();
            cout << "===== Customers Sorted by Name (A - Z) =====\n";
            TablePrinter::printTable(tableWidths, tableHeaders, data);
            InputHelper::getInputOptionalString("\nPress Enter to continue...");
        }
        else if (opt == 2)
        {
            sort(list.begin(), list.end(), greater<Customer>());
            LOG("[CustomerMenu] Customers sorted by name (Z - A)");
            vector<vector<string>> data;
            for (const auto &c : list) data.push_back(c.exportToVector());

            SystemHelper::clearScreen();
            cout << "===== Customers Sorted by Name (Z - A) =====\n";
            TablePrinter::printTable(tableWidths, tableHeaders, data);
            InputHelper::getInputOptionalString("\nPress Enter to continue...");
        }
        else
        {
            cout << "[Error] Invalid option! Please try again.\n";
            InputHelper::getInputOptionalString("Press Enter to continue...");
        }
    }
}

void CustomerMenu::updateCustomer()
{
    SystemHelper::clearScreen();
    cout << "=========================================\n";
    cout << "           UPDATE CUSTOMER INFO\n";
    cout << "=========================================\n";
    TablePrinter::printTable(tableWidths, tableHeaders, repoCustomer.exportToVector());

    string inputID = convertToUpper(InputHelper::getInputString("Enter Customer ID to update: "));
    Customer *cus = repoCustomer.readByID(inputID);

    if (cus == nullptr)
    {
        cout << "\n[Error] No customer found with ID: " << inputID << "\n";
        InputHelper::getInputOptionalString("Press Enter to return...");
        return;
    }

    SystemHelper::clearScreen();
    cout << "===== Current Customer Information =====\n";
    TablePrinter::printSingle(tableWidths, tableHeaders, cus->exportToVector());

    cout << "\n(Leave field empty to keep current value)\n";

    // 1. Update Name
    // Name is related to contracts, disabling updating names
    // string newName;
    // while (true)
    // {
    //     newName = InputHelper::getInputOptionalString("New Name [" + cus->getName() + "]: ");
    //     if (newName.empty()) break; // Keep current value

    //     try
    //     {
    //         Customer testCus = *cus;
    //         testCus.setName(newName);
    //         break;
    //     }
    //     catch (const exception &e)
    //     {
    //         cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
    //         LOG("[CustomerMenu] Invalid Name input during update: " + string(e.what()));
    //     }
    // }
    cout<<"[Customer Menu] Customer name can not be updated, skipping\n";

    // 2. Update Phone
    string newPhone;
    while (true)
    {
        newPhone = InputHelper::getInputOptionalPhone("New Phone [" + cus->getPhone() + "]: ");
        if (newPhone.empty()) break; // Keep current value

        try
        {
            Customer testCus = *cus;
            testCus.setPhone(newPhone);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[CustomerMenu] Invalid Phone input during update: " + string(e.what()));
        }
    }

    // 3. Update Email
    string newEmail;
    while (true)
    {
        newEmail = InputHelper::getInputOptionalEmail("New Email [" + cus->getEmail() + "]: ");
        if (newEmail.empty()) break; // Keep current value

        try
        {
            Customer testCus = *cus;
            testCus.setEmail(newEmail);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[CustomerMenu] Invalid Email input during update: " + string(e.what()));
        }
    }

    // 4. Update Address
    string newAddress;
    while (true)
    {
        newAddress = InputHelper::getInputOptionalString("New Address [" + cus->getAddress() + "]: ");
        if (newAddress.empty()) break; // Keep current value

        try
        {
            Customer testCus = *cus;
            testCus.setAddress(newAddress);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[CustomerMenu] Invalid Address input during update: " + string(e.what()));
        }
    }

    try
    {
        // if (!newName.empty()) cus->setName(newName);
        if (!newPhone.empty()) cus->setPhone(newPhone);
        if (!newEmail.empty()) cus->setEmail(newEmail);
        if (!newAddress.empty()) cus->setAddress(newAddress);

        if (repoCustomer.update(*cus))
        {
            try
            {
                repoCustomer.writeToFile();
                LOG("[CustomerMenu] Customer " + inputID + " updated successfully");
                cout << "\n[Success] Customer updated successfully!\n";
            }
            catch (const exception &e)
            {
                repoCustomer.loadFromFile();
                LOG("[CustomerMenu] File error while updating customer: " + string(e.what()));
                cout << "\n[File Error] " << e.what() << "\n";
                cout << "Changes discarded. Customer updates were not saved to disk.\n";
            }
        }
        else
        {
            LOG("[CustomerMenu] Failed to update customer: " + inputID);
            cout << "\n[Error] Failed to update customer record!\n";
        }
    }
    catch (const exception &e)
    {
        LOG("[CustomerMenu] Validation exception during update: " + string(e.what()));
        cout << "\n[Validation Error] " << e.what() << "\n";
        cout << "Changes discarded due to invalid input.\n";
        repoCustomer.loadFromFile();
    }

    InputHelper::getInputOptionalString("\nPress Enter to continue...");
}

void CustomerMenu::deleteCustomer()
{
    SystemHelper::clearScreen();
    cout << "=========================================\n";
    cout << "             DELETE CUSTOMER\n";
    cout << "=========================================\n";
    TablePrinter::printTable(tableWidths, tableHeaders, repoCustomer.exportToVector());

    string targetID = convertToUpper(InputHelper::getInputString("Enter Customer ID to delete: "));
    Customer *cus = repoCustomer.readByID(targetID);

    if (cus == nullptr)
    {
        cout << "\n[Error] No customer found with ID: " << targetID << "\n";
        InputHelper::getInputOptionalString("Press Enter to return...");
        return;
    }

    SystemHelper::clearScreen();
    cout << "You are about to delete the following Customer:\n";
    TablePrinter::printSingle(tableWidths, tableHeaders, cus->exportToVector());

    // =====================================================================
    // REFERENTIAL INTEGRITY & DELETE CONSTRAINTS
    // =====================================================================
    // Refresh contract & invoice lists to ensure up-to-date referential integrity
    repoContract.loadFromFile();
    repoHoaDon.loadFromFile();

    // 1. Check if customer is bound to any active or suspended contract
    const vector<Contract> &contractList = repoContract.readAll();
    for (const Contract &c : contractList)
    {
        if (targetID == c.get_customerID() &&
            (c.get_status() == ContractStatus::ACTIVE || c.get_status() == ContractStatus::SUSPENDED))
        {
            LOG("[CustomerMenu] Customer deletion rejected: Active/suspended Contract [" + c.getID() + "]");
            cout << "\n[Deletion Rejected] This customer is currently bound to Contract [" << c.getID() 
                 << "] (Status: " << ContractStatus_toString(c.get_status()) << ")!\n";
            cout << "Please terminate or cancel the active contract before removing the customer profile.\n";
            InputHelper::getInputOptionalString("\nPress Enter to return...");
            return;
        }
    }

    // 2. Check if customer has any unpaid invoices
    const vector<HoaDon> &invoiceList = repoHoaDon.readAll();
    for (const HoaDon &hd : invoiceList)
    {
        if (targetID == hd.getMaKhachHang() && hd.getTrangThai() == "chua_thanh_toan")
        {
            LOG("[CustomerMenu] Customer deletion rejected: Unpaid Invoice [" + hd.getID() + "]");
            cout << "\n[Deletion Rejected] This customer has an unpaid Invoice [" << hd.getID() 
                 << "] (Amount: " << (long long)hd.getSoTien() << " VND)!\n";
            cout << "Please settle all outstanding invoices before deleting the customer.\n";
            InputHelper::getInputOptionalString("\nPress Enter to return...");
            return;
        }
    }

    // =====================================================================
    string confirm = InputHelper::getInputString("\nAre you sure you want to delete this customer? (y/n): ");
    if (confirm == "y" || confirm == "Y")
    {
        if (repoCustomer.remove(targetID))
        {
            try
            {
                repoCustomer.writeToFile();
                LOG("[CustomerMenu] Customer deleted successfully, ID: " + targetID);
                cout << "\n[Success] Customer deleted successfully!\n";
            }
            catch (const exception &e)
            {
                repoCustomer.loadFromFile();
                LOG("[CustomerMenu] File error while deleting customer: " + string(e.what()));
                cout << "\n[File Error] " << e.what() << "\n";
                cout << "Deletion aborted. Customer record restored.\n";
            }
        }
        else
        {
            LOG("[CustomerMenu] Failed to remove customer, ID: " + targetID);
            cout << "\n[Error] Failed to delete customer!\n";
        }
    }
    else
    {
        cout << "\n[Cancelled] Customer deletion cancelled.\n";
        LOG("[CustomerMenu] Delete operation cancelled by user");
    }

    InputHelper::getInputOptionalString("\nPress Enter to return...");
}

void CustomerMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "=========================================\n";
    cout << "           CUSTOMER MANAGEMENT\n";
    cout << "=========================================\n";
    cout << " 1. Add new customer\n";
    cout << " 2. View all customers\n";
    cout << " 3. Search customers\n";
    cout << " 4. Sort customer list\n";
    cout << " 5. Update customer information\n";
    cout << " 6. Delete customer\n";
    cout << "-----------------------------------------\n";
    cout << " 0. Return to main menu\n";
    cout << "=========================================\n";
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
        searchCustomer();
        break;
    case 4:
        sortCustomer();
        break;
    case 5:
        updateCustomer();
        break;
    case 6:
        deleteCustomer();
        break;
    default:
        cout << "[Error] Invalid choice! Please select an option from 0 to 6.\n";
        InputHelper::getInputOptionalString("Press Enter to continue...");
        break;
    }
}
