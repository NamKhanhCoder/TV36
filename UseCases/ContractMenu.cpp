#include "ContractMenu.h"
#include "Entities/Contract.h"
#include "Utils/SystemHelper.h"
#include "Utils/StringHelper.h"
#include "Utils/TablePrinter.h"
#include "Utils/InputHelper.h"
#include <iostream>

using namespace std;


// CONSTRUCTOR & INITIALIZATION ====================================================================
ContractMenu::ContractMenu() : 
    repo("Data/contracts.txt", "Data/contracts_counter.txt", "HD"),
    repoCustomer("Data/customers.txt", "Data/customers_counter.txt", "KH"),
    repoPackage("Data/packages.txt", "Data/packages_counter.txt", "GOI"),
    repoSetTopBox("Data/settopboxes.txt", "Data/settopboxes_counter.txt", "STB"),
    repoHoaDon("Data/hoadon.txt", "Data/hdn_counter.txt", "HDN"),
    repoSupportTicket("Data/support_tickets.txt", "Data/support_tickets_counter.txt", "PHT")
{
    repo.loadFromFile();   
    repoCustomer.loadFromFile();
    repoPackage.loadFromFile();
    repoSetTopBox.loadFromFile();
    repoHoaDon.loadFromFile();
    repoSupportTicket.loadFromFile();
}


// TABLE PRINTING HELPERS ===================================================================
void ContractMenu::printCustomerTable()
{
    vector<int> widths = {8, 22, 12, 25, 20};
    vector<string> headers = {"ID", "Name", "Phone", "Email", "Address"};
    cout << "\n===== All Customers =====\n";
    TablePrinter::printTable(widths, headers, repoCustomer.exportToVector());
}

void ContractMenu::printPackageTable()
{
    vector<int> widths = {8, 20, 15, 16, 25, 20};
    vector<string> headers = {"ID", "Name", "Monthly Price", "Status", "Channels", "VODs"};
    cout << "\n===== All Packages =====\n";
    TablePrinter::printTable(widths, headers, repoPackage.exportToVector());
}

void ContractMenu::printSetTopBoxTable()
{
    vector<int> widths = {8, 15, 15, 15, 12, 15};
    vector<string> headers = {"ID", "Serial", "Manufacturer", "Model", "Import Date", "Status"};
    cout << "\n===== All Set-Top Boxes =====\n";
    TablePrinter::printTable(widths, headers, repoSetTopBox.exportToVector());
}

void ContractMenu::printContractTable(const vector<vector<string>> &data)
{
    vector<int> widths = {7, 12, 12, 12, 12, 12, 12, 20, 15, 12};
    vector<string> headers = {"ID", "Customer ID", "Package ID", "Set-Top Box", "Sign Date", "Start Date", "End Date", "Address", "Monthly Fee", "Status"};
    TablePrinter::printTable(widths, headers, data);
}


// VALIDATION & SELECTION HELPERS ===================================================================
bool ContractMenu::isSetTopBoxInUse(const string &stbID, const string &excludeContractID)
{
    for (const auto &c : repo.readAll())
    {
        if (!excludeContractID.empty() && c.getID() == excludeContractID)
            continue;

        // Neu hop dong dang hoat dong ma xai STB nay thi STB duoc coi la dang su dung
        if (c.get_stbID() == stbID && c.get_status() == ContractStatus::ACTIVE)
            return true;
    }
    return false;
}

string ContractMenu::selectCustomerID()
{
    printCustomerTable();
    while (true)
    {
        string id = convertToUpper(InputHelper::getInputString("Select Customer ID (or 0 to cancel): "));
        if (id == "0") return "";

        Customer *cus = repoCustomer.readByID(id);
        if (cus != nullptr)
        {
            return id;
        }
        cout << "[Error] Customer ID not found: " << id << ". Please choose from the table above.\n";
    }
}

