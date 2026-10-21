// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Hong Anh (NotaSpookyGhost)
// Use Case 5: Cai dat du lieu va kiem tra hop le cua Schedule
// ===============================================================
#include "Schedule.h"
#include "Utils/StringHelper.h"
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include "Utils/Date.h"
#include "Utils/Time.h"

namespace
{
    void checkText(const string& value, const string& field, bool optional = false)
    {
        if (value.find_first_of("|\r\n") != string::npos)
            throw invalid_argument(field + " must not contain | or line breaks.");
        if (!optional && (value.empty() || all_of(value.begin(), value.end(),
            [](unsigned char c) { return isspace(c); })))
            throw invalid_argument(field + " must not be empty.");
    }

    void checkID(const string& value, const string& prefix)
    {
        if (value.size() < prefix.size() + 3 || value.compare(0, prefix.size(), prefix) != 0 ||
            !all_of(value.begin() + prefix.size(), value.end(),
                [](unsigned char c) { return c >= '0' && c <= '9'; }))
            throw invalid_argument("Invalid ID. Expected " + prefix + " followed by at least 3 digits.");
    }
}

Schedule::Schedule() : Entity() {}

Schedule::Schedule(const string& id, const string& channelID, const string& programName, const string& startTime, const string& endTime, const string& description) : Entity(id)
{
    checkID(id, "LPS");
    setChannelID(channelID);
    setProgramName(programName);
    setDescription(description);
    setTimeRange(startTime, endTime);
}

string Schedule::getChannelID() const { return channelID; }

void Schedule::setChannelID(const string& value)
{
    checkText(value, "channelID");
    checkID(value, "KENH");
    channelID = value;
}

string Schedule::getProgramName() const { return programName; }

void Schedule::setProgramName(const string& value)
{
    checkText(value, "programName");
    programName = value;
}

string Schedule::getStartTime() const { return startTime; }

void Schedule::setStartTime(const string& value)
{
    string normalized = normalizeDateTime(value);
    if (!endTime.empty() && normalized >= endTime)
        throw invalid_argument("End time must be after start time.");
    startTime = normalized;
}

string Schedule::getEndTime() const { return endTime; }

void Schedule::setEndTime(const string& value)
{
    string normalized = normalizeDateTime(value);
    if (!startTime.empty() && normalized <= startTime)
        throw invalid_argument("End time must be after start time.");
    endTime = normalized;
}

string Schedule::getDescription() const { return description; }

void Schedule::setDescription(const string& value)
{
    checkText(value, "description", true);
    description = value;
}

// Chuoi chuan cho phep so sanh truc tiep ca ngay va gio, ke ca qua nua dem.
string Schedule::normalizeDateTime(const string& value)
{
    if (value.size() != 16 || value[4] != '-' || value[7] != '-' ||
        value[10] != ' ' || value[13] != ':')
        throw invalid_argument("Use yyyy-mm-dd HH:mm, for example 2026-10-16 19:00.");
    for (size_t i = 0; i < value.size(); ++i)
    {
        if (i == 4 || i == 7 || i == 10 || i == 13) continue;
        if (value[i] < '0' || value[i] > '9')
            throw invalid_argument("Date and time must contain digits.");
    }
    // Constructor Date/Time kiem tra ngay gio thuc te (nam nhuan, gio 0..23).
    // Date::toFileString() cua nhom la dd-mm-yyyy, nen giu chuoi ISO da kiem tra.
    Date(value.substr(0, 10));
    Time(value.substr(11, 5));
    return value;
}

void Schedule::setTimeRange(const string& start, const string& end)
{
    string newStart = normalizeDateTime(start);
    string newEnd = normalizeDateTime(end);
    if (newEnd <= newStart)
        throw invalid_argument("End time must be after start time.");
    startTime = newStart;
    endTime = newEnd;
}

void Schedule::parseFromString(const string& data)
{
    vector<string> words = splitString(data);
    if (words.size() != 6)
        throw invalid_argument("Schedule row must have 6 fields.");
    // Chi gan sau khi toan bo dong du lieu hop le.
    *this = Schedule(words[0], words[1], words[2], words[3], words[4], words[5]);
}

string Schedule::exportToString() const
{
    return id + "|" + channelID + "|" + programName + "|" + startTime + "|" + endTime + "|" + description;
}

vector<string> Schedule::exportToVector() const
{
    return {id, channelID, programName, startTime, endTime, description};
}
