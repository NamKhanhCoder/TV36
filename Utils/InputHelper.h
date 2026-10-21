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
    static int getInputInt(const string &message = "");
    static double getInputDouble(const string &message = "");
    static string getInputString(const string &message = "");
    static string getInputEmail(const string &message = "");
    static string getInputPhone(const string &message = "");
    static Date getInputDate(const string &message = "");
    static Time getInputTime(const string &message = "");
    // 0 -> NGUNG_PHAT, 1 -> DANG_PHAT; optional cho phep Enter de giu nguyen.
    static string getInputChannelStatus(const string &message = "Channel status", bool optional = false);
    static string getInputID(const string &message, const string &prefix, bool optional = false);
    // Tra 0 neu bo trong luc sua; du lieu nhap thuc te phai lon hon 0.
    static int getInputPositiveInt(const string &message, bool optional = false);
    static string getInputChannelResolution(const string &message, bool optional = false);
    static string getInputText(const string &message, bool optional = false);
    static string getInputDateTime(const string &message, bool optional = false);
    static bool getInputConfirmation(const string &message);

    static int getInputOptionalInt(const string &message = "");
    static double getInputOptionalDouble(const string &message = "");
    static string getInputOptionalString(const string &message = "");
    static string getInputOptionalEmail(const string &message = "");
    static string getInputOptionalPhone(const string &message = "");
    static Date getInputOptionalDate(const string &message = "");
    static Time getInputOptionalTime(const string &message = "");
};
