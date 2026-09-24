#include "Customer.h"
#include <string>
#include "Utils/StringHelper.h"
using namespace std;
Customer::Customer(string id, string name, string phone, string email, string address) : Entity(id)
{
    this->name = name;
    this->phone = phone;
    this->email = email;
    this->address = address;
}
void Customer::setName(string &name)
{
    this->name = name;
}
void Customer::setPhone(string &phone)
{
    this->phone = phone;
}
void Customer::setEmail(string &email)
{
    this->email = email;
}
void Customer::setAddress(string &address)
{
    this->address = address;
}

string Customer::getName() const
{
    return name;
}
string Customer::getPhone() const
{
    return phone;
}
string Customer::getEmail() const
{
    return email;
}
string Customer::getAddress() const
{
    return address;
}

string Customer::exportToString() const
{
    return id + "|" + name + "|" + phone + "|" + email + "|" + address;
}
vector<string> Customer::exportToVector() const
{
    vector<string> result = {id, name, phone, email, address};
    return result;
}
void Customer::parseFromString(const string &data)
{
    vector<string> words;
    words=split(data);

    if (words.size() >= 5) 
    {
        id      = words[0];
        name    = words[1];
        phone   = words[2];
        email   = words[3];
        address = words[4];
    }
}