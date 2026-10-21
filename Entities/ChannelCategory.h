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
    ChannelCategory() : Entity() {};
    ChannelCategory(const string &id, const string &name, const string &description);

    void setName(const string &name);
    void setDescription(const string &description);
    
    string getName() const;
    string getDescription() const;

    void parseFromString(const string &data) override;

    string exportToString() const override;
    vector<string> exportToVector() const override;

    bool operator<(const ChannelCategory &other) const;
};