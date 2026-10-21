// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Hong Anh (NotaSpookyGhost)
// Use Case 4: Quan ly kenh truyen hinh (Channel)
// ===============================================================
#pragma once
#include "Entity.h"
#include <string>

// Du lieu mot kenh; setter bao ve gia tri, Menu kiem tra lien ket voi doi tuong khac.
class Channel : public Entity
{
private:
    string name;
    string categoryID;
    int channelNumber;
    string resolution;
    string streamURL;
    string status;

public:
    // Tao ban nhap tam; can dien du truong truoc khi luu vao Repository.
    Channel();
    // Tao kenh hoan chinh; throw invalid_argument neu bat ky truong nao sai.
    Channel(const string& id, const string& name, const string& categoryID, int channelNumber, const string& resolution, const string& streamURL, const string& status);
    // Lay ten kenh.
    string getName() const;
    // Ten bat buoc, khong chua dau phan cach file hoac xuong dong.
    void setName(const string& value);
    // Lay ma the loai.
    string getCategoryID() const;
    // Ma the loai co dang TL + it nhat 3 chu so; Menu kiem tra ton tai.
    void setCategoryID(const string& value);
    // Lay so thu tu kenh.
    int getChannelNumber() const;
    // So kenh phai duong; Menu kiem tra trung voi kenh khac.
    void setChannelNumber(int value);
    // Lay do phan giai.
    string getResolution() const;
    // Chi chap nhan SD, HD, FHD hoac UHD.
    void setResolution(const string& value);
    // Lay dia chi luong phat.
    string getStreamURL() const;
    // URL bat buoc va khong duoc pha dinh dang file; khong kiem tra ket noi mang.
    void setStreamURL(const string& value);
    // Lay trang thai luu trong file.
    string getStatus() const;
    // Chi chap nhan DANG_PHAT hoac NGUNG_PHAT.
    void setStatus(const string& value);

    // Doc 7 cot; throw neu sai va giu nguyen doi tuong hien tai.
    void parseFromString(const string& data) override;
    // Xuat 7 cot ngan cach bang | de Repository ghi file.
    string exportToString() const override;
    // Xuat cac cot theo thu tu bang hien thi.
    vector<string> exportToVector() const override;
};
