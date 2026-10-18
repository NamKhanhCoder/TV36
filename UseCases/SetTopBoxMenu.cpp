// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193)
// Use Case 2: Quan ly Thiet bi Set-top Box (STB Menu)
// ===============================================================
#include "UseCases/SetTopBoxMenu.h"
#include "Utils/Debugger.h"
#include "Utils/TablePrinter.h"
#include "Utils/SystemHelper.h"
#include "Utils/InputHelper.h"
#include "Utils/StringHelper.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

using namespace std;

namespace
{
    const vector<int> tableWidths = {8, 16, 16, 16, 14, 16};
    const vector<string> tableHeaders = {"ID", "Serial", "Manufacturer", "Model", "Import Date", "Status"};
}

SetTopBoxMenu::SetTopBoxMenu()
    : repoSetTopBox("Data/settopboxes.txt", "Data/settopboxes_counter.txt", "STB"),
      repoContract("Data/contracts.txt", "Data/contracts_counter.txt", "HD"),
      repoSupportTicket("Data/support_tickets.txt", "Data/support_tickets_counter.txt", "PHT")
{
    repoSetTopBox.loadFromFile();
    repoContract.loadFromFile();
    repoSupportTicket.loadFromFile();
}

TechnicalStatus SetTopBoxMenu::inputTechnicalStatus(bool isOptional, TechnicalStatus defaultStatus)
{
    while (true)
    {
        cout << "\nSelect technical status:" << endl;
        cout << "1. TOT (Good / Operational)" << endl;
        cout << "2. HONG (Broken / Faulty)" << endl;
        cout << "3. BAO_TRI (Under Maintenance)" << endl;
        cout << "4. NGUNG_SU_DUNG (Decommissioned / Discarded)" << endl;

        string prompt = isOptional ? "Enter choice (1-4) or leave empty to keep current: " : "Enter choice (1-4): ";
        string choiceStr = isOptional ? InputHelper::getInputOptionalString(prompt) : InputHelper::getInputString(prompt);

        if (isOptional && choiceStr.empty())
            return defaultStatus;

        if (choiceStr == "1") return TOT;
        if (choiceStr == "2") return HONG;
        if (choiceStr == "3") return BAO_TRI;
        if (choiceStr == "4") return NGUNG_SU_DUNG;

        cout << "[Error] Invalid choice! Please enter a number from 1 to 4.\n";
    }
}

void SetTopBoxMenu::createSetTopBox()
{
    SystemHelper::clearScreen();
    cout << "=========================================\n";
    cout << "          ADD NEW SET-TOP BOX\n";
    cout << "=========================================\n";

    SetTopBox temp;

    // 1. Serial (unique & valid)
    while (true)
    {
        string serial = InputHelper::getInputString("Enter device Serial (must be unique): ");
        bool exists = false;
        for (const auto &stb : repoSetTopBox.readAll())
        {
            if (stb.getSerial() == serial)
            {
                exists = true;
                break;
            }
        }
        if (exists)
        {
            cout << "[Error] This Serial number already exists in inventory! Please enter another serial.\n";
            continue;
        }

        try
        {
            temp.setSerial(serial);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[SetTopBoxMenu] Invalid Serial input during creation: " + string(e.what()));
        }
    }

    // 2. Manufacturer
    while (true)
    {
        string manufacturer = InputHelper::getInputString("Enter manufacturer: ");
        try
        {
            temp.setManufacturer(manufacturer);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[SetTopBoxMenu] Invalid Manufacturer input during creation: " + string(e.what()));
        }
    }

    // 3. Model
    while (true)
    {
        string model = InputHelper::getInputString("Enter device model: ");
        try
        {
            temp.setModel(model);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[SetTopBoxMenu] Invalid Model input during creation: " + string(e.what()));
        }
    }

    // 4. Import Date
    while (true)
    {
        Date importDate = InputHelper::getInputDate("Enter import date");
        try
        {
            temp.setImportDate(importDate);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[SetTopBoxMenu] Invalid Import Date input during creation: " + string(e.what()));
        }
    }

    // 5. Technical Status
    TechnicalStatus status = inputTechnicalStatus(false, TOT);
    temp.setTechnicalStatus(status);

    try
    {
        string newID = repoSetTopBox.generateNextID();
        SetTopBox finalStb(newID, temp.getSerial(), temp.getManufacturer(), temp.getModel(), temp.getImportDate(), temp.getTechnicalStatus());

        if (repoSetTopBox.create(finalStb))
        {
            repoSetTopBox.writeToFile();
            LOG("[SetTopBoxMenu] SetTopBox created successfully, ID: " + newID);
            cout << "\n[Success] Set-top box created successfully! Assigned ID: " << newID << "\n";
        }
        else
        {
            LOG("[SetTopBoxMenu] Failed to create SetTopBox, duplicate ID: " + newID);
            cout << "\n[Error] Failed to create Set-top box (duplicate ID)!\n";
        }
    }
    catch (const exception &e)
    {
        LOG("[SetTopBoxMenu] Exception caught during SetTopBox creation: " + string(e.what()));
        cout << "\n[Exception] " << e.what() << "\n";
    }

    InputHelper::getInputOptionalString("\nPress Enter to return...");
}

