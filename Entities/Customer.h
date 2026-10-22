// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193)
// Use Case 1: Quan ly Khach hang (Customer)
// ===============================================================
#pragma once
#include "Entity.h"
#include <string>
#include <vector>

using namespace std;

class Customer : public Entity
{
private:
    string name;
    string phone;
    string email;
    string address;

public:
    Customer() : Entity() {}
    Customer(string id, string name, string phone, string email, string address);

    void setName(const string &name);
    void setPhone(const string &phone);
    void setEmail(const string &email);
    void setAddress(const string &address);

    string getName() const;
    string getPhone() const;
    string getEmail() const;
    string getAddress() const;

    string exportToString() const override;
    vector<string> exportToVector() const override;
    void parseFromString(const string &data) override;
    
    bool operator<(const Customer &other) const;
    bool operator>(const Customer &other) const;
    bool operator==(const Customer &other) const;
};