string ContractMenu::selectPackageID()
{
    printPackageTable();
    while (true)
    {
        string id = convertToUpper(InputHelper::getInputString("Select Package ID (or 0 to cancel): "));
        if (id == "0") return "";

        Package *pkg = repoPackage.readByID(id);
        if (pkg != nullptr)
        {
            return id;
        }
        cout << "[Error] Package ID not found: " << id << ". Please choose from the table above.\n";
    }
}

string ContractMenu::selectSetTopBoxID(const string &currentContractID)
{
    printSetTopBoxTable();
    while (true)
    {
        string id = convertToUpper(InputHelper::getInputString("Select Set-Top Box ID (or 0 to cancel): "));
        if (id == "0") return "";

        SetTopBox *stb = repoSetTopBox.readByID(id);
        if (stb == nullptr)
        {
            cout << "[Error] Set-Top Box ID not found: " << id << ". Please choose from the table above.\n";
            continue;
        }

        if (stb->getTechnicalStatus() != TechnicalStatus::TOT)
        {
            cout << "[Error] Set-Top Box " << id << " cannot be allocated because its status is: " 
                 << stb->getTechnicalStatusString() << " (must be TOT).\n";
            cout << "Please select a functional Set-Top Box.\n";
            continue;
        }

        if (isSetTopBoxInUse(id, currentContractID))
        {
            cout << "[Error] Set-Top Box " << id << " is already allocated to another active contract!\n";
            cout << "Please select an available Set-Top Box.\n";
            continue;
        }

        return id;
    }
}

Date ContractMenu::inputValidStartDate(const Date &signDate)
{
    while (true)
    {
        Date startDate = InputHelper::getInputDate("Start Date (dd/mm/yyyy): ");
        if (startDate < signDate)
        {
            cout << "[Error] Invalid date: Start Date must be on or after Sign Date (" << signDate.toString() << ").\n";
            continue;
        }
        return startDate;
    }
}

Date ContractMenu::calculateEndDate(const Date &startDate)
{
    while (true)
    {
        int durationMonths = InputHelper::getInputInt("Contract duration in months (e.g. 1, 3, 6, 12): ");
        if (durationMonths <= 0)
        {
            cout << "[Error] Duration must be a positive integer (> 0).\n";
            continue;
        }

        Date endDate = startDate;
        endDate.addMonths(durationMonths);
        cout << "[Info] Calculated End Date (" << durationMonths << " months): " << endDate.toString() << "\n";
        return endDate;
    }
}

ContractStatus ContractMenu::inputStatus()
{
    while (true)
    {
        cout << "\nSelect contract status:\n";
        cout << "1. ACTIVE\n";
        cout << "2. SUSPENDED\n";
        cout << "3. EXPIRED\n";
        cout << "4. CANCELED\n";

        int s = InputHelper::getInputInt("Contract's status (1-4): ");
        if (s == 1) return ContractStatus::ACTIVE;
        if (s == 2) return ContractStatus::SUSPENDED;
        if (s == 3) return ContractStatus::EXPIRED;
        if (s == 4) return ContractStatus::CANCELED;

        cout << "[Error] Invalid choice! Please enter a number from 1 to 4.\n";
    }
}

ContractStatus ContractMenu::inputOptionalStatus(ContractStatus currentStatus)
{
    while (true)
    {
        cout << "\nSelect contract status (Current: " << ContractStatus_toString(currentStatus) << "):\n";
        cout << "1. ACTIVE\n";
        cout << "2. SUSPENDED\n";
        cout << "3. EXPIRED\n";
        cout << "4. CANCELED\n";
        cout << "0. Keep current status\n";

        int s = InputHelper::getInputInt("Your choice (0-4): ");
        if (s == 1) return ContractStatus::ACTIVE;
        if (s == 2) return ContractStatus::SUSPENDED;
        if (s == 3) return ContractStatus::EXPIRED;
        if (s == 4) return ContractStatus::CANCELED;
        if (s == 0) return currentStatus;

        cout << "[Error] Invalid choice! Please try again.\n";
    }
}