void SetTopBoxMenu::readSetTopBox()
{
    SystemHelper::clearScreen();
    cout << "=========================================\n";
    cout << "           SET-TOP BOX LIST\n";
    cout << "=========================================\n";

    if (repoSetTopBox.readAll().empty())
    {
        cout << "[Notice] Set-top box inventory is currently empty!\n";
    }
    else
    {
        TablePrinter::printTable(tableWidths, tableHeaders, repoSetTopBox.exportToVector());
        LOG("[SetTopBoxMenu] SetTopBox table printed successfully");
    }

    InputHelper::getInputOptionalString("\nPress Enter to return...");
}

void SetTopBoxMenu::searchSetTopBox()
{
    while (true)
    {
        SystemHelper::clearScreen();
        cout << "=========================================\n";
        cout << "          SEARCH SET-TOP BOXES\n";
        cout << "=========================================\n";
        cout << "1. Exact search by Set-Top Box ID\n";
        cout << "2. Search by Serial number\n";
        cout << "3. Search by Manufacturer or Model\n";
        cout << "4. Filter by Technical Status\n";
        cout << "0. Go back\n";

        int opt = InputHelper::getInputInt("Select an option (0-4): ");
        if (opt == 0) break;

        if (opt == 1)
        {
            string id = convertToUpper(InputHelper::getInputString("Enter STB ID to search (e.g. STB001): "));
            LOG("[SetTopBoxMenu] Search STB by ID: " + id);
            SetTopBox *stb = repoSetTopBox.readByID(id);
            SystemHelper::clearScreen();
            cout << "===== Search Results for ID: " << id << " =====\n";
            if (stb != nullptr)
            {
                TablePrinter::printSingle(tableWidths, tableHeaders, stb->exportToVector());
            }
            else
            {
                cout << "[Notice] No Set-Top Box found with ID: " << id << "\n";
            }
            InputHelper::getInputOptionalString("\nPress Enter to continue...");
        }
        else if (opt == 2)
        {
            string serialQuery = convertToUpper(InputHelper::getInputString("Enter Serial number to search: "));
            LOG("[SetTopBoxMenu] Search STB by serial: " + serialQuery);
            vector<vector<string>> matched;
            for (const auto &item : repoSetTopBox.readAll())
            {
                if (convertToUpper(item.getSerial()).find(serialQuery) != string::npos)
                {
                    matched.push_back(item.exportToVector());
                }
            }

            SystemHelper::clearScreen();
            cout << "===== Search Results for Serial: \"" << serialQuery << "\" =====\n";
            if (!matched.empty())
            {
                TablePrinter::printTable(tableWidths, tableHeaders, matched);
            }
            else
            {
                cout << "[Notice] No devices matched that Serial number!\n";
            }
            InputHelper::getInputOptionalString("\nPress Enter to continue...");
        }
        else if (opt == 3)
        {
            string kw = convertToUpper(InputHelper::getInputString("Enter Manufacturer or Model keyword: "));
            LOG("[SetTopBoxMenu] Search STB by manufacturer/model: " + kw);
            vector<vector<string>> matched;
            for (const auto &item : repoSetTopBox.readAll())
            {
                if (convertToUpper(item.getManufacturer()).find(kw) != string::npos ||
                    convertToUpper(item.getModel()).find(kw) != string::npos)
                {
                    matched.push_back(item.exportToVector());
                }
            }

            SystemHelper::clearScreen();
            cout << "===== Search Results for Keyword: \"" << kw << "\" =====\n";
            if (!matched.empty())
            {
                TablePrinter::printTable(tableWidths, tableHeaders, matched);
            }
            else
            {
                cout << "[Notice] No devices matched that keyword!\n";
            }
            InputHelper::getInputOptionalString("\nPress Enter to continue...");
        }
        else if (opt == 4)
        {
            TechnicalStatus st = inputTechnicalStatus(false);
            LOG("[SetTopBoxMenu] Filter STB by status");
            vector<vector<string>> matched;
            for (const auto &item : repoSetTopBox.readAll())
            {
                if (item.getTechnicalStatus() == st)
                {
                    matched.push_back(item.exportToVector());
                }
            }

            SystemHelper::clearScreen();
            cout << "===== Devices Filtered by Technical Status =====\n";
            if (!matched.empty())
            {
                TablePrinter::printTable(tableWidths, tableHeaders, matched);
            }
            else
            {
                cout << "[Notice] No devices found with that status!\n";
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

void SetTopBoxMenu::sortSetTopBox()
{
    auto getStatusPriority = [](TechnicalStatus st) -> int {
        switch (st)
        {
            case TOT: return 1;
            case BAO_TRI: return 2;
            case HONG: return 3;
            case NGUNG_SU_DUNG: return 4;
        }
        return 5;
    };

    while (true)
    {
        SystemHelper::clearScreen();
        cout << "=========================================\n";
        cout << "           SORT SET-TOP BOXES\n";
        cout << "=========================================\n";
        cout << "1. Sort by Serial (A - Z)\n";
        cout << "2. Sort by Serial (Z - A)\n";
        cout << "3. Sort by Manufacturer (A - Z)\n";
        cout << "4. Sort by Manufacturer (Z - A)\n";
        cout << "5. Sort by Import Date (Oldest -> Newest)\n";
        cout << "6. Sort by Import Date (Newest -> Oldest)\n";
        cout << "7. Sort by Technical Status (TOT -> BAO_TRI -> HONG -> NGUNG_SU_DUNG)\n";
        cout << "8. Sort by Technical Status (NGUNG_SU_DUNG -> HONG -> BAO_TRI -> TOT)\n";
        cout << "0. Go back\n";

        int opt = InputHelper::getInputInt("Select an option (0-8): ");
        if (opt == 0) break;

        vector<SetTopBox> list = repoSetTopBox.readAll();
        if (list.empty())
        {
            cout << "[Notice] Set-top box inventory is currently empty!\n";
            InputHelper::getInputOptionalString("Press Enter to continue...");
            break;
        }

        string title = "";

        if (opt == 1)
        {
            sort(list.begin(), list.end(), [](const SetTopBox &a, const SetTopBox &b) {
                if (a.getSerial() != b.getSerial())
                    return a.getSerial() < b.getSerial();
                return a.getID() < b.getID();
            });
            LOG("[SetTopBoxMenu] STBs sorted by serial (A - Z)");
            title = "Sorted by Serial (A - Z)";
        }
        else if (opt == 2)
        {
            sort(list.begin(), list.end(), [](const SetTopBox &a, const SetTopBox &b) {
                if (a.getSerial() != b.getSerial())
                    return a.getSerial() > b.getSerial();
                return a.getID() < b.getID();
            });
            LOG("[SetTopBoxMenu] STBs sorted by serial (Z - A)");
            title = "Sorted by Serial (Z - A)";
        }
        else if (opt == 3)
        {
            sort(list.begin(), list.end(), [](const SetTopBox &a, const SetTopBox &b) {
                if (a.getManufacturer() != b.getManufacturer())
                    return a.getManufacturer() < b.getManufacturer();
                if (a.getSerial() != b.getSerial())
                    return a.getSerial() < b.getSerial();
                return a.getID() < b.getID();
            });
            LOG("[SetTopBoxMenu] STBs sorted by manufacturer (A - Z)");
            title = "Sorted by Manufacturer (A - Z)";
        }
        else if (opt == 4)
        {
            sort(list.begin(), list.end(), [](const SetTopBox &a, const SetTopBox &b) {
                if (a.getManufacturer() != b.getManufacturer())
                    return a.getManufacturer() > b.getManufacturer();
                if (a.getSerial() != b.getSerial())
                    return a.getSerial() < b.getSerial();
                return a.getID() < b.getID();
            });
            LOG("[SetTopBoxMenu] STBs sorted by manufacturer (Z - A)");
            title = "Sorted by Manufacturer (Z - A)";
        }
        else if (opt == 5)
        {
            sort(list.begin(), list.end());
            LOG("[SetTopBoxMenu] STBs sorted by import date (Oldest -> Newest)");
            title = "Sorted by Import Date (Oldest -> Newest)";
        }
        else if (opt == 6)
        {
            sort(list.begin(), list.end(), greater<SetTopBox>());
            LOG("[SetTopBoxMenu] STBs sorted by import date (Newest -> Oldest)");
            title = "Sorted by Import Date (Newest -> Oldest)";
        }
        else if (opt == 7)
        {
            sort(list.begin(), list.end(), [&](const SetTopBox &a, const SetTopBox &b) {
                int pa = getStatusPriority(a.getTechnicalStatus());
                int pb = getStatusPriority(b.getTechnicalStatus());
                if (pa != pb) return pa < pb;
                return a.getID() < b.getID();
            });
            LOG("[SetTopBoxMenu] STBs sorted by technical status (Operational order)");
            title = "Sorted by Technical Status (TOT -> BAO_TRI -> HONG -> NGUNG_SU_DUNG)";
        }
        else if (opt == 8)
        {
            sort(list.begin(), list.end(), [&](const SetTopBox &a, const SetTopBox &b) {
                int pa = getStatusPriority(a.getTechnicalStatus());
                int pb = getStatusPriority(b.getTechnicalStatus());
                if (pa != pb) return pa > pb;
                return a.getID() < b.getID();
            });
            LOG("[SetTopBoxMenu] STBs sorted by technical status (Reverse operational order)");
            title = "Sorted by Technical Status (NGUNG_SU_DUNG -> HONG -> BAO_TRI -> TOT)";
        }
        else
        {
            cout << "[Error] Invalid option! Please try again.\n";
            InputHelper::getInputOptionalString("Press Enter to continue...");
            continue;
        }

        vector<vector<string>> data;
        for (const auto &item : list)
            data.push_back(item.exportToVector());

        SystemHelper::clearScreen();
        cout << "===== " << title << " =====\n";
        TablePrinter::printTable(tableWidths, tableHeaders, data);
        InputHelper::getInputOptionalString("\nPress Enter to continue...");
    }
}

void SetTopBoxMenu::updateSetTopBox()
{
    SystemHelper::clearScreen();
    cout << "=========================================\n";
    cout << "         UPDATE SET-TOP BOX INFO\n";
    cout << "=========================================\n";
    TablePrinter::printTable(tableWidths, tableHeaders, repoSetTopBox.exportToVector());

    string inputID = convertToUpper(InputHelper::getInputString("Enter STB ID to update: "));
    SetTopBox *stb = repoSetTopBox.readByID(inputID);

    if (stb == nullptr)
    {
        cout << "\n[Error] No Set-top box found with ID: " << inputID << "\n";
        InputHelper::getInputOptionalString("Press Enter to return...");
        return;
    }

    // Refresh contract & ticket lists to ensure up-to-date referential integrity
    repoContract.loadFromFile();
    repoSupportTicket.loadFromFile();

    const Contract *activeContract = nullptr;
    for (const auto &c : repoContract.readAll())
    {
        if (inputID == c.get_stbID() &&
            (c.get_status() == ContractStatus::ACTIVE || c.get_status() == ContractStatus::SUSPENDED))
        {
            activeContract = &c;
            break;
        }
    }

    SystemHelper::clearScreen();
    cout << "===== Current Set-Top Box Information =====\n";
    TablePrinter::printSingle(tableWidths, tableHeaders, stb->exportToVector());

    string newSerial;
    string newManufacturer;
    string newModel;
    Date newDate;

    if (activeContract != nullptr)
    {
        cout << "\n=========================================================\n";
        cout << "              CONTRACT INTEGRITY ENFORCED\n";
        cout << "=========================================================\n";
        cout << "[Notice] This Set-top box is currently allocated to:\n";
        cout << "         Contract ID    : " << activeContract->getID() << "\n";
        cout << "         Contract Status: " << ContractStatus_toString(activeContract->get_status()) << "\n";
        cout << "         Customer ID    : " << activeContract->get_customerID() << "\n";
        cout << "---------------------------------------------------------\n";
        cout << "Hardware specifications (Serial, Manufacturer, Model, Import Date)\n";
        cout << "are legally LOCKED to maintain contractual & audit integrity.\n";
        cout << "To replace this device, please perform a Device Swap in Contract Menu.\n";
        cout << "---------------------------------------------------------\n";
        cout << "Only the Technical Status can be updated.\n";
        cout << "=========================================================\n\n";
        // InputHelper::getInputOptionalString("Press Enter to return...");
    }
    else
    {
        cout << "\n(Leave field empty to keep current value)\n";

        // 1. Update Serial
        while (true)
        {
            newSerial = InputHelper::getInputOptionalString("New Serial [" + stb->getSerial() + "]: ");
            if (newSerial.empty()) break; // Keep current value

            if (newSerial != stb->getSerial())
            {
                bool exists = false;
                for (const auto &item : repoSetTopBox.readAll())
                {
                    if (item.getSerial() == newSerial)
                    {
                        exists = true;
                        break;
                    }
                }
                if (exists)
                {
                    cout << "[Error] Serial already exists on another device! Please try again.\n";
                    continue;
                }
            }

            try
            {
                SetTopBox testStb = *stb;
                testStb.setSerial(newSerial);
                break;
            }
            catch (const exception &e)
            {
                cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
                LOG("[SetTopBoxMenu] Invalid Serial input during update: " + string(e.what()));
            }
        }

        // 2. Update Manufacturer
        while (true)
        {
            newManufacturer = InputHelper::getInputOptionalString("New Manufacturer [" + stb->getManufacturer() + "]: ");
            if (newManufacturer.empty()) break; // Keep current value

            try
            {
                SetTopBox testStb = *stb;
                testStb.setManufacturer(newManufacturer);
                break;
            }
            catch (const exception &e)
            {
                cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
                LOG("[SetTopBoxMenu] Invalid Manufacturer input during update: " + string(e.what()));
            }
        }

        // 3. Update Model
        while (true)
        {
            newModel = InputHelper::getInputOptionalString("New Model [" + stb->getModel() + "]: ");
            if (newModel.empty()) break; // Keep current value

            try
            {
                SetTopBox testStb = *stb;
                testStb.setModel(newModel);
                break;
            }
            catch (const exception &e)
            {
                cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
                LOG("[SetTopBoxMenu] Invalid Model input during update: " + string(e.what()));
            }
        }

        // 4. Update Import Date
        while (true)
        {
            newDate = InputHelper::getInputOptionalDate("New Import Date [" + stb->getImportDate().toString() + "]");
            if (newDate.isEmpty()) break; // Keep current value

            try
            {
                SetTopBox testStb = *stb;
                testStb.setImportDate(newDate);
                break;
            }
            catch (const exception &e)
            {
                cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
                LOG("[SetTopBoxMenu] Invalid Import Date input during update: " + string(e.what()));
            }
        }
    }

    // 5. Update Technical Status
    TechnicalStatus newStatus;
    while (true)
    {
        cout << "Current status: " << stb->getTechnicalStatusString() << endl;
        newStatus = inputTechnicalStatus(true, stb->getTechnicalStatus());

        if (activeContract != nullptr && newStatus == TechnicalStatus::NGUNG_SU_DUNG)
        {
            cout << "\n[Constraint Error] Cannot set status to NGUNG_SU_DUNG (Decommissioned)!\n";
            cout << "This device is actively assigned to Contract [" << activeContract->getID() << "].\n";
            cout << "Please swap the device or terminate the contract before decommissioning.\n\n";
            continue;
        }
        break;
    }

    try
    {
        if (activeContract == nullptr)
        {
            if (!newSerial.empty()) stb->setSerial(newSerial);
            if (!newManufacturer.empty()) stb->setManufacturer(newManufacturer);
            if (!newModel.empty()) stb->setModel(newModel);
            if (!newDate.isEmpty()) stb->setImportDate(newDate);
        }
        stb->setTechnicalStatus(newStatus);

        if (repoSetTopBox.update(*stb))
        {
            repoSetTopBox.writeToFile();
            LOG("[SetTopBoxMenu] SetTopBox " + inputID + " updated successfully");
            cout << "\n[Success] Set-top box updated successfully!\n";

            if (activeContract != nullptr && (newStatus == TechnicalStatus::HONG || newStatus == TechnicalStatus::BAO_TRI))
            {
                cout << "\n[Advisory] Device marked as " << stb->getTechnicalStatusString() 
                     << " while allocated to active Contract [" << activeContract->getID() << "].\n";
                cout << "Recommended next step: Create a Support Ticket or swap the STB in Contract Menu.\n";
            }
        }
        else
        {
            LOG("[SetTopBoxMenu] Failed to update SetTopBox: " + inputID);
            cout << "\n[Error] Failed to update device record!\n";
        }
    }
    catch (const exception &e)
    {
        LOG("[SetTopBoxMenu] Validation exception during update: " + string(e.what()));
        cout << "\n[Validation Error] " << e.what() << "\n";
        cout << "Changes discarded due to invalid input.\n";
        repoSetTopBox.loadFromFile();
    }

    InputHelper::getInputOptionalString("\nPress Enter to continue...");
}

void SetTopBoxMenu::deleteSetTopBox()
{
    SystemHelper::clearScreen();
    cout << "=========================================\n";
    cout << "            DELETE SET-TOP BOX\n";
    cout << "=========================================\n";
    TablePrinter::printTable(tableWidths, tableHeaders, repoSetTopBox.exportToVector());

    string targetID = convertToUpper(InputHelper::getInputString("Enter STB ID to delete: "));
    SetTopBox *stb = repoSetTopBox.readByID(targetID);

    if (stb == nullptr)
    {
        cout << "\n[Error] No Set-top box found with ID: " << targetID << "\n";
        InputHelper::getInputOptionalString("Press Enter to return...");
        return;
    }

    SystemHelper::clearScreen();
    cout << "You are about to delete the following Set-top box:\n";
    TablePrinter::printSingle(tableWidths, tableHeaders, stb->exportToVector());

    // =====================================================================
    // REFERENTIAL INTEGRITY & DELETE CONSTRAINTS
    // =====================================================================
    // 1. Check if device is bound to an active or suspended contract
    const vector<Contract> &contractList = repoContract.readAll();
    for (const auto &c : contractList)
    {
        if (targetID == c.get_stbID() &&
            (c.get_status() == ContractStatus::ACTIVE || c.get_status() == ContractStatus::SUSPENDED))
        {
            LOG("[SetTopBoxMenu] SetTopBox deletion rejected: Active/suspended Contract [" + c.getID() + "]");
            cout << "\n[Deletion Rejected] This device is currently allocated to Contract [" << c.getID() 
                 << "] (Status: " << ContractStatus_toString(c.get_status()) << ")!\n";
            cout << "Please return the device to inventory or terminate the contract before deleting.\n";
            InputHelper::getInputOptionalString("\nPress Enter to return...");
            return;
        }
    }

    // 2. Check if device has an open support ticket
    const vector<SupportTicket> &ticketList = repoSupportTicket.readAll();
    for (const auto &ticket : ticketList)
    {
        if (targetID == ticket.getStbID() && ticket.getStatus() != TicketStatus::DA_DONG)
        {
            LOG("[SetTopBoxMenu] SetTopBox deletion rejected: Open Support Ticket [" + ticket.getID() + "]");
            cout << "\n[Deletion Rejected] This device has an open Support Ticket [" << ticket.getID() 
                 << "] (Status: Under Investigation / Unresolved)!\n";
            cout << "Please resolve and close the support ticket before deleting the device.\n";
            InputHelper::getInputOptionalString("\nPress Enter to return...");
            return;
        }
    }

    // =====================================================================
    string confirm = InputHelper::getInputString("\nAre you sure you want to delete this Set-top box? (y/n): ");
    if (confirm == "y" || confirm == "Y")
    {
        if (repoSetTopBox.remove(targetID))
        {
            repoSetTopBox.writeToFile();
            LOG("[SetTopBoxMenu] SetTopBox deleted successfully, ID: " + targetID);
            cout << "\n[Success] Set-top box deleted successfully!\n";
        }
        else
        {
            LOG("[SetTopBoxMenu] Failed to remove SetTopBox, ID: " + targetID);
            cout << "\n[Error] Failed to delete Set-top box!\n";
        }
    }
    else
    {
        cout << "\n[Cancelled] Set-top box deletion cancelled.\n";
        LOG("[SetTopBoxMenu] Delete operation cancelled by user");
    }

    InputHelper::getInputOptionalString("\nPress Enter to return...");
}

void SetTopBoxMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "=========================================\n";
    cout << "          SET-TOP BOX MANAGEMENT\n";
    cout << "=========================================\n";
    cout << " 1. Add new Set-top box to inventory\n";
    cout << " 2. View all Set-top boxes\n";
    cout << " 3. Search Set-top boxes\n";
    cout << " 4. Sort Set-top box list\n";
    cout << " 5. Update Set-top box information\n";
    cout << " 6. Delete Set-top box from inventory\n";
    cout << "-----------------------------------------\n";
    cout << " 0. Return to main menu\n";
    cout << "=========================================\n";
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
        searchSetTopBox();
        break;
    case 4:
        sortSetTopBox();
        break;
    case 5:
        updateSetTopBox();
        break;
    case 6:
        deleteSetTopBox();
        break;
    default:
        cout << "[Error] Invalid choice! Please select an option from 0 to 6.\n";
        InputHelper::getInputOptionalString("Press Enter to continue...");
        break;
    }
}
