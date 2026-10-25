// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Pham Quoc Trung (B24DCVT361)
// Use Case 7: Quan ly Hop dong (Contract Menu)
// ===============================================================
#include "ContractMenu.h"
#include "Entities/Contract.h"
#include "Utils/SystemHelper.h"
#include "Utils/StringHelper.h"
#include "Utils/TablePrinter.h"
#include "Utils/InputHelper.h"
#include <iostream>
#include <algorithm>

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

        // Hop dong tam dung van giu thiet bi da cap phat.
        if (c.get_stbID() == stbID &&
            (c.get_status() == ContractStatus::ACTIVE || c.get_status() == ContractStatus::SUSPENDED))
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
            if (pkg->getStatus() != PackageStatus::DANG_CUNG_CAP)
            {
                cout << "[Error] Package " << id << " is not available for new contracts. Please select another package.\n";
                continue;
            }
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
            cout << "[Error] Set-Top Box " << id << " is already allocated to an ACTIVE or SUSPENDED contract!\n";
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

        try
        {
            Date endDate = startDate;
            endDate.addMonths(durationMonths);
            cout << "[Info] Calculated End Date (" << durationMonths << " months): " << endDate.toString() << "\n";
            return endDate;
        }
        catch (const exception &e)
        {
            cout << "[Error] " << e.what() << ". Please choose a smaller duration.\n";
        }
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

    // signDate <= startDate < endDate, signDate <= Today
    SystemHelper::clearScreen();
    cout << "===== Contract Period & Installation =====\n";
    Date signDate;
    while (true)
    {
        signDate = InputHelper::getInputDate("Sign Date (dd/mm/yyyy): ");
        if (Date::getCurrentDate() < signDate)
        {
            cout << "[Error] Sign date cannot be in the future (Today is " << Date::getCurrentDate().toString() << "). Please try again.\n";
            continue;
        }
        break;
    }
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
            Contract dummy;
            dummy.set_monthlyFee(customFee);
            monthlyFee = customFee;
        } catch (const exception &e) {
            cout << "[Warning] " << e.what() << "\nStandard fee will be applied instead.\n";
        }
    }

    // Trang thai hop dong ban dau
    ContractStatus status = inputStatus();

    // Tao va luu hop dong
    Contract newContract(repo.generateNextID(), customerID, packageID, stbID,
                         signDate, startDate, endDate, installationAddress, monthlyFee, status);

    if (repo.create(newContract))
    {
        try
        {
            repo.writeToFile();
            LOG("[ContractMenu] Contract created successfully, ID: " + newContract.getID());
            cout << "\n[Success] Contract created successfully! Assigned ID: " << newContract.getID() << "\n";
        }
        catch (const exception &e)
        {
            repo.loadFromFile();
            LOG("[ContractMenu] Write failed on create, rolling back: " + string(e.what()));
            cout << "\n[Error] Failed to save contract to file! RAM rolled back.\n";
        }
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
        cout << "3. Sort contracts\n";
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
        else if (choice == 3)
        {
            sortContracts();
        }
        else
        {
            cout << "[Error] Invalid option! Please choose again.\n";
            InputHelper::getInputOptionalString("Press Enter to go back...");
        }
    }
}

void ContractMenu::sortContracts()
{
    while (true)
    {
        SystemHelper::clearScreen();
        cout << "===== Sort Contracts =====\n";
        cout << "1. Sort by Start Date (Newest -> Oldest)\n";
        cout << "2. Sort by Monthly Fee (Highest -> Lowest)\n";
        cout << "0. Go back\n";

        int choice = InputHelper::getInputInt("Select an option (0-2): ");
        if (choice == 0) return;

        vector<Contract> list = repo.readAll();
        if (list.empty())
        {
            cout << "[Notice] No contracts available to display.\n";
            InputHelper::getInputOptionalString("Press Enter to go back...");
            return;
        }

        SystemHelper::clearScreen();
        if (choice == 1)
        {
            cout << "===== Contracts Sorted by Start Date (Newest -> Oldest) =====\n";
            sort(list.begin(), list.end(), [](const Contract &a, const Contract &b) {
                return b.get_startDate() < a.get_startDate();
            });
            LOG("[ContractMenu] Contracts sorted by start date (Newest -> Oldest)");
        }
        else if (choice == 2)
        {
            cout << "===== Contracts Sorted by Monthly Fee (Highest -> Lowest) =====\n";
            sort(list.begin(), list.end(), [](const Contract &a, const Contract &b) {
                long long feeA = 0, feeB = 0;
                try { feeA = stoll(a.get_monthlyFee()); } catch (...) {}
                try { feeB = stoll(b.get_monthlyFee()); } catch (...) {}
                return feeA > feeB;
            });
            LOG("[ContractMenu] Contracts sorted by monthly fee (Highest -> Lowest)");
        }
        else
        {
            cout << "[Error] Invalid option! Please choose again.\n";
            InputHelper::getInputOptionalString("Press Enter to go back...");
            continue;
        }

        vector<vector<string>> data;
        for (const auto &c : list)
        {
            data.push_back(c.exportToVector());
        }

        printContractTable(data);
        cout << "Total: " << list.size() << " record(s)\n";
        InputHelper::getInputOptionalString("Press Enter to continue...");
    }
}