// CRUD ===================================================================

void ContractMenu::createContract()
{
    SystemHelper::clearScreen();
    cout << "=========================================\n";
    cout << "         CREATE NEW CONTRACT\n";
    cout << "=========================================\n";

    // Khach hang kiem tra ton tai
    string customerID = selectCustomerID();
    if (customerID.empty())
    {
        cout << "[Notice] Contract creation cancelled.\n";
        InputHelper::getInputOptionalString("Press Enter to go back...");
        return;
    }

    // Goi cuoc kiem tra ton tai
    string packageID = selectPackageID();
    if (packageID.empty())
    {
        cout << "[Notice] Contract creation cancelled.\n";
        InputHelper::getInputOptionalString("Press Enter to go back...");
        return;
    }

    // Set-Top Box kiem tra ton tai, phai TOT, va khong trung thiet bi dang dung
    string stbID = selectSetTopBoxID();
    if (stbID.empty())
    {
        cout << "[Notice] Contract creation cancelled.\n";
        InputHelper::getInputOptionalString("Press Enter to go back...");
        return;
    }

    // signDate <= startDate < endDate
    SystemHelper::clearScreen();
    cout << "===== Contract Period & Installation =====\n";
    Date signDate = InputHelper::getInputDate("Sign Date (dd/mm/yyyy): ");
    Date startDate = inputValidStartDate(signDate);
    Date endDate = calculateEndDate(startDate);

    // Dia chi
    string installationAddress;
    while (true)
    {
        installationAddress = InputHelper::getInputString("Installation Address: ");
        try
        {
            Contract dummy;
            dummy.set_installationAddress(installationAddress);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
        }
    }

    // Tinh phi thang: Mac dinh theo Package, co the tuy chinh gia thoa thuan
    Package *pkg = repoPackage.readByID(packageID);
    string defaultFee = to_string(pkg->getMonthlyPrice());
    cout << "\nPackage standard monthly fee: " << defaultFee << " VND\n";
    string customFee = InputHelper::getInputOptionalString("Enter agreed monthly fee (press Enter to use standard fee): ");
    string monthlyFee = defaultFee;
    if (!customFee.empty())
    {
        try {
            long long fee = stoll(customFee);
            if (fee > 0) monthlyFee = customFee;
            else cout << "[Warning] Invalid fee. Standard fee will be applied.\n";
        } catch (...) {
            cout << "[Warning] Invalid fee format. Standard fee will be applied.\n";
        }
    }

    // Trang thai hop dong ban dau
    ContractStatus status = inputStatus();

    // Tao va luu hop dong
    Contract newContract(repo.generateNextID(), customerID, packageID, stbID,
                         signDate, startDate, endDate, installationAddress, monthlyFee, status);

    if (repo.create(newContract))
    {
        repo.writeToFile();
        LOG("[ContractMenu] Contract created successfully, ID: " + newContract.getID());
        cout << "\n[Success] Contract created successfully! Assigned ID: " << newContract.getID() << "\n";
    }
    else
    {
        LOG("[ContractMenu] Contract creation failed");
        cout << "\n[Error] Contract creation failed!\n";
    }
    InputHelper::getInputOptionalString("Press Enter to go back...");
}

