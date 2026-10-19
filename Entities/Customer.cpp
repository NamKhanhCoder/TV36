// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193)
// Use Case 1: Quan ly Khach hang (Customer)
// ===============================================================
#include "Customer.h"
#include <string>
#include <stdexcept>
#include "Utils/StringHelper.h"
#include "Utils/Validator.h"

using namespace std;

Customer::Customer(string id, string name, string phone, string email, string address) : Entity(id)
{
    setName(name);
    setPhone(phone);
    setEmail(email);
    setAddress(address);
}

void Customer::setName(const string &name)
{
    this->name = name;
}

void Customer::setPhone(const string &phone)
{
    this->phone = phone;
}

void Customer::setEmail(const string &email)
{
    this->email = email;
}

void Customer::setAddress(const string &address)
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
    vector<string> words = splitString(data);

    if(words.size()!=5)
    {
        throw invalid_argument("[Customer] The line of data is not valid: "+data);
    }

    id = words[0];
    if(Validator::isValidID(id,"KH",5) == false)
    {
        throw invalid_argument("[Customer] The ID of data is not valid: "+data);
    }
    name = words[1];
    phone = words[2];
    if(Validator::isValidPhone(phone)==false)
    {
        throw invalid_argument("[Customer] The Phone of data is not valid: "+data);
    }
    email = words[3];
    if(Validator::isValidEmail(email)==false)
    {
        throw invalid_argument("[Customer] The Email of data is not valid: "+data);
    }
    string addr = words[4];
    if (!addr.empty() && addr.back() == '\r')
        addr.pop_back();
    address = addr;
}

bool Customer::operator<(const Customer &other) const
{
    if (this->name != other.name)
        return this->name < other.name;
    return this->id < other.id;
}

bool Customer::operator>(const Customer &other) const
{
    return other < *this;
}

bool Customer::operator==(const Customer &other) const
{
    return this->id == other.id;
}