#include <iostream>
#include "InputHelper.h"
#include "Validator.h"
#include "Utils/Debugger.h"
using namespace std;
string trim_end_spaces(const string &str) // sanitizer (remove spaces of 2 ends)
{
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first)
        return ""; // full space
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}
static string force_lowercase(string &str)
{
    for (char &c : str)
    {
        c = tolower(c);
    }
    return str;
}
string normalizePhone(string &phone)
{
    string clean_phone = "";
    for (int i = 0; i < phone.size(); i++)
    {
        if (phone[i] == '+' || (phone[i] >= '0' && phone[i] <= '9'))
        {
            clean_phone += phone[i];
        }
    }
    if (clean_phone.length() >= 3 && clean_phone.substr(0, 3) == "+84")
    {
        clean_phone = "0" + clean_phone.substr(3);
    }
    return clean_phone;
}

int InputHelper::getInputInt(const string &message)
{

    string input;
    while (true)
    {
        cout << message << endl;
        getline(cin, input);
        input = trim_end_spaces(input);
        LOG("[InputHelper] Integer sanitized to: " + input);
        if (Validator::isValidInt(input))
        {
            LOG("[InputHelper] Integer is accepted, returning");
            return stoi(input);
        }
        else
        {
            cout << "[Error] The entered value is invalid, please try again" << endl;
            LOG("[InputHelper] Integer is rejected, please try again");
        }
    }
}
double InputHelper::getInputDouble(const string &message)
{
    string input;
    while (true)
    {
        cout << message << endl;
        getline(cin, input);
        input = trim_end_spaces(input);
        LOG("[InputHelper] Double sanitized to: " + input);
        if (Validator::isValidDouble(input))
        {
            LOG("[InputHelper] Double is accepted, returning");
            return stod(input);
        }
        else
        {
            cout << "[Error] The entered value is invalid, please try again" << endl;
            LOG("[InputHelper] Double is rejected, please try again");
        }
    }
}
string InputHelper::getInputString(const string &message)
{
    string input;
    while (true)
    {
        cout << message << endl;
        getline(cin, input);
        input = trim_end_spaces(input);
        if (input.length() != 0) // not empty case
        {
            LOG("[InputHelper] String is accepted, returning");
            return input;
        }
        else
        {
            cout << "[Error] The entered value is invalid, please try again" << endl;
            LOG("[InputHelper] String is rejected, please try again");
        }
    }
}
string InputHelper::getInputOptionalString(const string &message)
{
    string input;
    cout << message << " (Press Enter to skip): " << endl;
    getline(cin, input);
    input = trim_end_spaces(input);
    
    if (input.empty())
    {
        LOG("[InputHelper] Optional String skipped, returning empty string");
        return "";
    }
    
    LOG("[InputHelper] OptionalString is accepted, returning");
    return input;
}

string InputHelper::getInputEmail(const string &message)
{
    string input;
    while (true)
    {
        cout << message << endl;
        getline(cin, input);
        input = trim_end_spaces(input);
        input = force_lowercase(input);
        if (Validator::isValidEmail(input)) // not empty case
        {
            LOG("[InputHelper] Email is accepted, returning");
            return input;
        }
        else
        {
            cout << "[Error] The entered value is invalid, please try again" << endl;
            LOG("[InputHelper] Email is rejected, please try again");
        }
    }
}
string InputHelper::getInputPhone(const string &message)
{
    string input;
    while (true)
    {
        cout << message << endl;
        getline(cin, input);
        input = trim_end_spaces(input);
        input = normalizePhone(input);
        if (Validator::isValidPhone(input)) // not empty case
        {
            LOG("[InputHelper] Phone is accepted, returning");
            return input;
        }
        else
        {
            cout << "[Error] The entered value is invalid, please try again" << endl;
            LOG("[InputHelper] Phone is rejected, please try again");
        }
    }
}

int InputHelper::getInputOptionalInt(const string &message)
{
    string input;
    while (true)
    {
        cout << message << " (Press Enter to skip): " << endl;
        getline(cin, input);
        input = trim_end_spaces(input);

        if (input.empty())
        {
            LOG("[InputHelper] Optional Int skipped, returning -1");
            return -1; // -1 represents skipped
        }

        LOG("[InputHelper] Optional Int sanitized to: " + input);
        if (Validator::isValidInt(input))
        {
            LOG("[InputHelper] Optional Int is accepted, returning");
            return stoi(input);
        }
        else
        {
            cout << "[Error] The entered value is invalid, please try again" << endl;
            LOG("[InputHelper] Optional Int is rejected, please try again");
        }
    }
}

