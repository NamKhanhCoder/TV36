// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Duong Bao Long (B24DCVT231)
// Use Case 2: Quan ly Phieu ho tro ki thuat Support Ticket (Menu dieu khien)
// ===============================================================
#include "UseCases/SupportTicketMenu.h"
#include "Utils/Debugger.h"
#include "Utils/TablePrinter.h"
#include "Utils/SystemHelper.h"
#include "Utils/InputHelper.h"
#include "Utils/StringHelper.h"
#include "Utils/Validator.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace std;

static string getCurrentDateTimeString()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    stringstream ss;
    ss << setfill('0') << setw(4) << (1900 + ltm->tm_year) << "-"
       << setw(2) << (1 + ltm->tm_mon) << "-"
       << setw(2) << ltm->tm_mday << " "
       << setw(2) << ltm->tm_hour << ":"
       << setw(2) << ltm->tm_min;
    return ss.str();
}

SupportTicketMenu::SupportTicketMenu() 
    : repo("Data/support_tickets.txt", "Data/support_tickets_counter.txt", "PHT"),
      repoContract("Data/contracts.txt", "Data/contracts_counter.txt", "HD")
{
    repo.loadFromFile();
    repoContract.loadFromFile();
}

TicketPriority SupportTicketMenu::inputTicketPriority(bool isOptional, TicketPriority current)
{
    while (true)
    {
        cout << "\nSelect Priority:" << endl;
        cout << "1. BINH_THUONG" << endl;
        cout << "2. UU_TIEN" << endl;
        cout << "3. KHAN_CAP" << endl;
        if (isOptional)
        {
            cout << "(Press Enter to skip, keep current: " << ticketPriorityToString(current) << ")" << endl;
        }

        string input = InputHelper::getInputOptionalString("Choice: ");
        if (isOptional && input.empty())
        {
            return current;
        }
        if (input == "1") return TicketPriority::BINH_THUONG;
        if (input == "2") return TicketPriority::UU_TIEN;
        if (input == "3") return TicketPriority::KHAN_CAP;

        cout << "Invalid choice! Please try again." << endl;
    }
}

TicketStatus SupportTicketMenu::inputTicketStatus(bool isOptional, TicketStatus current)
{
    while (true)
    {
        cout << "\nSelect Status:" << endl;
        cout << "1. TIEP_NHAN" << endl;
        cout << "2. DANG_XU_LY" << endl;
        cout << "3. DA_DONG" << endl;
        if (isOptional)
        {
            cout << "(Press Enter to skip, keep current: " << ticketStatusToString(current) << ")" << endl;
        }

        string input = InputHelper::getInputOptionalString("Choice: ");
        if (isOptional && input.empty())
        {
            return current;
        }
        if (input == "1") return TicketStatus::TIEP_NHAN;
        if (input == "2") return TicketStatus::DANG_XU_LY;
        if (input == "3") return TicketStatus::DA_DONG;

        cout << "Invalid choice! Please try again." << endl;
    }
}