void ContractMenu::readContract()
{
    while (true)
    {
        SystemHelper::clearScreen();
        cout << "===== View Contracts =====\n";
        cout << "1. View all contracts\n";
        cout << "2. Search contract by ID\n";
        cout << "0. Go back\n";

        int choice = InputHelper::getInputInt("Select an option: ");
        if (choice == 0) break;

        if (choice == 1)
        {
            SystemHelper::clearScreen();
            cout << "===== All Contracts =====\n";
            printContractTable(repo.exportToVector());
            cout << "Total: " << repo.readAll().size() << " record(s)\n";
            LOG("[ContractMenu] All contracts viewed");
            InputHelper::getInputOptionalString("Press Enter to go back...");
        }
        else if (choice == 2)
        {
            string searchID = convertToUpper(InputHelper::getInputString("Enter Contract ID: "));
            Contract *contract = repo.readByID(searchID);
            if (contract == nullptr)
            {
                cout << "[Error] Contract not found with ID: " << searchID << "\n";
            }
            else
            {
                vector<int> widths = {7, 12, 12, 12, 12, 12, 12, 20, 15, 12};
                vector<string> headers = {"ID", "Customer ID", "Package ID", "Set-Top Box", "Sign Date", "Start Date", "End Date", "Address", "Monthly Fee", "Status"};
                TablePrinter::printSingle(widths, headers, contract->exportToVector());
            }
            LOG("[ContractMenu] Search contract completed");
            InputHelper::getInputOptionalString("Press Enter to go back...");
        }
        else
        {
            cout << "[Error] Invalid option! Please choose again.\n";
            InputHelper::getInputOptionalString("Press Enter to go back...");
        }
    }
}


// UPDATE SUB-FUNCTIONS

void ContractMenu::updatePackage(Contract *contract)
{
    cout << "\n--- Change Service Package ---\n";
    cout << "Current Package ID: " << contract->get_packageID() << "\n";

    printPackageTable();
    while (true)
    {
        string newPackageID = convertToUpper(InputHelper::getInputString("Enter new Package ID (or 0 to cancel): "));
        if (newPackageID == "0") return;

        if (newPackageID == contract->get_packageID())
        {
            cout << "[Notice] Contract is already using this package. Nothing changed.\n";
            return;
        }

        Package *pkg = repoPackage.readByID(newPackageID);
        if (pkg == nullptr)
        {
            cout << "[Error] Package not found: " << newPackageID << ". Please try again.\n";
            continue;
        }

        contract->set_packageID(newPackageID);

        // Cap nhat lai gia tien theo goi cuoc moi
        string defaultFee = to_string(pkg->getMonthlyPrice());
        cout << "New package standard monthly fee: " << defaultFee << " VND\n";
        string customFee = InputHelper::getInputOptionalString("Enter agreed monthly fee (press Enter to use standard fee): ");
        string newMonthlyFee = defaultFee;
        if (!customFee.empty())
        {
            try {
                long long fee = stoll(customFee);
                if (fee > 0) newMonthlyFee = customFee;
                else cout << "[Warning] Invalid fee. Standard fee applied.\n";
            } catch (...) {
                cout << "[Warning] Invalid fee format. Standard fee applied.\n";
            }
        }

        contract->set_monthlyFee(newMonthlyFee);
        cout << "[Success] Package updated to " << newPackageID << " with fee " << newMonthlyFee << " VND!\n";
        break;
    }
}

void ContractMenu::updateSetTopBox(Contract *contract)
{
    cout << "\n--- Change Set-Top Box (Warranty / Replace) ---\n";
    cout << "Current Set-Top Box ID: " << contract->get_stbID() << "\n";

    printSetTopBoxTable();
    while (true)
    {
        string newStbID = convertToUpper(InputHelper::getInputString("Enter new Set-Top Box ID (or 0 to cancel): "));
        if (newStbID == "0") return;

        if (newStbID == contract->get_stbID())
        {
            cout << "[Notice] Contract is already allocated with this Set-Top Box.\n";
            return;
        }

        SetTopBox *stb = repoSetTopBox.readByID(newStbID);
        if (stb == nullptr)
        {
            cout << "[Error] Set-Top Box not found: " << newStbID << ". Please try again.\n";
            continue;
        }

        if (stb->getTechnicalStatus() != TechnicalStatus::TOT)
        {
            cout << "[Error] Set-Top Box " << newStbID << " is not in good condition (Status: " 
                 << stb->getTechnicalStatusString() << "). Must be TOT.\n";
            continue;
        }

        if (isSetTopBoxInUse(newStbID, contract->getID()))
        {
            cout << "[Error] Set-Top Box " << newStbID << " is currently used by another active contract!\n";
            continue;
        }

        contract->set_stbID(newStbID);
        cout << "[Success] Set-Top Box updated to " << newStbID << "!\n";
        break;
    }
}

