#include "Date.h"

Date::Date() // set default date
{
    this->day = 1;
    this->month = 1;
    this->year = 2000;
}
Date::Date(int d, int m, int y) 
{
    this->day = d;
    this->month = m;
    this->year = y;
}
Date::Date(string input)
{
    day   = stoi(input.substr(0, 2));  // Cắt 2 ký tự từ vị trí 0
    month = stoi(input.substr(3, 2));  // Cắt 2 ký tự từ vị trí 3
    year  = stoi(input.substr(6));     // Cắt từ vị trí 6 đến hết
}
bool Date::isValid()
{
    if (month <= 0 || month > 12) // check month validation
    {
        return false;
    }

    if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) // check day validation (case month 31)
    {
        return (day >= 1 && day <= 31);
    }
    else if (month == 4 || month == 6 || month == 9 || month == 11) // check day validation (case month 30)
    {
        return (day >= 1 && day <= 30);
    }

    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) // check day validation (case february leap year)
    {
        return (day >= 1 && day <= 29);
    }
    else // case feb not leap year
    {
        return (day >= 1 && day <= 28);
    }
}

bool Date::operator<(Date comparedDate)
{
    if (year != comparedDate.year)
        return year < comparedDate.year;
    if (month != comparedDate.month)
        return month < comparedDate.month;
    if (day != comparedDate.day)
        return day < comparedDate.day;

    // after all cases, the 2 dates are equal, so its false for operator <
    return false;
}

bool Date::operator>(Date comparedDate)
{
    if (year != comparedDate.year)
        return year > comparedDate.year;
    if (month != comparedDate.month)
        return month > comparedDate.month;
    if (day != comparedDate.day)
        return day > comparedDate.day;

    // after all cases, the 2 dates are equal, so its false for operator <
    return false;
}

bool Date::operator==(Date comparedDate)
{
    return (day == comparedDate.day && month == comparedDate.month && year == comparedDate.year);
}
string Date::toString()
{
    string d = (day < 10)   ? "0" + to_string(day)   : to_string(day);
    string m = (month < 10) ? "0" + to_string(month) : to_string(month);
    return d + "/" + m + "/" + to_string(year);
}