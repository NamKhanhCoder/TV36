// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Duong Bao Long (B24DCVT231)
// Use Case 2: Quan ly Phieu ho tro ki thuat Support Ticket
// ===============================================================
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
    if (str == "TIEP_NHAN")
        return TicketStatus::TIEP_NHAN;
    if (str == "DANG_XU_LY")
        return TicketStatus::DANG_XU_LY;
    if (str == "DA_DONG")
        return TicketStatus::DA_DONG;
    throw invalid_argument("[SupportTicket] Trang thai phieu ho tro khong hop le: " + str);
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
                             string content, TicketPriority priority,
                             TicketStatus status,
                             string resolution, string closedDate)
    : Entity(id)
{
    setContractID(contractID);
    setStbID(stbID);
    setReceivedDate(receivedDate);
    setContent(content);
    setPriority(priority);
    setStatus(status);
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

void SupportTicket::setStatus(TicketStatus status)
{
    this->status = status;
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
    // Dinh dang 9 truong co status:
    // id|contractid|stbid|receiveddate|content|priority|status|resolution|closeddate
    return id + "|" + contractID + "|" + stbID + "|" + receivedDate + "|" +
           content + "|" + getPriorityString() + "|" + getStatusString() + "|" + resolution + "|" + closedDate;
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
    if (words.size() != 9 && words.size() != 8)
    {
        throw invalid_argument("[SupportTicket] Dong du lieu phieu ho tro khong hop le (yeu cau 9 truong): " + data);
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

    if (words.size() == 9)
    {
        setStatus(stringToTicketStatus(words[6]));
        setResolution(words[7]);
        string closed = words[8];
        if (!closed.empty() && closed.back() == '\r')
            closed.pop_back();
        setClosedDate(closed);
    }
    else
    {
        // Tuong thich nguoc du lieu 8 truong (neu co)
        string res = words[6];
        setResolution(res);
        string closed = words[7];
        if (!closed.empty() && closed.back() == '\r')
            closed.pop_back();
        setClosedDate(closed);

        if (!closed.empty() || !res.empty())
            setStatus(TicketStatus::DA_DONG);
        else
            setStatus(TicketStatus::TIEP_NHAN);
    }
}