void SupportTicketMenu::createTicket()
{
    SystemHelper::clearScreen();
    cout << "===== Create Support Ticket =====" << endl;

    repoContract.loadFromFile();

    SupportTicket temp;

    // 1. Contract ID with setter try-catch
    Contract *matchedContract = nullptr;
    while (true)
    {
        string contractID = convertToUpper(InputHelper::getInputString("Contract ID: "));
        if (contractID.length() < 5 || !Validator::isValidID(contractID, "HD", static_cast<short>(contractID.length())))
        {
            cout << "[Error] Invalid Contract ID format! (Must start with 'HD' followed by at least 3 digits)\n";
            continue;
        }

        matchedContract = repoContract.readByID(contractID);
        if (matchedContract == nullptr)
        {
            cout << "[Error] Contract ID '" << contractID << "' does not exist in repository! Please try again.\n";
            continue;
        }

        try
        {
            temp.setContractID(contractID);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[SupportTicketMenu] Invalid Contract ID input during creation: " + string(e.what()));
        }
    }

    // 2. Set-Top Box ID (From contract) with setter try-catch
    while (true)
    {
        string contractSTB = matchedContract->get_stbID();
        cout << "STB assigned in contract: " << contractSTB << endl;
        string prompt = "Set-Top Box ID (press Enter to use contract STB [" + contractSTB + "], or leave empty if not device issue): ";
        string stbID = convertToUpper(InputHelper::getInputOptionalString(prompt));
        if (stbID.empty())
        {
            stbID = contractSTB; // auto-fill from contract
        }

        if (!stbID.empty() && stbID != contractSTB)
        {
            cout << "[Error] STB ID must match the STB assigned to this Contract (" << contractSTB << ")!\n";
            continue;
        }

        try
        {
            temp.setStbID(stbID);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[SupportTicketMenu] Invalid STB ID input during creation: " + string(e.what()));
        }
    }
    
    // 3. Received Date: must be today or past
    string currentDT = getCurrentDateTimeString();
    cout << "Received Date: " << currentDT << " (Auto-generated current timestamp, verified as today/past)" << endl;
    try
    {
        temp.setReceivedDate(currentDT);
    }
    catch (const exception &e)
    {
        cout << "[Validation Error] " << e.what() << endl;
    }
    
    // 4. Content with setter try-catch
    while (true)
    {
        string content = InputHelper::getInputString("Issue Content / Description: ");
        try
        {
            temp.setContent(content);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[SupportTicketMenu] Invalid Content input during creation: " + string(e.what()));
        }
    }

    // 5. Priority with setter try-catch
    TicketPriority priority = inputTicketPriority(false);
    try
    {
        temp.setPriority(priority);
    }
    catch (const exception &e)
    {
        cout << "[Validation Error] " << e.what() << endl;
    }

    temp.setResolution("");
    temp.setClosedDate("");

    SupportTicket newTicket(repo.generateNextID(), temp.getContractID(), temp.getStbID(), temp.getReceivedDate(),
                            temp.getContent(), temp.getPriority(), TicketStatus::TIEP_NHAN, "", "");
    if (repo.create(newTicket))
    {
        try
        {
            repo.writeToFile();
            LOG("[SupportTicketMenu] Ticket created successfully, ID: " + newTicket.getID());
            cout << "Support Ticket created successfully! ID: " << newTicket.getID() << endl;
        }
        catch (const exception &e)
        {
            repo.loadFromFile(); // Rollback if save fails
            cout << "[File Error] Failed to save ticket to disk: " << e.what() << endl;
        }
    }
    else
    {
        cout << "[Error] Support Ticket creation failed!\n";
    }

    InputHelper::getInputOptionalString("Press Enter to continue...");
}

void SupportTicketMenu::readTicket()
{
    while (true)
    {
        SystemHelper::clearScreen();
        cout << "===== View Support Tickets =====" << endl;
        cout << "1. View all Support Tickets" << endl;
        cout << "2. Search Support Ticket by ID" << endl;
        cout << "3. Sort Support Tickets" << endl;
        cout << "0. Go back" << endl;

        vector<int> widths = {8, 10, 10, 17, 25, 13, 12, 25, 17};
        vector<string> headers = {"ID", "Contract", "STB ID", "Received Date", "Content", "Priority", "Status", "Resolution", "Closed Date"};

        int choice = InputHelper::getInputInt("Select an option (0-3): ");
        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            SystemHelper::clearScreen();
            cout << "===== All Support Tickets =====\n";
            TablePrinter::printTable(widths, headers, repo.exportToVector());
            LOG("[SupportTicketMenu] Support Ticket table printed successfully");
            InputHelper::getInputOptionalString("\nPress Enter to continue...");
        }
        else if (choice == 2)
        {
            string inputID = convertToUpper(InputHelper::getInputString("Enter Ticket ID to search: "));
            SupportTicket *ticket = repo.readByID(inputID);
            SystemHelper::clearScreen();
            if (ticket == nullptr)
            {
                cout << "[Error] Ticket not found, ID: " << inputID << endl;
            }
            else
            {
                cout << "===== Support Ticket Detail =====\n";
                TablePrinter::printSingle(widths, headers, ticket->exportToVector());
            }
            LOG("[SupportTicketMenu] Search ticket completed");
            InputHelper::getInputOptionalString("\nPress Enter to continue...");
        }
        else if (choice == 3)
        {
            sortTicket();
        }
        else
        {
            cout << "Invalid choice! Please try again.\n";
            InputHelper::getInputOptionalString("Press Enter to continue...");
        }
    }
}

