// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Duong Bao Long (B24DCVT231)
// Use Case 2: Quan ly Phieu ho tro ki thuat Support Ticket
// ===============================================================
#pragma once
#include "Entity.h"
#include <string>
#include <vector>

using namespace std;

enum class TicketPriority
{
    BINH_THUONG,
    UU_TIEN,
    KHAN_CAP
};

enum class TicketStatus
{
    TIEP_NHAN,
    DANG_XU_LY,
    DA_DONG
};

string ticketPriorityToString(TicketPriority priority);
TicketPriority stringToTicketPriority(const string &str);

string ticketStatusToString(TicketStatus status);
TicketStatus stringToTicketStatus(const string &str);

class SupportTicket : public Entity
{
private:
    string contractID;
    string stbID;         // Tuỳ chọn, để trống nếu không liên quan thiết bị
    string receivedDate;  // yyyy-mm-dd HH:mm
    string content;
    TicketPriority priority;
    TicketStatus status;
    string resolution;    // Tuỳ chọn / ket qua xu ly
    string closedDate;    // yyyy-mm-dd HH:mm

public:
    SupportTicket();
    SupportTicket(string id, string contractID, string stbID, string receivedDate,
                  string content, TicketPriority priority,
                  TicketStatus status = TicketStatus::TIEP_NHAN,
                  string resolution = "", string closedDate = "");

    void setContractID(const string &contractID);
    void setStbID(const string &stbID);
    void setReceivedDate(const string &receivedDate);
    void setContent(const string &content);
    void setPriority(TicketPriority priority);
    void setStatus(TicketStatus status);
    void setResolution(const string &resolution);
    void setClosedDate(const string &closedDate);

    string getContractID() const;
    string getStbID() const;
    string getReceivedDate() const;
    string getContent() const;
    TicketPriority getPriority() const;
    string getPriorityString() const;
    TicketStatus getStatus() const;
    string getStatusString() const;
    string getResolution() const;
    string getClosedDate() const;

    string exportToString() const override;
    vector<string> exportToVector() const override;
    void parseFromString(const string &data) override;
};