double InputHelper::getInputOptionalDouble(const string &message)
{
    string input;
    while (true)
    {
        cout << message << " (Press Enter to skip): " << endl;
        getline(cin, input);
        input = trim_end_spaces(input);

        if (input.empty())
        {
            LOG("[InputHelper] Optional Double skipped, returning -1.0");
            return -1.0; // -1.0 represents skipped
        }

        LOG("[InputHelper] Optional Double sanitized to: " + input);
        if (Validator::isValidDouble(input))
        {
            LOG("[InputHelper] Optional Double is accepted, returning");
            return stod(input);
        }
        else
        {
            cout << "[Error] The entered value is invalid, please try again" << endl;
            LOG("[InputHelper] Optional Double is rejected, please try again");
        }
    }
}

string InputHelper::getInputOptionalEmail(const string &message)
{
    string input;
    while (true)
    {
        cout << message << " (Press Enter to skip): " << endl;
        getline(cin, input);
        input = trim_end_spaces(input);

        if (input.empty())
        {
            LOG("[InputHelper] Optional Email skipped, returning empty string");
            return ""; // Empty string represents skipped
        }

        input = force_lowercase(input);
        if (Validator::isValidEmail(input))
        {
            LOG("[InputHelper] Optional Email is accepted, returning");
            return input;
        }
        else
        {
            cout << "[Error] The entered value is invalid, please try again" << endl;
            LOG("[InputHelper] Optional Email is rejected, please try again");
        }
    }
}

string InputHelper::getInputOptionalPhone(const string &message)
{
    string input;
    while (true)
    {
        cout << message << " (Press Enter to skip): " << endl;
        getline(cin, input);
        input = trim_end_spaces(input);

        if (input.empty())
        {
            LOG("[InputHelper] Optional Phone skipped, returning empty string");
            return ""; // Empty string represents skipped
        }

        input = normalizePhone(input);
        if (Validator::isValidPhone(input))
        {
            LOG("[InputHelper] Optional Phone is accepted, returning");
            return input;
        }
        else
        {
            cout << "[Error] The entered value is invalid, please try again" << endl;
            LOG("[InputHelper] Optional Phone is rejected, please try again");
        }
    }
}

Date InputHelper::getInputDate(const string &message)
{
    string input;
    while (true)
    {
        cout << message << " (Dinh dang dd/mm/yyyy): " << endl;
        getline(cin, input);
        input = trim_end_spaces(input);
        
        LOG("[InputHelper] Date sanitized to: " + input);
        if (Validator::isValidDate(input))
        {
            LOG("[InputHelper] Date is accepted, returning Date object");
            return Date(input); // Trả về luôn object Date!
        }
        else
        {
            cout << "[Error] Dinh dang ngay thang khong hop le, vui long nhap lai!" << endl;
            LOG("[InputHelper] Date is rejected, please try again");
        }
    }
}

Time InputHelper::getInputTime(const string &message)
{
    string input;
    while (true)
    {
        cout << message << " (hh:mm): " << endl;
        getline(cin, input);
        input = trim_end_spaces(input);
        
        LOG("[InputHelper] Time sanitized to: " + input);
        if (Validator::isValidTime(input))
        {
            LOG("[InputHelper] Time is accepted, returning Time object");
            return Time(input); 
        }
        else
        {
            cout << "[Error] Time is invalid" << endl;
            LOG("[InputHelper] Time is rejected, please try again");
        }
    }
}

Date InputHelper::getInputOptionalDate(const string &message)
{
    string input;
    while (true)
    {
        cout << message << " (Dinh dang dd/mm/yyyy - An Enter de bo qua): " << endl;
        getline(cin, input);
        input = trim_end_spaces(input);

        if (input.empty())
        {
            LOG("[InputHelper] Optional Date skipped, returning empty Date");
            return Date(); // Khởi tạo mặc định (-1, -1, -1)
        }

        LOG("[InputHelper] Optional Date sanitized to: " + input);
        if (Validator::isValidDate(input))
        {
            LOG("[InputHelper] Optional Date is accepted");
            return Date(input);
        }
        else
        {
            cout << "[Error] Dinh dang ngay thang khong hop le, vui long nhap lai!" << endl;
            LOG("[InputHelper] Optional Date is rejected");
        }
    }
}

Time InputHelper::getInputOptionalTime(const string &message)
{
    string input;
    while (true)
    {
        cout << message << " (Dinh dang hh:mm - An Enter de bo qua): " << endl;
        getline(cin, input);
        input = trim_end_spaces(input);

        if (input.empty())
        {
            LOG("[InputHelper] Optional Time skipped, returning empty Time");
            return Time(); // Khởi tạo mặc định (-1, -1)
        }

        LOG("[InputHelper] Optional Time sanitized to: " + input);
        if (Validator::isValidTime(input))
        {
            LOG("[InputHelper] Optional Time is accepted");
            return Time(input);
        }
        else
        {
            cout << "[Error] Dinh dang thoi gian khong hop le, vui long nhap lai!" << endl;
            LOG("[InputHelper] Optional Time is rejected");
        }
    }
}