static int getPriorityRank(TicketPriority p)
{
    switch (p)
    {
    case TicketPriority::BINH_THUONG: return 1;
    case TicketPriority::UU_TIEN: return 2;
    case TicketPriority::KHAN_CAP: return 3;
    default: return 0;
    }
}

static int getStatusRank(TicketStatus s)
{
    switch (s)
    {
    case TicketStatus::TIEP_NHAN: return 1;
    case TicketStatus::DANG_XU_LY: return 2;
    case TicketStatus::DA_DONG: return 3;
    default: return 0;
    }
}

static int extractContractNumber(const string &contractID)
{
    int num = 0;
    for (char c : contractID)
    {
        if (isdigit(static_cast<unsigned char>(c)))
        {
            num = num * 10 + (c - '0');
        }
    }
    return num;
}

void SupportTicketMenu::sortTicket()
{
    vector<int> widths = {8, 10, 10, 17, 25, 13, 12, 25, 17};
    vector<string> headers = {"ID", "Contract", "STB ID", "Received Date", "Content", "Priority", "Status", "Resolution", "Closed Date"};

    while (true)
    {
        SystemHelper::clearScreen();
        cout << "=========================================\n";
        cout << "           SORT SUPPORT TICKETS          \n";
        cout << "=========================================\n";
        cout << "1. Contract ID descending (High -> Low)\n";
        cout << "2. Contract ID ascending (Low -> High)\n";
        cout << "3. Priority: BINH_THUONG -> UU_TIEN -> KHAN_CAP\n";
        cout << "4. Priority: KHAN_CAP -> UU_TIEN -> BINH_THUONG\n";
        cout << "5. Status: TIEP_NHAN -> DANG_XU_LY -> DA_DONG\n";
        cout << "6. Status: DA_DONG -> DANG_XU_LY -> TIEP_NHAN\n";
        cout << "0. Go back\n";

        int opt = InputHelper::getInputInt("Select an option (0-6): ");
        if (opt == 0) break;

        vector<SupportTicket> list = repo.readAll();
        if (list.empty())
        {
            cout << "[Notice] Support ticket list is currently empty!\n";
            InputHelper::getInputOptionalString("Press Enter to continue...");
            break;
        }

        if (opt == 1)
        {
            sort(list.begin(), list.end(), [](const SupportTicket &a, const SupportTicket &b) {
                int numA = extractContractNumber(a.getContractID());
                int numB = extractContractNumber(b.getContractID());
                if (numA != numB) return numA > numB;
                return a.getID() < b.getID();
            });
            LOG("[SupportTicketMenu] Tickets sorted by Contract ID descending");
            vector<vector<string>> data;
            for (const auto &t : list) data.push_back(t.exportToVector());

            SystemHelper::clearScreen();
            cout << "===== Support Tickets Sorted by Contract ID (Descending) =====\n";
            TablePrinter::printTable(widths, headers, data);
            InputHelper::getInputOptionalString("\nPress Enter to continue...");
        }
        else if (opt == 2)
        {
            sort(list.begin(), list.end(), [](const SupportTicket &a, const SupportTicket &b) {
                int numA = extractContractNumber(a.getContractID());
                int numB = extractContractNumber(b.getContractID());
                if (numA != numB) return numA < numB;
                return a.getID() < b.getID();
            });
            LOG("[SupportTicketMenu] Tickets sorted by Contract ID ascending");
            vector<vector<string>> data;
            for (const auto &t : list) data.push_back(t.exportToVector());

            SystemHelper::clearScreen();
            cout << "===== Support Tickets Sorted by Contract ID (Ascending) =====\n";
            TablePrinter::printTable(widths, headers, data);
            InputHelper::getInputOptionalString("\nPress Enter to continue...");
        }
        else if (opt == 3)
        {
            sort(list.begin(), list.end(), [](const SupportTicket &a, const SupportTicket &b) {
                int rankA = getPriorityRank(a.getPriority());
                int rankB = getPriorityRank(b.getPriority());
                if (rankA != rankB) return rankA < rankB;
                return a.getID() < b.getID();
            });
            LOG("[SupportTicketMenu] Tickets sorted by Priority (BINH_THUONG -> UU_TIEN -> KHAN_CAP)");
            vector<vector<string>> data;
            for (const auto &t : list) data.push_back(t.exportToVector());

            SystemHelper::clearScreen();
            cout << "===== Support Tickets Sorted by Priority (BINH_THUONG -> UU_TIEN -> KHAN_CAP) =====\n";
            TablePrinter::printTable(widths, headers, data);
            InputHelper::getInputOptionalString("\nPress Enter to continue...");
        }
        else if (opt == 4)
        {
            sort(list.begin(), list.end(), [](const SupportTicket &a, const SupportTicket &b) {
                int rankA = getPriorityRank(a.getPriority());
                int rankB = getPriorityRank(b.getPriority());
                if (rankA != rankB) return rankA > rankB;
                return a.getID() < b.getID();
            });
            LOG("[SupportTicketMenu] Tickets sorted by Priority (KHAN_CAP -> UU_TIEN -> BINH_THUONG)");
            vector<vector<string>> data;
            for (const auto &t : list) data.push_back(t.exportToVector());

            SystemHelper::clearScreen();
            cout << "===== Support Tickets Sorted by Priority (KHAN_CAP -> UU_TIEN -> BINH_THUONG) =====\n";
            TablePrinter::printTable(widths, headers, data);
            InputHelper::getInputOptionalString("\nPress Enter to continue...");
        }
        else if (opt == 5)
        {
            sort(list.begin(), list.end(), [](const SupportTicket &a, const SupportTicket &b) {
                int rankA = getStatusRank(a.getStatus());
                int rankB = getStatusRank(b.getStatus());
                if (rankA != rankB) return rankA < rankB;
                return a.getID() < b.getID();
            });
            LOG("[SupportTicketMenu] Tickets sorted by Status (TIEP_NHAN -> DANG_XU_LY -> DA_DONG)");
            vector<vector<string>> data;
            for (const auto &t : list) data.push_back(t.exportToVector());

            SystemHelper::clearScreen();
            cout << "===== Support Tickets Sorted by Status (TIEP_NHAN -> DANG_XU_LY -> DA_DONG) =====\n";
            TablePrinter::printTable(widths, headers, data);
            InputHelper::getInputOptionalString("\nPress Enter to continue...");
        }
        else if (opt == 6)
        {
            sort(list.begin(), list.end(), [](const SupportTicket &a, const SupportTicket &b) {
                int rankA = getStatusRank(a.getStatus());
                int rankB = getStatusRank(b.getStatus());
                if (rankA != rankB) return rankA > rankB;
                return a.getID() < b.getID();
            });
            LOG("[SupportTicketMenu] Tickets sorted by Status (DA_DONG -> DANG_XU_LY -> TIEP_NHAN)");
            vector<vector<string>> data;
            for (const auto &t : list) data.push_back(t.exportToVector());

            SystemHelper::clearScreen();
            cout << "===== Support Tickets Sorted by Status (DA_DONG -> DANG_XU_LY -> TIEP_NHAN) =====\n";
            TablePrinter::printTable(widths, headers, data);
            InputHelper::getInputOptionalString("\nPress Enter to continue...");
        }
        else
        {
            cout << "[Error] Invalid option! Please try again.\n";
            InputHelper::getInputOptionalString("Press Enter to continue...");
        }
    }
}

