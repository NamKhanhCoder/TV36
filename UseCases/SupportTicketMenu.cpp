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

    string contractID = "";
    Contract *matchedContract = nullptr;
    while (true)
    {
        contractID = convertToUpper(InputHelper::getInputString("Contract ID: "));
        if (!Validator::isValidID(contractID, "HD", static_cast<short>(contractID.length())))
        {
            cout << "[Error] Invalid Contract ID format! (Must start with 'HD' followed by digits)\n";
            continue;
        }

        matchedContract = repoContract.readByID(contractID);
        if (matchedContract == nullptr)
        {
            cout << "[Error] Contract ID '" << contractID << "' does not exist in repository! Please try again.\n";
            continue;
        }
        break;
    }

    // Set-Top Box ID (From contract, optional)
    string stbID = "";
    while (true)
    {
        string prompt = "Set-Top Box ID (leave empty if not device issue, contract STB is " + matchedContract->get_stbID() + "): ";
        stbID = convertToUpper(InputHelper::getInputOptionalString(prompt));
        if (stbID.empty())
        {
            break;
        }

        if (stbID != matchedContract->get_stbID())
        {
            cout << "[Error] Set-Top Box ID must match the STB assigned to this Contract (" 
                 << matchedContract->get_stbID() << ") or be left empty!\n";
            continue;
        }
        break;
    }
    
    // Received Date: must be today or past
    string currentDT = getCurrentDateTimeString();
    cout << "Received Date: " << currentDT << " (Auto-generated current timestamp, verified as today/past)" << endl;
    string receivedDate = currentDT;
    
    string content = InputHelper::getInputString("Issue Content / Description: ");
    TicketPriority priority = inputTicketPriority(false);
    TicketStatus status = TicketStatus::TIEP_NHAN;
    string resolution = "";
    string closedDate = "";

    SupportTicket temp(repo.generateNextID(), contractID, stbID, receivedDate, content, priority, status, resolution, closedDate);
    if (repo.create(temp))
    {
        repo.writeToFile();
        LOG("[SupportTicketMenu] Ticket created successfully, ID: " + temp.getID());
        cout << "Support Ticket created successfully! ID: " << temp.getID() << endl;
    }
    else
    {
        cout << "[Error] Support Ticket creation failed!\n";
    }

    InputHelper::getInputOptionalString("Press Enter to continue...");
}

void SupportTicketMenu::readTicket()
{
    SystemHelper::clearScreen();
    cout << "===== View Support Tickets =====" << endl;
    cout << "1. View all Support Tickets" << endl;
    cout << "2. Search Support Ticket by ID" << endl;

    vector<int> widths = {8, 10, 10, 17, 25, 13, 13, 25, 17};
    vector<string> headers = {"ID", "Contract", "STB ID", "Received Date", "Content", "Priority", "Status", "Resolution", "Closed Date"};

    int choice = InputHelper::getInputInt("Select an option (0 to go back): ");
    if (choice == 1)
    {
        SystemHelper::clearScreen();
        cout << "===== All Support Tickets =====\n";
        TablePrinter::printTable(widths, headers, repo.exportToVector());
        LOG("[SupportTicketMenu] Support Ticket table printed successfully");
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
    }

    InputHelper::getInputOptionalString("Press Enter to continue...");
}

void SupportTicketMenu::updateTicket()
{
    SystemHelper::clearScreen();
    cout << "===== Update Support Ticket =====" << endl;
    vector<int> widths = {8, 10, 10, 17, 25, 13, 13, 25, 17};
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

    SystemHelper::clearScreen();
    cout << "===== Current Ticket Information =====" << endl;
    TablePrinter::printSingle(widths, headers, ticket->exportToVector());

    cout << "\nLeave field empty (Press Enter) to keep current value.\n";

    string newContent = InputHelper::getInputOptionalString("New Content [" + ticket->getContent() + "]: ");
    if (!newContent.empty())
        ticket->setContent(newContent);

    TicketPriority newPriority = inputTicketPriority(true, ticket->getPriority());
    ticket->setPriority(newPriority);

    TicketStatus newStatus = inputTicketStatus(true, ticket->getStatus());
    ticket->setStatus(newStatus);

    if (newStatus == TicketStatus::DA_DONG)
    {
        string currentResolution = ticket->getResolution();
        string resPrompt = "Resolution result (Press Enter to keep current: " + (currentResolution.empty() ? "None" : currentResolution) + "): ";
        string newResolution = InputHelper::getInputOptionalString(resPrompt);
        if (!newResolution.empty())
        {
            ticket->setResolution(newResolution);
        }
        else if (ticket->getResolution().empty())
        {
            // Neu truoc do chua co resolution thi yeu cau nhap
            while (ticket->getResolution().empty())
            {
                cout << "[Notice] Ticket status is DA_DONG, resolution is required!\n";
                string res = InputHelper::getInputString("Enter Resolution result: ");
                ticket->setResolution(res);
            }
        }

        if (ticket->getClosedDate().empty())
        {
            ticket->setClosedDate(getCurrentDateTimeString());
            cout << "Closed Date set to: " << ticket->getClosedDate() << endl;
        }
    }

    if (repo.update(*ticket))
    {
        repo.writeToFile();
        LOG("[SupportTicketMenu] Ticket updated successfully, ID: " + ticket->getID());
        cout << "Support Ticket updated successfully!\n";
    }
    else
    {
        cout << "[Error] Update failed!\n";
    }

    InputHelper::getInputOptionalString("Press Enter to continue...");
}

void SupportTicketMenu::deleteTicket()
{
    SystemHelper::clearScreen();
    cout << "===== Delete Support Ticket =====" << endl;
    vector<int> widths = {8, 10, 10, 17, 25, 13, 13, 25, 17};
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

    // Check Removal dependency: Must be resolved (Status == DA_DONG)
    if (ticket->getStatus() != TicketStatus::DA_DONG)
    {
        cout << "\n[Error] Cannot delete Ticket " << targetID << " because it has not been resolved yet (Current Status: " 
             << ticket->getStatusString() << ")!\n";
        cout << "[Rule] Only resolved tickets (DA_DONG) can be deleted/removed.\n";
        InputHelper::getInputOptionalString("Press Enter to continue...");
        return;
    }

    string confirm = InputHelper::getInputString("Are you sure you want to delete this Ticket? (y/n): ");
    if (confirm == "y" || confirm == "Y")
    {
        if (repo.remove(targetID))
        {
            repo.writeToFile();
            LOG("[SupportTicketMenu] Ticket deleted successfully, ID: " + targetID);
            cout << "Support Ticket deleted successfully!\n";
        }
        else
        {
            cout << "[Error] Failed to delete Ticket!\n";
        }
    }
    else
    {
        cout << "Deletion cancelled.\n";
        LOG("[SupportTicketMenu] Ticket deletion cancelled by user");
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
