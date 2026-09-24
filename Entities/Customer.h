// What makes you feel human?
#pragma once
#include "Entity.h"
#include <string.h>

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
    void setName(string &name);
    void setPhone(string &phone);
    void setEmail(string &email);
    void setAddress(string &address);

    string getName() const;
    string getPhone() const;
    string getEmail() const;
    string getAddress() const;

    string exportToString() const override;
    vector<string> exportToVector() const override;

    void parseFromString(const string &data) override;
};