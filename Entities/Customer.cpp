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
    if (name.empty())
        throw invalid_argument("[Customer] Customer name cannot be empty!");
    if (name.find_first_of("|\r\n") != string::npos)
        throw invalid_argument("[Customer] Customer name must not contain '|' or line breaks!");
    this->name = name;
}

void Customer::setPhone(const string &phone)
{
    if (!Validator::isValidPhone(phone))
        throw invalid_argument("[Customer] Invalid phone number (must be 10 digits and start with 0)!");
    this->phone = phone;
}

void Customer::setEmail(const string &email)
{
    if (!Validator::isValidEmail(email))
        throw invalid_argument("[Customer] Invalid email address format!");
    this->email = email;
}

void Customer::setAddress(const string &address)
{
    if (address.empty())
        throw invalid_argument("[Customer] Address cannot be empty!");
    if (address.find_first_of("|\r\n") != string::npos)
        throw invalid_argument("[Customer] Address must not contain '|' or line breaks!");
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

    if (words.size() != 5)
    {
        throw invalid_argument("[Customer] The line of data is not valid: " + data);
    }

    id = words[0];
    if (Validator::isValidID(id, "KH", 5) == false)
    {
        throw invalid_argument("[Customer] The ID of data is not valid: " + data);
    }

    setName(words[1]);
    setPhone(words[2]);
    setEmail(words[3]);
    string addr = words[4];
    if (!addr.empty() && addr.back() == '\r')
        addr.pop_back();
    setAddress(addr);
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