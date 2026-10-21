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

/**
 * @brief Enum bieu dien muc do uu tien cua phieu ho tro.
 */
enum class TicketPriority
{
    BINH_THUONG, ///< Xu ly theo quy trinh thong thuong
    UU_TIEN,     ///< Su co anh huong trai nghiem can xu ly som
    KHAN_CAP     ///< Mat tin hieu hoan toan hoac su co thiet bi nghiem trong
};

/**
 * @brief Enum bieu dien tien trinh vong doi cua phieu ho tro.
 */
enum class TicketStatus
{
    TIEP_NHAN,  ///< Vua tao moi, chua co ky thuat vien xu ly
    DANG_XU_LY, ///< Ky thuat vien dang kiem tra, xu ly su co
    DA_DONG     ///< Su co da duoc khac phuc, da co bien ban va ngay dong
};

string ticketPriorityToString(TicketPriority priority);
TicketPriority stringToTicketPriority(const string &str);

string ticketStatusToString(TicketStatus status);
TicketStatus stringToTicketStatus(const string &str);

/**
 * @brief Lop thuc the quan ly Phieu ho tro ky thuat cua khach hang.
 */
class SupportTicket : public Entity
{
private:
    string contractID;    ///< Ma hop dong tham chieu (bat buoc, HDxxx)
    string stbID;         ///< Ma Set-top Box su co (tuy chon, neu co phai khop hop dong)
    string receivedDate;  ///< Thoi diem tiep nhan yyyy-mm-dd HH:mm
    string content;       ///< Noi dung mo ta su co
    TicketPriority priority; ///< Muc do uu tien
    TicketStatus status;  ///< Trang thai vong doi
    string resolution;    ///< Ket qua xu ly (bat buoc khi DA_DONG)
    string closedDate;    ///< Thoi diem dong phieu yyyy-mm-dd HH:mm (bat buoc khi DA_DONG)

public:
    /**
     * @brief Constructor mac dinh.
     */
    SupportTicket();

    /**
     * @brief Constructor day du tham so.
     */
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

    /**
     * @brief Kiem tra va chuan hoa chuoi ngay gio yyyy-mm-dd HH:mm.
     * @param value Chuoi thoi diem.
     * @return Chuoi thoi diem chuan neu hop le.
     * @throw std::invalid_argument neu sai dinh dang hoac sai ngay gio thuc te.
     */
    static string validateAndNormalizeDateTime(const string &value);

    string exportToString() const override;
    vector<string> exportToVector() const override;
    void parseFromString(const string &data) override;

    // Nạp chồng toán tử so sánh (Rubric OOP)
    bool operator<(const SupportTicket &other) const;
    bool operator>(const SupportTicket &other) const;
    bool operator==(const SupportTicket &other) const;
};
