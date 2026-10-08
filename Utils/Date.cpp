#include "Date.h"
#include <stdexcept>
#include <vector>

bool Date::isValidDate(int d, int m, int y) const
{
    if (y < 1900 || y > 2100) return false;//year out of bound
    if (m < 1 || m > 12) return false;//month out of bound 
    if (d < 1 || d > daysInMonth(m, y)) return false; //day out of bound
    return true;
}

bool Date::isLeapYear(int y)
{
    if (y % 400 == 0) return true;
    if (y % 100 == 0) return false;
    if (y % 4 == 0) return true;
    return false;
}

int Date::daysInMonth(int m, int y)
{
    if (m == 2)
    {
        if (isLeapYear(y)) return 29;
        else return 28;
    }
    if (m == 4 || m == 6 || m == 9 || m == 11)
    {
        return 30;
    }
    return 31;
}

Date::Date() : day(-1), month(-1), year(-1) //initial day
{
}

bool Date::isEmpty() const
{
    return (day == -1 && month == -1 && year == -1);
}

Date::Date(int d, int m, int y)
{
    if (!isValidDate(d, m, y))
    {
        throw std::invalid_argument("[Date] The date is not valid");
    }
    day = d;
    month = m;
    year = y;
}

Date::Date(const string& dateString) //constructor from string
{
    int d = 1, m = 1, y = 2000;
    
    try 
    {
        size_t pos1 = dateString.find('/');
        char sep = '/';
        if (pos1 == string::npos)
        {
            pos1 = dateString.find('-'); //if not / then its -
            sep = '-';
        }
        if (pos1 == string::npos)
        {
            throw std::invalid_argument("[Date] Error parsing");
        }
        
        size_t pos2 = dateString.find(sep, pos1 + 1); //find from last pos to end
        if (pos2 == string::npos || dateString.find_first_of("/-", pos2 + 1) != string::npos)
        {
            throw std::invalid_argument("[Date] Error parsing");
        }
        
        string part1 = dateString.substr(0, pos1);
        string part2 = dateString.substr(pos1 + 1, pos2 - pos1 - 1);
        string part3 = dateString.substr(pos2 + 1);
        if (part1.empty() || part2.empty() || part3.empty())
        {
            throw std::invalid_argument("[Date] Error parsing");
        }

        size_t idx1 = 0, idx2 = 0, idx3 = 0;
        int v1 = stoi(part1, &idx1);
        int v2 = stoi(part2, &idx2);
        int v3 = stoi(part3, &idx3);
        if (idx1 != part1.length() || idx2 != part2.length() || idx3 != part3.length())
        {
            throw std::invalid_argument("[Date] Error parsing");
        }

        //split string
        if (pos1 == 4) 
        { 
            // case yyyy-mm-dd
            y = v1;
            m = v2;
            d = v3;
        } 
        else 
        { 
            // case dd/mm/yyyy
            d = v1;
            m = v2;
            y = v3;
        }
    } 
    catch (...) 
    {
        throw std::invalid_argument("[Date] Error parsing");
    }

    if (!isValidDate(d, m, y))
    {
        throw std::invalid_argument("[Date] Invalid Date");
    }
    
    day = d;
    month = m;
    year = y;
}

int Date::getDay() const
{
    return day;
}

int Date::getMonth() const
{
    return month;
}

int Date::getYear() const
{
    return year;
}

string Date::toString() const
{
    string dStr = (day < 10) ? "0" + to_string(day) : to_string(day);
    string mStr = (month < 10) ? "0" + to_string(month) : to_string(month);
    return dStr + "/" + mStr + "/" + to_string(year);
}

string Date::toFileString() const
{
    string dStr = (day < 10) ? "0" + to_string(day) : to_string(day);
    string mStr = (month < 10) ? "0" + to_string(month) : to_string(month);
    return to_string(year) + "-" + mStr + "-" + dStr;
}

void Date::addDays(int days)
{
    day += days;
    while (day > daysInMonth(month, year))
    {
        day -= daysInMonth(month, year);
        month++;
        if (month > 12)
        {
            month = 1;
            year++;
        }
    }
}

void Date::addMonths(int months)
{
    month += months;
    while (month > 12)
    {
        month -= 12;
        year++;
    }
    
    int maxDays = daysInMonth(month, year);
    if (day > maxDays)
    {
        day = maxDays;
    }
}

//operators 
bool Date::operator==(const Date& other) const
{
    return year == other.year && month == other.month && day == other.day;
}

bool Date::operator!=(const Date& other) const
{
    return !(*this == other);
}

bool Date::operator<(const Date& other) const
{
    if (year != other.year) return year < other.year;
    if (month != other.month) return month < other.month;
    return day < other.day;
}

bool Date::operator<=(const Date& other) const
{
    return *this < other || *this == other;
}

bool Date::operator>(const Date& other) const
{
    return !(*this <= other);
}

bool Date::operator>=(const Date& other) const
{
    return !(*this < other);
}
