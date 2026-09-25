#pragma once
#include <string>

using namespace std;

class Time
{
private:
    int hour;
    int minute;

    bool isValidTime(int h, int m) const;

public:
    // Constructors
    Time();
    Time(int h, int m);
    Time(const string& timeStr);

    // Getters
    int getHour() const;
    int getMinute() const;

    // Formatting
    bool isEmpty() const;
    string toString() const;

    // Tính toán thời gian
    void addMinutes(int mins);
    int toTotalMinutes() const;

    // Operator Overloading
    bool operator==(const Time& other) const;
    bool operator!=(const Time& other) const;
    bool operator<(const Time& other) const;
    bool operator<=(const Time& other) const;
    bool operator>(const Time& other) const;
    bool operator>=(const Time& other) const;
};