void ContractMenu::extendContract(Contract *contract)
{
    cout << "\n--- Extend Contract Duration ---\n";
    cout << "Current End Date: " << contract->get_endDate().toString() << "\n";

    while (true)
    {
        int addMonths = InputHelper::getInputInt("Enter number of months to extend (> 0, or 0 to cancel): ");
        if (addMonths == 0) return;

        if (addMonths < 0)
        {
            cout << "[Error] Number of months must be greater than 0!\n";
            continue;
        }

        Date updatedEndDate = contract->get_endDate();
        updatedEndDate.addMonths(addMonths);
        contract->set_endDate(updatedEndDate);

        if (contract->get_status() == ContractStatus::EXPIRED)
        {
            string reactivate = InputHelper::getInputString("Contract was EXPIRED. Reactivate to ACTIVE? (y/n): ");
            if (reactivate == "y" || reactivate == "Y")
            {
                contract->set_status(ContractStatus::ACTIVE);
            }
        }

        cout << "[Success] Contract extended by " << addMonths << " months! New End Date: " << updatedEndDate.toString() << "\n";
        break;
    }
}

void ContractMenu::updateAddress(Contract *contract)
{
    cout << "\n--- Change Installation Address ---\n";
    cout << "Current Address: " << contract->get_installationAddress() << "\n";

    while (true)
    {
        string newAddress = InputHelper::getInputOptionalString("Enter new installation address (or leave empty to cancel): ");
        if (newAddress.empty())
        {
            cout << "[Notice] Address update cancelled.\n";
            return;
        }

        try
        {
            contract->set_installationAddress(newAddress);
            cout << "[Success] Installation address updated successfully!\n";
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
        }
    }
}

void ContractMenu::updateStatus(Contract *contract)
{
    cout << "\n--- Change Contract Status ---\n";
    ContractStatus newStatus = inputOptionalStatus(contract->get_status());
    contract->set_status(newStatus);
    cout << "[Success] Contract status updated to: " << ContractStatus_toString(newStatus) << "\n";
}

void ContractMenu::updateContract()
{
    SystemHelper::clearScreen();
    cout << "===== Update Contract =====\n";
    printContractTable(repo.exportToVector());

    string targetID = convertToUpper(InputHelper::getInputString("Enter Contract ID to edit: "));
    Contract *contract = repo.readByID(targetID);

    if (contract == nullptr)
    {
        cout << "[Error] Contract not found with ID: " << targetID << "\n";
        InputHelper::getInputOptionalString("Press Enter to go back...");
        return;
    }

    vector<int> widths = {7, 12, 12, 12, 12, 12, 12, 20, 15, 12};
    vector<string> headers = {"ID", "Customer ID", "Package ID", "Set-Top Box", "Sign Date", "Start Date", "End Date", "Address", "Monthly Fee", "Status"};

    while (true)
    {
        SystemHelper::clearScreen();
        cout << "===== Current Contract Information: " << contract->getID() << " =====\n";
        TablePrinter::printSingle(widths, headers, contract->exportToVector());

        cout << "\n===== Update Options =====\n";
        cout << "1. Doi goi cuoc (Change Service Package)\n";
        cout << "2. Thay moi Set-Top Box (Replace / Warranty STB)\n";
        cout << "3. Gia han goi dich vu (Extend Duration Months)\n";
        cout << "4. Doi dia chi lap dat (Change Address)\n";
        cout << "5. Thay doi trang thai hop dong (Change Status)\n";
        cout << "0. Save & Exit\n";

        int choice = InputHelper::getInputInt("Select an option (0-5): ");
        if (choice == 0)
        {
            repo.writeToFile();
            cout << "\n[Success] All changes saved to file successfully!\n";
            LOG("[ContractMenu] Contract " + contract->getID() + " updated and saved");
            InputHelper::getInputOptionalString("Press Enter to go back...");
            break;
        }

        switch (choice)
        {
        case 1:
            updatePackage(contract);
            break;
        case 2:
            updateSetTopBox(contract);
            break;
        case 3:
            extendContract(contract);
            break;
        case 4:
            updateAddress(contract);
            break;
        case 5:
            updateStatus(contract);
            break;
        default:
            cout << "[Error] Invalid option! Please select 0 to 5.\n";
            break;
        }

        InputHelper::getInputOptionalString("Press Enter to continue...");
    }
}

