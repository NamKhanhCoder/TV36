// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Hong Anh (NotaSpookyGhost)
// Use Case 5: Quan ly lich phat song (Schedule)
// ===============================================================
#pragma once
#include "Entity.h"
#include <string>

// Du lieu mot chuong trinh theo khoang gio; Menu kiem tra kenh va lich trung.
class Schedule : public Entity
{
private:
    string channelID;
    string programName;
    string startTime;
    string endTime;
    string description;

public:
    // Tao ban nhap tam; can dien du truong truoc khi luu.
    Schedule();
    // Tao lich hoan chinh va kiem tra tat ca thuoc tinh.
    Schedule(const string& id, const string& channelID, const string& programName, const string& startTime, const string& endTime, const string& description);
    // Lay ma kenh phat song.
    string getChannelID() const;
    // Ma kenh co dang KENH + it nhat 3 chu so; Menu kiem tra ton tai.
    void setChannelID(const string& value);
    // Lay ten chuong trinh.
    string getProgramName() const;
    // Ten bat buoc, khong chua | hoac xuong dong.
    void setProgramName(const string& value);
    // Lay moc bat dau theo yyyy-mm-dd HH:mm.
    string getStartTime() const;
    // Bat dau phai hop le va truoc moc ket thuc hien co.
    void setStartTime(const string& value);
    // Lay moc ket thuc theo yyyy-mm-dd HH:mm.
    string getEndTime() const;
    // Ket thuc phai hop le va sau moc bat dau hien co.
    void setEndTime(const string& value);
    // Lay mo ta, co the rong.
    string getDescription() const;
    // Cho phep rong, cam | va xuong dong de bao ve dinh dang file.
    void setDescription(const string& value);
    // Kiem tra ca hai moc truoc khi gan; end > start, cho phep qua nua dem.
    void setTimeRange(const string& start, const string& end);
    // Kiem tra ngay gio thuc te va dinh dang ISO; throw invalid_argument neu sai.
    static string normalizeDateTime(const string& value);

    // Doc 6 cot; throw neu sai va giu nguyen doi tuong hien tai.
    void parseFromString(const string& data) override;
    // Xuat 6 cot ngan cach bang | de Repository ghi file.
    string exportToString() const override;
    // Xuat cac cot theo thu tu bang hien thi.
    vector<string> exportToVector() const override;
};
