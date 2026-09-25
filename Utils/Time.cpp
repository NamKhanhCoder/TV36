#include "Time.h"
#include <stdexcept>

// Ham kiem tra gio phut co hop le khong
bool Time::isValidTime(int h, int m) const
{
    if (h < 0 || h > 23) return false;
    if (m < 0 || m > 59) return false;
    return true;
}

Time::Time() : hour(-1), minute(-1)
{
}

bool Time::isEmpty() const
{
    return (hour == -1 && minute == -1);
}

Time::Time(int h, int m)
{
    if (!isValidTime(h, m))
    {
        throw std::invalid_argument("Gio phut khong hop le!");
    }
    hour = h;
    minute = m;
}

// Khoi tao tu chuoi (vi du: 14:30)
Time::Time(const string& timeStr)
{
    int h = 0, m = 0;
    
    try 
    {
        // Dung thuat toan cat chuoi co ban de lay gio va phut
        int pos = timeStr.find(':');
        h = stoi(timeStr.substr(0, pos));
        m = stoi(timeStr.substr(pos + 1));
    } 
    catch (...) 
    {
        throw std::invalid_argument("Loi cat chuoi thoi gian!");
    }

    if (!isValidTime(h, m))
    {
        throw std::invalid_argument("Gia tri gio phut khong hop le!");
    }
    
    hour = h;
    minute = m;
}

int Time::getHour() const
{
    return hour;
}

int Time::getMinute() const
{
    return minute;
}

// Xuat ra man hinh dang hh:mm (tu them so 0 neu be hon 10)
string Time::toString() const
{
    string hStr = (hour < 10) ? "0" + to_string(hour) : to_string(hour);
    string mStr = (minute < 10) ? "0" + to_string(minute) : to_string(minute);
    return hStr + ":" + mStr;
}

// Cong them phut vao thoi gian (vi du cong 120 phut)
void Time::addMinutes(int mins)
{
    int totalMins = hour * 60 + minute + mins;
    
    // Xu ly truong hop vuot qua 24 tieng hoac bi am
    totalMins = totalMins % (24 * 60);
    if (totalMins < 0)
    {
        totalMins += 24 * 60;
    }
    
    hour = totalMins / 60;
    minute = totalMins % 60;
}

int Time::toTotalMinutes() const
{
    return hour * 60 + minute;
}

// Cac toan tu so sanh co ban
bool Time::operator==(const Time& other) const
{
    return hour == other.hour && minute == other.minute;
}

bool Time::operator!=(const Time& other) const
{
    return !(*this == other);
}

bool Time::operator<(const Time& other) const
{
    return this->toTotalMinutes() < other.toTotalMinutes();
}

bool Time::operator<=(const Time& other) const
{
    return this->toTotalMinutes() <= other.toTotalMinutes();
}

bool Time::operator>(const Time& other) const
{
    return this->toTotalMinutes() > other.toTotalMinutes();
}

bool Time::operator>=(const Time& other) const
{
    return this->toTotalMinutes() >= other.toTotalMinutes();
}
