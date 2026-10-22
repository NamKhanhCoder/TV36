// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Pham Quoc Trung (B24DCVT361)
// Use Case 3: Quan ly The loai kenh (Channel Category)
// ===============================================================
#pragma once
#include "Entity.h"
#include <string>

class ChannelCategory : public Entity
{
private:
    string name;
    string description;

public:
    // Ham khoi tao mac dinh
    ChannelCategory() : Entity() {};

    // Ham khoi tao co tham so
    ChannelCategory(const string &id, const string &name, const string &description);

    // Cac phuong thuc setter kiem tra tinh hop le cua du lieu
    void setName(const string &name);
    void setDescription(const string &description);
    
    // Cac phuong thuc getter
    string getName() const;
    string getDescription() const;

    // Ghi de phuong thuc tu Entity chuyen doi qua lai voi file van ban va bang hien thi
    void parseFromString(const string &data) override;
    string exportToString() const override;
    vector<string> exportToVector() const override;

    // Nap chong toan tu so sanh theo ten the loai (A - Z) phuc vu sap xep
    bool operator<(const ChannelCategory &other) const;
};