void SupportTicketMenu::updateTicket()
{
    SystemHelper::clearScreen();
    cout << "===== Update Support Ticket =====" << endl;
    cout << "[Update Rules] Allowed fields: Content, Priority, Status, Solution.\n";
    cout << "[Note] Tickets with status 'DA_DONG' cannot be modified (locked for audit history).\n\n";

    vector<int> widths = {8, 10, 10, 17, 25, 13, 12, 25, 17};
    vector<string> headers = {"ID", "Contract", "STB ID", "Received Date", "Content", "Priority", "Status", "Resolution", "Closed Date"};
    TablePrinter::printTable(widths, headers, repo.exportToVector());

    string inputID = convertToUpper(InputHelper::getInputString("Enter Ticket ID to update: "));
    SupportTicket *ticket = repo.readByID(inputID);

    if (ticket == nullptr)
    {
        cout << "[Error] Ticket not found, ID: " << inputID << endl;
        InputHelper::getInputOptionalString("Press Enter to continue...");
        return;
    }

    // Rang buoc dac biet: Ticket da o trang thai DA_DONG thi khoa hoan toan, khong cho phep sua
    if (ticket->getStatus() == TicketStatus::DA_DONG)
    {
        SystemHelper::clearScreen();
        cout << "===== Ticket Locked =====" << endl;
        TablePrinter::printSingle(widths, headers, ticket->exportToVector());
        cout << "\n[Update Rejected] Support Ticket " << inputID << " is already closed (DA_DONG)!\n";
        cout << "[Rule] Closed tickets cannot be modified to preserve audit trail integrity.\n";
        InputHelper::getInputOptionalString("\nPress Enter to return...");
        return;
    }

    // F08: Luu ban sao backup de ho tro rollback neu qua trinh sua hoac ghi file bi loi
    SupportTicket backup = *ticket;

    SystemHelper::clearScreen();
    cout << "===== Current Ticket Information =====" << endl;
    TablePrinter::printSingle(widths, headers, ticket->exportToVector());

    cout << "\nLeave field empty (Press Enter) to keep current value.\n";

    // 1. Content with setter try-catch
    while (true)
    {
        string newContent = InputHelper::getInputOptionalString("New Content [" + ticket->getContent() + "]: ");
        if (newContent.empty())
            break;
        try
        {
            ticket->setContent(newContent);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
        }
    }

    // 2. Priority with setter try-catch
    TicketPriority newPriority = inputTicketPriority(true, ticket->getPriority());
    try
    {
        ticket->setPriority(newPriority);
    }
    catch (const exception &e)
    {
        cout << "[Validation Error] " << e.what() << endl;
    }

    // 3. Status with setter try-catch
    TicketStatus oldStatus = ticket->getStatus();
    TicketStatus newStatus = inputTicketStatus(true, oldStatus);
    ticket->setStatus(newStatus);

    // 4. Solution (Resolution) & Closed Date:
    // F05: Chi cap nhat thoi diem dong closedDate khi vua chuyen tu trang thai khac sang DA_DONG
    if (newStatus == TicketStatus::DA_DONG)
    {
        cout << "\n[Status: DA_DONG] Solution is mandatory when closing ticket.\n";
        while (true)
        {
            string currentRes = ticket->getResolution();
            string prompt = "Solution/Resolution [" + (currentRes.empty() ? "None" : currentRes) + "]: ";
            string newRes = InputHelper::getInputOptionalString(prompt);
            
            if (newRes.empty())
            {
                if (!currentRes.empty())
                {
                    break;
                }
                else
                {
                    cout << "[Error] Solution cannot be empty when status is DA_DONG! Please enter solution.\n";
                    continue;
                }
            }

            try
            {
                ticket->setResolution(newRes);
                break;
            }
            catch (const exception &e)
            {
                cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            }
        }

        // F05 fix: Chi tu dong gan ClosedDate neu truoc do chua dong
        if (oldStatus != TicketStatus::DA_DONG)
        {
            string closedDT = getCurrentDateTimeString();
            ticket->setClosedDate(closedDT);
            cout << "[Auto Closed Date] Closed Date automatically set to: " << closedDT << endl;
        }
    }
    else
    {
        cout << "\n[Note] Ticket status is " << ticketStatusToString(newStatus) 
             << ". Solution and Closed Date remain empty.\n";
    }

    try
    {
        if (repo.update(*ticket))
        {
            repo.writeToFile();
            LOG("[SupportTicketMenu] Ticket updated successfully, ID: " + ticket->getID());
            cout << "\nSupport Ticket updated successfully!\n";
        }
        else
        {
            *ticket = backup; // Rollback
            cout << "\n[Error] Update failed!\n";
        }
    }
    catch (const exception &e)
    {
        // F08: Rollback state in RAM on write failure
        *ticket = backup;
        repo.loadFromFile();
        LOG("[SupportTicketMenu] File error while updating ticket: " + string(e.what()));
        cout << "\n[File Error] " << e.what() << "\nChanges discarded, ticket state rolled back.\n";
    }

    InputHelper::getInputOptionalString("Press Enter to continue...");
}

