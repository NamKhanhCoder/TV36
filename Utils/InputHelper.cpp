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

string InputHelper::getInputChannelStatus(const string &message, bool optional)
{
    string input;
    while (true)
    {
        cout << message << " (0: Stopped/NGUNG_PHAT, 1: Active/DANG_PHAT";
        if (optional)
            cout << ", Enter: keep current";
        cout << "): " << endl;
        if (!getline(cin, input))
            throw runtime_error("Failed to read channel status.");
        input = trim_end_spaces(input);

        if (optional && input.empty())
            return "";
        if (input == "0")
            return "NGUNG_PHAT";
        if (input == "1")
            return "DANG_PHAT";
        cout << "[Error] Please enter 0 or 1 only (Chi nhap 0 hoac 1). Please try again.\n";
    }
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

int InputHelper::getInputPositiveInt(const string &message, bool optional)
{
    string input;
    while (true)
    {
        cout << message << endl;
        if (!getline(cin, input))
            throw runtime_error("Failed to read number.");
        input = trim_end_spaces(input);
        if (optional && input.empty())
            return 0;
        if (Validator::isValidInt(input))
        {
            int value = stoi(input);
            if (value > 0)
                return value;
        }
        cout << "[Error] Channel number must be an integer greater than 0 (So kenh phai la so nguyen lon hon 0). Please try again.\n";
    }
}

string InputHelper::getInputChannelResolution(const string &message, bool optional)
{
    string input;
    while (true)
    {
        cout << message << endl;
        if (!getline(cin, input))
            throw runtime_error("Failed to read resolution.");
        input = trim_end_spaces(input);
        if (optional && input.empty())
            return "";
        transform(input.begin(), input.end(), input.begin(),
                  [](unsigned char c)
                  { return static_cast<char>(toupper(c)); });
        if (input == "SD" || input == "HD" || input == "FHD" || input == "UHD")
            return input;
        cout << "[Error] Resolution only accepts SD, HD, FHD, or UHD (Do phan giai chi nhan SD, HD, FHD hoac UHD). Please try again.\n";
    }
}

string InputHelper::getInputText(const string &message, bool optional)
{
    string input;
    while (true)
    {
        cout << message << endl;
        if (!getline(cin, input))
            throw runtime_error("Failed to read data.");
        size_t first = input.find_first_not_of(" \t\v\f");
        if (first == string::npos)
            input.clear();
        else
            input = input.substr(first, input.find_last_not_of(" \t\v\f") - first + 1);
        if (optional && input.empty())
            return "";
        if (!input.empty() && input.find_first_of("|\r\n") == string::npos)
            return input;
        cout << "[Error] Text must not be empty or contain | / line breaks. Please enter again.\n";
    }
}

string InputHelper::getInputDateTime(const string &message, bool optional)
{
    while (true)
    {
        string input = getInputText(message, optional);
        if (optional && input.empty())
            return "";
        try
        {
            if (input.size() != 16 || input[4] != '-' || input[7] != '-' ||
                input[10] != ' ' || input[13] != ':')
                throw invalid_argument("Use yyyy-mm-dd HH:mm.");
            for (size_t i = 0; i < input.size(); ++i)
            {
                if (i == 4 || i == 7 || i == 10 || i == 13)
                    continue;
                if (input[i] < '0' || input[i] > '9')
                    throw invalid_argument("Date/time must contain digits.");
            }
            Date(input.substr(0, 10));
            Time(input.substr(11, 5));
            return input;
        }
        catch (const exception &error)
        {
            cout << "[Error] Invalid date/time: " << error.what() << " Please enter again.\n";
        }
    }
}

bool InputHelper::getInputConfirmation(const string &message)
{
    while (true)
    {
        string input = getInputText(message);
        if (input == "y" || input == "Y")
            return true;
        if (input == "n" || input == "N")
            return false;
        cout << "[Error] Please enter y or n.\n";
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
        cout << message << " (dd/mm/yyyy): " << endl;
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
        cout << message << " (dd/mm/yyyy): " << endl;
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
