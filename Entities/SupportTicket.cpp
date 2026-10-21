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
#include "Utils/Date.h"
#include "Utils/Time.h"
#include <stdexcept>
#include <cctype>

using namespace std;

namespace {
    string trimStr(const string &s)
    {
        size_t start = 0;
        while (start < s.size() && isspace(static_cast<unsigned char>(s[start])))
        {
            start++;
        }
        size_t end = s.size();
        while (end > start && isspace(static_cast<unsigned char>(s[end - 1])))
        {
            end--;
        }
        return s.substr(start, end - start);
    }
}

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
    string s = trimStr(str);
    if (s == "UU_TIEN")
        return TicketPriority::UU_TIEN;
    if (s == "KHAN_CAP")
        return TicketPriority::KHAN_CAP;
    if (s == "BINH_THUONG")
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
    string s = trimStr(str);
    if (s == "TIEP_NHAN")
        return TicketStatus::TIEP_NHAN;
    if (s == "DANG_XU_LY")
        return TicketStatus::DANG_XU_LY;
    if (s == "DA_DONG")
        return TicketStatus::DA_DONG;
    throw invalid_argument("[SupportTicket] Trang thai phieu ho tro khong hop le: " + str);
}

string SupportTicket::validateAndNormalizeDateTime(const string &value)
{
    if (value.empty()) return "";
    if (value.size() != 16 || value[4] != '-' || value[7] != '-' ||
        value[10] != ' ' || value[13] != ':')
    {
        throw invalid_argument("[SupportTicket] Dinh dang ngay gio phai la yyyy-mm-dd HH:mm (vi du 2026-10-16 19:00): " + value);
    }
    for (size_t i = 0; i < value.size(); ++i)
    {
        if (i == 4 || i == 7 || i == 10 || i == 13) continue;
        if (value[i] < '0' || value[i] > '9')
        {
            throw invalid_argument("[SupportTicket] Ngay gio chi duoc chua chu so: " + value);
        }
    }
    // Kiem tra gia tri thuc te qua Date va Time
    Date(value.substr(0, 10));
    Time(value.substr(11, 5));
    return value;
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
    string cid = trimStr(contractID);
    if (cid.empty())
        throw invalid_argument("[SupportTicket] Ma hop dong khong duoc de trong!");
    if (cid.length() < 5 || !Validator::isValidID(cid, "HD", static_cast<short>(cid.length())))
        throw invalid_argument("[SupportTicket] Ma hop dong khong hop le: " + cid);
    this->contractID = cid;
}

void SupportTicket::setStbID(const string &stbID)
{
    string sid = trimStr(stbID);
    if (!sid.empty() && (sid.length() < 6 || !Validator::isValidID(sid, "STB", static_cast<short>(sid.length()))))
        throw invalid_argument("[SupportTicket] Ma thiet bi STB khong hop le: " + sid);
    this->stbID = sid;
}

void SupportTicket::setReceivedDate(const string &receivedDate)
{
    string rd = trimStr(receivedDate);
    if (rd.empty())
        throw invalid_argument("[SupportTicket] Thoi diem tiep nhan khong duoc de trong!");
    validateAndNormalizeDateTime(rd);
    this->receivedDate = rd;
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
    string cd = trimStr(closedDate);
    if (!cd.empty())
    {
        validateAndNormalizeDateTime(cd);
    }
    this->closedDate = cd;
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

    string ticketID = trimStr(words[0]);
    // Chan bare-prefix (phai bat dau bang PHT va co it nhat 3 chu so, length >= 6)
    if (ticketID.length() < 6 || !Validator::isValidID(ticketID, "PHT", static_cast<short>(ticketID.length())))
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
        string closed = trimStr(words[8]);
        if (!closed.empty() && closed.back() == '\r')
            closed.pop_back();
        setClosedDate(closed);
    }
    else
    {
        string res = words[6];
        setResolution(res);
        string closed = trimStr(words[7]);
        if (!closed.empty() && closed.back() == '\r')
            closed.pop_back();
        setClosedDate(closed);

        if (!closed.empty() || !res.empty())
            setStatus(TicketStatus::DA_DONG);
        else
            setStatus(TicketStatus::TIEP_NHAN);
    }
}

bool SupportTicket::operator<(const SupportTicket &other) const
{
    if (this->receivedDate != other.receivedDate)
        return this->receivedDate < other.receivedDate;
    return this->id < other.id;
}

bool SupportTicket::operator>(const SupportTicket &other) const
{
    return other < *this;
}

bool SupportTicket::operator==(const SupportTicket &other) const
{
    return this->id == other.id;
}