void ContractMenu::deleteContract()
{
    SystemHelper::clearScreen();
    cout << "\n===== Delete Contract =====\n";
    printContractTable(repo.exportToVector());

    string targetID = convertToUpper(InputHelper::getInputString("Enter Contract ID to delete: "));
    Contract *contract = repo.readByID(targetID);

    if (contract == nullptr)
    {
        cout << "[Error] Contract not found: " << targetID << "\n";
        InputHelper::getInputOptionalString("Press Enter to go back...");
        return;
    }
    
    repoHoaDon.loadFromFile();
    repoSupportTicket.loadFromFile();

    bool hasHoaDon = false;
    for (const auto &hd : repoHoaDon.readAll())
    {
        if (hd.getMaHopDong() == targetID)
        {
            hasHoaDon = true;
            break;
        }
    }

    bool hasTicket = false;
    for (const auto &ticket : repoSupportTicket.readAll())
    {
        if (ticket.getContractID() == targetID)
        {
            hasTicket = true;
            break;
        }
    }

    if (hasHoaDon || hasTicket)
    {
        cout << "\n[Error] Cannot delete contract " << targetID << "!\n";
        cout << "Reason: Referential integrity constraint violation:\n";
        if (hasHoaDon)
            cout << " - Contract is referenced by one or more Invoices (HoaDon).\n";
        if (hasTicket)
            cout << " - Contract is referenced by one or more Support Tickets (Phieu ho tro).\n";
        cout << "Please remove related invoices/tickets before deleting this contract.\n";
        InputHelper::getInputOptionalString("Press Enter to go back...");
        return;
    }

    vector<int> widths = {7, 12, 12, 12, 12, 12, 12, 20, 15, 12};
    vector<string> headers = {"ID", "Customer ID", "Package ID", "Set-Top Box", "Sign Date", "Start Date", "End Date", "Address", "Monthly Fee", "Status"};
    cout << "\nYou are about to remove this Contract:\n";
    TablePrinter::printSingle(widths, headers, contract->exportToVector());

    string confirm = InputHelper::getInputString("Are you sure you want to delete this contract? (y/n): ");
    if (confirm == "y" || confirm == "Y")
    {
        if (repo.remove(targetID))
        {
            repo.writeToFile();
            cout << "[Success] Contract deleted successfully, ID: " << targetID << "\n";
            LOG("[ContractMenu] Contract deleted successfully, ID: " + targetID);
        }
        else
        {
            cout << "[Error] Contract deletion failed!\n";
        }
    }
    else
    {
        cout << "[Abort] Deletion cancelled by user.\n";
        LOG("[ContractMenu] Contract deletion cancelled");
    }
    InputHelper::getInputOptionalString("Press Enter to go back...");
}

void ContractMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "=========================================\n";
    cout << "           CONTRACT MANAGEMENT\n";
    cout << "=========================================\n";
    cout << "1. Create a Contract\n";
    cout << "2. Read / Search Contracts\n";
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
        cout << "[Error] Invalid option in Contract menu!\n";
        InputHelper::getInputOptionalString("Press Enter to go back...");
        break;
    }
}
