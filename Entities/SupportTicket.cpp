#include "SupportTicket.h"
#include "Utils/StringHelper.h"
#include "Utils/Validator.h"
#include <stdexcept>

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
    if (str == "BINH_THUONG")
        return TicketPriority::BINH_THUONG;
    throw invalid_argument("[SupportTicket] Muc do uu tien khong hop le: " + str);
}

SupportTicket::SupportTicket() : Entity("")
{
    this->contractID = "";
    this->stbID = "";
    this->receivedDate = "";
    this->content = "";
    this->priority = TicketPriority::BINH_THUONG;
    this->resolution = "";
    this->closedDate = "";
}

SupportTicket::SupportTicket(string id, string contractID, string stbID, string receivedDate,
                             string content, TicketPriority priority,
                             string resolution, string closedDate)
    : Entity(id)
{
    setContractID(contractID);
    setStbID(stbID);
    setReceivedDate(receivedDate);
    setContent(content);
    setPriority(priority);
    setResolution(resolution);
    setClosedDate(closedDate);
}

void SupportTicket::setContractID(const string &contractID)
{
    if (contractID.empty())
        throw invalid_argument("[SupportTicket] Ma hop dong khong duoc de trong!");
    if (!Validator::isValidID(contractID, "HD", static_cast<short>(contractID.length())))
        throw invalid_argument("[SupportTicket] Ma hop dong khong hop le: " + contractID);
    this->contractID = contractID;
}

void SupportTicket::setStbID(const string &stbID)
{
    if (!stbID.empty() && !Validator::isValidID(stbID, "STB", static_cast<short>(stbID.length())))
        throw invalid_argument("[SupportTicket] Ma thiet bi STB khong hop le: " + stbID);
    this->stbID = stbID;
}

void SupportTicket::setReceivedDate(const string &receivedDate)
{
    if (receivedDate.empty())
        throw invalid_argument("[SupportTicket] Thoi diem tiep nhan khong duoc de trong!");
    if (receivedDate.find_first_of("|\r\n") != string::npos)
        throw invalid_argument("[SupportTicket] Thoi diem tiep nhan chua ky tu khong hop le!");
    this->receivedDate = receivedDate;
}

void SupportTicket::setContent(const string &content)
{
    if (content.empty())
        throw invalid_argument("[SupportTicket] Noi dung su co khong duoc de trong!");
    if (content.find_first_of("|\r\n") != string::npos)
        throw invalid_argument("[SupportTicket] Noi dung su co khong duoc chua ky tu '|' hoac xuong dong!");
    this->content = content;
}

void SupportTicket::setPriority(TicketPriority priority)
{
    this->priority = priority;
}

void SupportTicket::setResolution(const string &resolution)
{
    if (resolution.find_first_of("|\r\n") != string::npos)
        throw invalid_argument("[SupportTicket] Ket qua xu ly khong duoc chua ky tu '|' hoac xuong dong!");
    this->resolution = resolution;
}

void SupportTicket::setClosedDate(const string &closedDate)
{
    if (closedDate.find_first_of("|\r\n") != string::npos)
        throw invalid_argument("[SupportTicket] Thoi diem dong phieu chua ky tu khong hop le!");
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

string SupportTicket::getResolution() const
{
    return resolution;
}

string SupportTicket::getClosedDate() const
{
    return closedDate;
}

TicketStatus SupportTicket::getStatus() const
{
    if (!closedDate.empty() || !resolution.empty())
        return TicketStatus::DA_DONG;
    return TicketStatus::TIEP_NHAN;
}

string SupportTicket::getStatusString() const
{
    if (getStatus() == TicketStatus::DA_DONG)
        return "DA_DONG";
    return "TIEP_NHAN";
}

string SupportTicket::exportToString() const
{
    // Dung dinh dang khong co status:
    // id|contractid|stbid|receiveddate|content|priority|resolution|closeddate
    return id + "|" + contractID + "|" + stbID + "|" + receivedDate + "|" +
           content + "|" + getPriorityString() + "|" + resolution + "|" + closedDate;
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
        resolution,
        closedDate
    };
}

void SupportTicket::parseFromString(const string &data)
{
    vector<string> words = splitString(data);
    if (words.size() != 8)
    {
        throw invalid_argument("[SupportTicket] Dong du lieu phieu ho tro khong hop le (yeu cau dung 8 truong): " + data);
    }

    string ticketID = words[0];
    if (!Validator::isValidID(ticketID, "PHT", static_cast<short>(ticketID.length())))
    {
        throw invalid_argument("[SupportTicket] Ma phieu ho tro khong hop le: " + ticketID);
    }
    id = ticketID;

    setContractID(words[1]);
    setStbID(words[2]);
    setReceivedDate(words[3]);
    setContent(words[4]);
    setPriority(stringToTicketPriority(words[5]));

    string res = words[6];
    setResolution(res);

    string closed = words[7];
    if (!closed.empty() && closed.back() == '\r')
        closed.pop_back();
    setClosedDate(closed);
}
