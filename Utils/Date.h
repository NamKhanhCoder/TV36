// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193)
// Tien ich: Date (Quan ly & xu ly ngay thang)
// ===============================================================
#pragma once
#include <string>

using namespace std;

class Date
{
private:
    int day;
    int month;
    int year;

    bool isValidDate(int d, int m, int y) const;

public:
    // Constructors
    Date(); 
    Date(int d, int m, int y);
    Date(const string& dateStr); 

    // Getters
    int getDay() const;
    int getMonth() const;
    int getYear() const;

    // Output Formats
    string toString() const;
    string toFileString() const;

    // Date calculations & utils
    bool isEmpty() const;
    static bool isLeapYear(int y);
    static int daysInMonth(int m, int y);
    void addDays(int days);
    void addMonths(int months);

    static Date getCurrentDate();

    // Operator Overloading
    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;
    bool operator<(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator>=(const Date& other) const;
};
