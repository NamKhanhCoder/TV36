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
                            temp.getContent(), temp.getPriority(), temp.getResolution(), temp.getClosedDate());
    if (repo.create(newTicket))
    {
        repo.writeToFile();
        LOG("[SupportTicketMenu] Ticket created successfully, ID: " + newTicket.getID());
        cout << "Support Ticket created successfully! ID: " << newTicket.getID() << endl;
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

    vector<int> widths = {8, 10, 10, 17, 25, 13, 25, 17};
    vector<string> headers = {"ID", "Contract", "STB ID", "Received Date", "Content", "Priority", "Resolution", "Closed Date"};

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
    cout << "[Dependency Rule] Only allow: Content, Priority, Solution (Resolution), Close Date\n\n";

    vector<int> widths = {8, 10, 10, 17, 25, 13, 25, 17};
    vector<string> headers = {"ID", "Contract", "STB ID", "Received Date", "Content", "Priority", "Resolution", "Closed Date"};
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

    // 3. Solution (Resolution) with setter try-catch
    while (true)
    {
        string currentRes = ticket->getResolution();
        string newRes = InputHelper::getInputOptionalString("New Solution/Resolution [" + (currentRes.empty() ? "None" : currentRes) + "]: ");
        if (newRes.empty())
            break;
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

    // 4. Close Date (yyyy-mm-dd HH:mm) with setter try-catch
    while (true)
    {
        string currentClosed = ticket->getClosedDate();
        cout << "Note: Enter format 'yyyy-mm-dd HH:mm' or type 'now' for current time.\n";
        string newClosed = InputHelper::getInputOptionalString("New Close Date [" + (currentClosed.empty() ? "None" : currentClosed) + "]: ");
        if (newClosed.empty())
            break;
        if (newClosed == "now" || newClosed == "NOW")
        {
            newClosed = getCurrentDateTimeString();
        }
        try
        {
            ticket->setClosedDate(newClosed);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
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
    vector<int> widths = {8, 10, 10, 17, 25, 13, 25, 17};
    vector<string> headers = {"ID", "Contract", "STB ID", "Received Date", "Content", "Priority", "Resolution", "Closed Date"};
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

    // Removal dependency: Can not delete support ticket
    cout << "\n[Error] Cannot delete Support Ticket " << targetID << "!\n";
    cout << "[Rule] Removal dependency constraint: Support tickets cannot be deleted (must be kept for audit history).\n";
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