// UPDATE ===================================================================

void ContractMenu::updateContract()
{
    SystemHelper::clearScreen();
    cout << "===== Update Contract =====\n";
    printContractTable(repo.exportToVector());

    string targetID = convertToUpper(InputHelper::getInputString("Enter Contract ID to edit (or 0 to cancel): "));
    if (targetID == "0") return;

    Contract *contract = repo.readByID(targetID);

    if (contract == nullptr)
    {
        cout << "[Error] Contract not found with ID: " << targetID << "\n";
        InputHelper::getInputOptionalString("Press Enter to go back...");
        return;
    }

    vector<int> widths = {7, 12, 12, 12, 12, 12, 12, 20, 15, 12};
    vector<string> headers = {"ID", "Customer ID", "Package ID", "Set-Top Box", "Sign Date", "Start Date", "End Date", "Address", "Monthly Fee", "Status"};

    SystemHelper::clearScreen();
    cout << "===== Current Contract Information: " << contract->getID() << " =====\n";
    TablePrinter::printSingle(widths, headers, contract->exportToVector());

    cout << "\n(Leave field empty / Press Enter to keep current value)\n";

    Contract candidate = *contract;

    // 1. Update Package ID
    while (true)
    {
        string newPkgID = convertToUpper(InputHelper::getInputOptionalString("New Package ID [" + candidate.get_packageID() + "] (type '?' to list, or press Enter to keep): "));
        if (newPkgID.empty() || newPkgID == candidate.get_packageID())
        {
            break; // Keep current
        }

        if (newPkgID == "?")
        {
            printPackageTable();
            continue;
        }

        Package *pkg = repoPackage.readByID(newPkgID);
        if (pkg == nullptr)
        {
            cout << "[Error] Package not found: " << newPkgID << ". Please try again.\n";
            continue;
        }
        if (pkg->getStatus() != PackageStatus::DANG_CUNG_CAP)
        {
            cout << "[Error] Package " << newPkgID << " is not available for new contracts. Current package kept; choose another or press Enter.\n";
            continue;
        }

        try
        {
            candidate.set_packageID(newPkgID);
            cout << "[Notice] Package updated to: " << newPkgID << "\n";

            // Prompt for monthly fee
            string defaultFee = to_string(pkg->getMonthlyPrice());
            cout << "Standard monthly fee for package " << newPkgID << ": " << defaultFee << " VND\n";
            string customFee = InputHelper::getInputOptionalString("Agreed Monthly Fee [Current: " + candidate.get_monthlyFee() + ", Standard: " + defaultFee + "] (press Enter to keep current fee): ");
            if (!customFee.empty())
            {
                try {
                    Contract dummy;
                    dummy.set_monthlyFee(customFee);
                    candidate.set_monthlyFee(customFee);
                    cout << "[Notice] Monthly fee set to: " << customFee << " VND\n";
                } catch (const exception &e) {
                    cout << "[Warning] " << e.what() << "\nCurrent fee kept.\n";
                }
            }
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
        }
    }

    // 2. Update Set-Top Box ID
    while (true)
    {
        string newStbID = convertToUpper(InputHelper::getInputOptionalString("New Set-Top Box ID [" + candidate.get_stbID() + "] (type '?' to list, or press Enter to keep): "));
        if (newStbID.empty() || newStbID == candidate.get_stbID())
        {
            break; // Keep current
        }

        if (newStbID == "?")
        {
            printSetTopBoxTable();
            continue;
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

        if (isSetTopBoxInUse(newStbID, candidate.getID()))
        {
            cout << "[Error] Set-Top Box " << newStbID << " is currently used by another ACTIVE or SUSPENDED contract!\n";
            continue;
        }

        try
        {
            candidate.set_stbID(newStbID);
            cout << "[Notice] Set-Top Box updated to: " << newStbID << "\n";
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
        }
    }

    // 3. Extend Duration Months
    while (true)
    {
        string extendStr = InputHelper::getInputOptionalString("Extend duration in months [Current End Date: " + candidate.get_endDate().toString() + "] (press Enter or 0 to keep): ");
        if (extendStr.empty() || extendStr == "0")
        {
            break; // Keep current
        }

        try
        {
            if (!all_of(extendStr.begin(), extendStr.end(), [](unsigned char c) { return c >= '0' && c <= '9'; }))
                throw invalid_argument("Number of months must contain only digits.");
            int addMonths = stoi(extendStr);
            if (addMonths <= 0)
            {
                cout << "[Error] Number of months must be greater than 0!\n";
                continue;
            }

            Date updatedEndDate = candidate.get_endDate();
            updatedEndDate.addMonths(addMonths);

            candidate.set_endDate(updatedEndDate);

            if (candidate.get_status() == ContractStatus::EXPIRED)
            {
                string reactivate = InputHelper::getInputString("Contract was EXPIRED. Reactivate to ACTIVE? (y/n): ");
                if (reactivate == "y" || reactivate == "Y")
                {
                    if (isSetTopBoxInUse(candidate.get_stbID(), candidate.getID()))
                    {
                        cout << "[Error] Cannot reactivate to ACTIVE: Set-Top Box " << candidate.get_stbID() << " is in use by another ACTIVE or SUSPENDED contract!\n";
                    }
                    else
                    {
                        candidate.set_status(ContractStatus::ACTIVE);
                    }
                }
            }

            cout << "[Notice] Contract extended by " << addMonths << " months! New End Date: " << updatedEndDate.toString() << "\n";
            break;
        }
        catch (const exception &e)
        {
            cout << "[Error] Invalid input for months: " << e.what() << " Please enter a positive integer within the supported date range.\n";
        }
    }

    // 4. Update Installation Address
    while (true)
    {
        string newAddress = InputHelper::getInputOptionalString("New Installation Address [" + candidate.get_installationAddress() + "]: ");
        if (newAddress.empty() || newAddress == candidate.get_installationAddress())
        {
            break; // Keep current
        }

        try
        {
            candidate.set_installationAddress(newAddress);
            cout << "[Notice] Installation address updated.\n";
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
        }
    }

    // 5. Update Status
    cout << "\nChange Status [Current: " << ContractStatus_toString(candidate.get_status()) << "]:\n";
    cout << "1. ACTIVE\n2. SUSPENDED\n3. EXPIRED\n4. CANCELED\n0. Keep current status\n";
    string statusChoice = InputHelper::getInputOptionalString("Select status (0-4, or press Enter to keep): ");
    if (!statusChoice.empty() && statusChoice != "0")
    {
        if (statusChoice == "1")
        {
            if (isSetTopBoxInUse(candidate.get_stbID(), candidate.getID()))
            {
                cout << "[Error] Cannot set status to ACTIVE: Set-Top Box " << candidate.get_stbID()
                     << " is currently in use by another ACTIVE or SUSPENDED contract!\n";
            }
            else
            {
                candidate.set_status(ContractStatus::ACTIVE);
                cout << "[Notice] Contract status updated to ACTIVE.\n";
            }
        }
        else if (statusChoice == "2")
        {
            if (isSetTopBoxInUse(candidate.get_stbID(), candidate.getID()))
            {
                cout << "[Error] Cannot set status to SUSPENDED: Set-Top Box " << candidate.get_stbID()
                     << " is currently in use by another ACTIVE or SUSPENDED contract!\n";
            }
            else
            {
                candidate.set_status(ContractStatus::SUSPENDED);
                cout << "[Notice] Contract status updated to SUSPENDED.\n";
            }
        }
        else if (statusChoice == "3")
        {
            candidate.set_status(ContractStatus::EXPIRED);
            cout << "[Notice] Contract status updated to EXPIRED.\n";
        }
        else if (statusChoice == "4")
        {
            repoHoaDon.loadFromFile();
            bool hasUnpaid = false;
            for (const auto &hd : repoHoaDon.readAll())
            {
                if (hd.getMaHopDong() == candidate.getID() && hd.getTrangThai() != "Paid")
                {
                    hasUnpaid = true;
                    break;
                }
            }
            if (hasUnpaid)
            {
                cout << "[Error] Cannot cancel contract " << candidate.getID()
                     << ": Contract has unpaid invoice(s)! Please resolve invoices before cancelling.\n";
            }
            else
            {
                candidate.set_status(ContractStatus::CANCELED);
                cout << "[Notice] Contract status updated to CANCELED.\n";
            }
        }
        else
        {
            cout << "[Notice] Invalid choice. Current status kept.\n";
        }
    }

    // Save immediately to file with candidate clone and rollback protection
    if (repo.update(candidate))
    {
        try
        {
            repo.writeToFile();
            LOG("[ContractMenu] Contract " + candidate.getID() + " updated and saved");
            cout << "\n[Success] Contract updated and saved successfully!\n";
        }
        catch (const exception &e)
        {
            repo.loadFromFile();
            LOG("[ContractMenu] Write failed on update, rolling back: " + string(e.what()));
            cout << "\n[Error] Failed to save changes to file! RAM rolled back.\n";
        }
    }
    else
    {
        LOG("[ContractMenu] Failed to update contract " + candidate.getID());
        cout << "\n[Error] Failed to update contract!\n";
    }

    InputHelper::getInputOptionalString("Press Enter to go back...");
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
            try
            {
                repo.writeToFile();
                cout << "[Success] Contract deleted successfully, ID: " << targetID << "\n";
                LOG("[ContractMenu] Contract deleted successfully, ID: " + targetID);
            }
            catch (const exception &e)
            {
                repo.loadFromFile();
                LOG("[ContractMenu] Write failed on delete, rolling back: " + string(e.what()));
                cout << "\n[Error] Failed to save deletion to file! RAM rolled back.\n";
            }
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
