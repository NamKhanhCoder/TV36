#include "UseCases/SupportTicketMenu.h"
#include "Utils/Debugger.h"
#include "Utils/TablePrinter.h"
#include "Utils/SystemHelper.h"
#include "Utils/InputHelper.h"
#include "Utils/StringHelper.h"
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

SupportTicketMenu::SupportTicketMenu() : repo("Data/support_tickets.txt", "Data/support_tickets_counter.txt", "PHT")
{
    repo.loadFromFile();
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
            cout << "0. Skip (Keep current: " << ticketPriorityToString(current) << ")" << endl;
        }

        int choice = InputHelper::getInputInt("Choice: ");
        if (isOptional && choice == 0)
        {
            return current;
        }
        if (choice == 1) return TicketPriority::BINH_THUONG;
        if (choice == 2) return TicketPriority::UU_TIEN;
        if (choice == 3) return TicketPriority::KHAN_CAP;

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
            cout << "0. Skip (Keep current: " << ticketStatusToString(current) << ")" << endl;
        }

        int choice = InputHelper::getInputInt("Choice: ");
        if (isOptional && choice == 0)
        {
            return current;
        }
        if (choice == 1) return TicketStatus::TIEP_NHAN;
        if (choice == 2) return TicketStatus::DANG_XU_LY;
        if (choice == 3) return TicketStatus::DA_DONG;

        cout << "Invalid choice! Please try again." << endl;
    }
}

void SupportTicketMenu::createTicket()
{
    SystemHelper::clearScreen();
    cout << "===== Create Support Ticket =====" << endl;

    string contractID = convertToUpper(InputHelper::getInputString("Contract ID: "));
    string stbID = convertToUpper(InputHelper::getInputOptionalString("Set-Top Box ID (leave empty if not device issue): "));
    
    string receivedDate = getCurrentDateTimeString();
    cout << "Received Date: " << receivedDate << " (Auto-generated current time)" << endl;
    
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

    cout << "\nLeave field empty to keep current value.\n";

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
        string resPrompt = "Resolution result" + (currentResolution.empty() ? "" : " [" + currentResolution + "]") + ": ";
        string newResolution = InputHelper::getInputString(resPrompt);
        if (!newResolution.empty())
            ticket->setResolution(newResolution);

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

    if (ticket->getStatus() != TicketStatus::TIEP_NHAN)
    {
        cout << "[Notice] According to rules, tickets in status DANG_XU_LY or DA_DONG should be kept for audit history.\n";
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

