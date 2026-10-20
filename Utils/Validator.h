// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193)
// Tien ich: Validator (Kiem tra tinh hop le cua du lieu)
// ===============================================================
#pragma once
#include <string>
using namespace std;
class Validator
{
public:
    static bool isValidInt(const string &input);
    static bool isValidDouble(const string &input);
    static bool isValidID(const string &input, const string &prefix, short length);
    static bool isValidPhone(const string &input, short length = 10);
    static bool isValidEmail(const string &input);
    static bool isValidDate(const string &input);
    static bool isValidTime(const string &input);
};