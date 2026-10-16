#pragma once
#include "Entity.h"
#include <string>

class Schedule : public Entity
{
private:
    string channelID;
    string programName;
    string startTime;
    string endTime;
    string description;

public:
    Schedule();
    Schedule(const string& id, const string& channelID, const string& programName, const string& startTime, const string& endTime, const string& description);
    string getChannelID() const;
    void setChannelID(const string& value);
    string getProgramName() const;
    void setProgramName(const string& value);
    string getStartTime() const;
    void setStartTime(const string& value);
    string getEndTime() const;
    void setEndTime(const string& value);
    string getDescription() const;
    void setDescription(const string& value);
    // Cap nhat ca khoang gio tren ban sao de tranh so sanh voi moc cu.
    void setTimeRange(const string& start, const string& end);
    static string normalizeDateTime(const string& value);

    void parseFromString(const string& data) override;
    string exportToString() const override;
    vector<string> exportToVector() const override;
};
