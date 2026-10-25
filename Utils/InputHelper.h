// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193)
// Tien ich: InputHelper (Nhap lieu an toan & chong troi lenh)
// ===============================================================
#pragma once
#include <string>
#include "Utils/Date.h"
#include "Time.h"
using namespace std;

class InputHelper
{
public:
    // End of a console/redirected input session is control flow, not a
    // validation error. Keep it outside std::exception so retry handlers
    // cannot swallow it and loop forever.
    struct EndOfInput {};

    static int getInputInt(const string &message = "");
    static double getInputDouble(const string &message = "");
    static string getInputString(const string &message = "");
    static string getInputEmail(const string &message = "");
    static string getInputPhone(const string &message = "");
    static Date getInputDate(const string &message = "");
    static Time getInputTime(const string &message = "");
    static string getInputID(const string &message, const string &prefix, bool optional = false);

    static int getInputOptionalInt(const string &message = "");
    static double getInputOptionalDouble(const string &message = "");
    static string getInputOptionalString(const string &message = "");
    static string getInputOptionalEmail(const string &message = "");
    static string getInputOptionalPhone(const string &message = "");
    static Date getInputOptionalDate(const string &message = "");
    static Time getInputOptionalTime(const string &message = "");
};
