#include "SupportTicket.h"
#include "Utils/StringHelper.h"

using namespace std;

string ticketPriorityToString(TicketPriority priority)
{
    switch (priority)
    {
    case TicketPriority::BINH_THUONG:
        return "BINH_THUONG";
    case TicketPriority::UU_TIEN:
        return "UU_TIEN";
    case TicketPriority::KHAN_CAP:
        return "KHAN_CAP";
    default:
        return "BINH_THUONG";
    }
}

TicketPriority stringToTicketPriority(const string &str)
{
    if (str == "UU_TIEN")
        return TicketPriority::UU_TIEN;
    if (str == "KHAN_CAP")
        return TicketPriority::KHAN_CAP;
    return TicketPriority::BINH_THUONG;
}

string ticketStatusToString(TicketStatus status)
{
    switch (status)
    {
    case TicketStatus::TIEP_NHAN:
        return "TIEP_NHAN";
    case TicketStatus::DANG_XU_LY:
        return "DANG_XU_LY";
    case TicketStatus::DA_DONG:
        return "DA_DONG";
    default:
        return "TIEP_NHAN";
    }
}

TicketStatus stringToTicketStatus(const string &str)
{
    if (str == "DANG_XU_LY")
        return TicketStatus::DANG_XU_LY;
    if (str == "DA_DONG")
        return TicketStatus::DA_DONG;
    return TicketStatus::TIEP_NHAN;
}

SupportTicket::SupportTicket() : Entity("")
{
    this->contractID = "";
    this->stbID = "";
    this->receivedDate = "";
    this->content = "";
    this->priority = TicketPriority::BINH_THUONG;
    this->status = TicketStatus::TIEP_NHAN;
    this->resolution = "";
    this->closedDate = "";
}

SupportTicket::SupportTicket(string id, string contractID, string stbID, string receivedDate,
                             string content, TicketPriority priority, TicketStatus status,
                             string resolution, string closedDate)
    : Entity(id)
{
    this->contractID = contractID;
    this->stbID = stbID;
    this->receivedDate = receivedDate;
    this->content = content;
    this->priority = priority;
    this->status = status;
    this->resolution = resolution;
    this->closedDate = closedDate;
}

void SupportTicket::setContractID(const string &contractID)
{
    this->contractID = contractID;
}

void SupportTicket::setStbID(const string &stbID)
{
    this->stbID = stbID;
}

void SupportTicket::setReceivedDate(const string &receivedDate)
{
    this->receivedDate = receivedDate;
}

void SupportTicket::setContent(const string &content)
{
    this->content = content;
}

void SupportTicket::setPriority(TicketPriority priority)
{
    this->priority = priority;
}

void SupportTicket::setStatus(TicketStatus status)
{
    this->status = status;
}

void SupportTicket::setResolution(const string &resolution)
{
    this->resolution = resolution;
}

void SupportTicket::setClosedDate(const string &closedDate)
{
    this->closedDate = closedDate;
}

string SupportTicket::getContractID() const
{
    return contractID;
}

string SupportTicket::getStbID() const
{
    return stbID;
}

string SupportTicket::getReceivedDate() const
{
    return receivedDate;
}

string SupportTicket::getContent() const
{
    return content;
}

TicketPriority SupportTicket::getPriority() const
{
    return priority;
}

string SupportTicket::getPriorityString() const
{
    return ticketPriorityToString(priority);
}

TicketStatus SupportTicket::getStatus() const
{
    return status;
}

string SupportTicket::getStatusString() const
{
    return ticketStatusToString(status);
}

string SupportTicket::getResolution() const
{
    return resolution;
}

string SupportTicket::getClosedDate() const
{
    return closedDate;
}

string SupportTicket::exportToString() const
{
    // id|contractid|stbid|receiveddate|content|priority|status|resolution|closeddate
    return id + "|" + contractID + "|" + stbID + "|" + receivedDate + "|" +
           content + "|" + getPriorityString() + "|" + getStatusString() + "|" +
           resolution + "|" + closedDate;
}

vector<string> SupportTicket::exportToVector() const
{
    return {
        id,
        contractID,
        stbID,
        receivedDate,
        content,
        getPriorityString(),
        getStatusString(),
        resolution,
        closedDate
    };
}

void SupportTicket::parseFromString(const string &data)
{
    vector<string> words = splitString(data);
    if (words.size() >= 7)
    {
        id = words[0];
        contractID = words[1];
        stbID = words[2];
        receivedDate = words[3];
        content = words[4];
        priority = stringToTicketPriority(words[5]);
        status = stringToTicketStatus(words[6]);

        resolution = (words.size() >= 8) ? words[7] : "";
        closedDate = (words.size() >= 9) ? words[8] : "";
    }
}