void SupportTicketMenu::deleteTicket()
{
    SystemHelper::clearScreen();
    cout << "===== Delete Support Ticket =====" << endl;
    vector<int> widths = {8, 10, 10, 17, 25, 13, 12, 25, 17};
    vector<string> headers = {"ID", "Contract", "STB ID", "Received Date", "Content", "Priority", "Status", "Resolution", "Closed Date"};
    TablePrinter::printTable(widths, headers, repo.exportToVector());

    string targetID = convertToUpper(InputHelper::getInputString("Enter Ticket ID to delete: "));
    SupportTicket *ticket = repo.readByID(targetID);

    if (ticket == nullptr)
    {
        cout << "[Error] Ticket not found, ID: " << targetID << endl;
        InputHelper::getInputOptionalString("Press Enter to continue...");
        return;
    }

    SystemHelper::clearScreen();
    cout << "You are about to delete Support Ticket:" << endl;
    TablePrinter::printSingle(widths, headers, ticket->exportToVector());

    // Rang buoc xoa: Chi duoc phep xoa khi dang o trang thai "TIEP_NHAN"
    if (ticket->getStatus() != TicketStatus::TIEP_NHAN)
    {
        cout << "\n[Deletion Rejected] Cannot delete Support Ticket " << targetID << "!\n";
        cout << "[Rule] Only tickets with status 'TIEP_NHAN' can be deleted.\n";
        cout << "       Current status is '" << ticket->getStatusString() << "' (must be kept for audit history).\n";
        InputHelper::getInputOptionalString("Press Enter to continue...");
        return;
    }

    string confirm = InputHelper::getInputString("\nAre you sure you want to delete this ticket? (y/n): ");
    if (confirm == "y" || confirm == "Y")
    {
        if (repo.remove(targetID))
        {
            try
            {
                repo.writeToFile();
                LOG("[SupportTicketMenu] Ticket deleted successfully, ID: " + targetID);
                cout << "\nSupport Ticket " << targetID << " deleted successfully!\n";
            }
            catch (const exception &e)
            {
                repo.loadFromFile(); // Rollback
                cout << "\n[File Error] Failed to update file after deletion: " << e.what() << endl;
            }
        }
        else
        {
            cout << "\n[Error] Failed to delete ticket " << targetID << "!\n";
        }
    }
    else
    {
        cout << "\nDeletion cancelled.\n";
    }

    InputHelper::getInputOptionalString("Press Enter to continue...");
}

void SupportTicketMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "===== Support Ticket Management =====" << endl;
    cout << "1. Add Support Ticket" << endl;
    cout << "2. View Support Tickets" << endl;
    cout << "3. Update Support Ticket" << endl;
    cout << "4. Delete Support Ticket" << endl;
}

void SupportTicketMenu::handleChoice(int choice)
{
    switch (choice)
    {
    case 1:
        createTicket();
        break;
    case 2:
        readTicket();
        break;
    case 3:
        updateTicket();
        break;
    case 4:
        deleteTicket();
        break;
    default:
        cout << "Invalid choice! Press Enter to try again." << endl;
        InputHelper::getInputOptionalString("");
        break;
    }
}
