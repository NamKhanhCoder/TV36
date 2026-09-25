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

    // Operator Overloading
    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;
    bool operator<(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator>=(const Date& other) const;
};
