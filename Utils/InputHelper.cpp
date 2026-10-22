// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193)
// Tien ich: InputHelper (Nhap lieu an toan & chong troi lenh)
// ===============================================================
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <limits>
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

string InputHelper::getInputID(const string &message, const string &prefix, bool optional)
{
    string input;
    while (true)
    {
        cout << message << endl;
        if (!getline(cin, input))
            throw runtime_error("Failed to read ID.");
        input = trim_end_spaces(input);
        if (optional && input.empty())
            return "";
        transform(input.begin(), input.end(), input.begin(),
                  [](unsigned char c)
                  { return static_cast<char>(toupper(c)); });
        // Repository padding toi thieu 3 chu so; chap nhan ca TL1000, KENH1000.
        if (input.size() >= prefix.size() + 3 &&
            input.size() <= static_cast<size_t>(numeric_limits<short>::max()) &&
            Validator::isValidID(input, prefix, static_cast<short>(input.size())))
            return input;
        cout << "[Error] Invalid ID (Ma khong hop le). Use " << prefix
             << " and at least 3 digits (e.g. " << prefix << "004). Please try again.\n";
    }
}

int InputHelper::getInputInt(const string &message)
{

    string input;
    while (true)
    {
        cout << message << endl;
        if (!getline(cin, input))
        {
            cin.clear();
            continue;
        }
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
        if (!getline(cin, input))
        {
            cin.clear();
            continue;
        }
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
        if (!getline(cin, input))
        {
            cin.clear(); // Xóa cờ lỗi EOF/failbit để bàn phím nhận diện lại
            continue;    // Quay lại đầu vòng lặp chờ người dùng gõ tiếp
        }

        input = trim_end_spaces(input);
        if (input.empty())
        {
            cout << "[Error] The entered value is invalid, please try again" << endl;
            LOG("[InputHelper] String is rejected, please try again");
            continue;
        }
        if (input.find_first_of("|\r\n") != string::npos)
        {
            cout << "[Error] The entered value contains illegal characters, please try again" << endl;
            LOG("[InputHelper] String is rejected, please try again");
            continue;
        }
        return input;
    }
}
string InputHelper::getInputOptionalString(const string &message)
{
    string input;
    while (true)
    {
        cout << message << endl;
        if (!getline(cin, input))
        {
            cin.clear();
            return "";
        }

        input = trim_end_spaces(input);
        if (input.empty())
            return "";

        if (input.find_first_of("|\r\n") != string::npos) // case ịnection
        {
            cout << "[Error] The entered value contains illegal characters, please try again" << endl;
            continue;
        }
        return input;
    }
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
        cout << message << endl;
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
        cout << message << endl;
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
        cout << message << endl;
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
        cout << message << endl;
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
        cout << message <<endl;
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
            cout << "[Error] Invalid Date format" << endl;
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
        cout << message << endl;
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
            cout << "[Error] Invalid Optional Date" << endl;
            LOG("[InputHelper] Optional Date is rejected");
        }
    }
}

Time InputHelper::getInputOptionalTime(const string &message)
{
    string input;
    while (true)
    {
        cout << message << " (hh:mm): " << endl;
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
            cout << "[Error] Invalid Optional Time" << endl;
            LOG("[InputHelper] Optional Time is rejected");
        }
    }
}
