#pragma once
#include "Entity.h"
#include <string>

class Channel : public Entity
{
private:
    string name;
    string categoryID;
    int channelNumber;
    string resolution;
    string streamURL;
    string status;

public:
    Channel();
    Channel(const string& id, const string& name, const string& categoryID, int channelNumber, const string& resolution, const string& streamURL, const string& status);
    string getName() const;
    void setName(const string& value);
    string getCategoryID() const;
    void setCategoryID(const string& value);
    int getChannelNumber() const;
    void setChannelNumber(int value);
    string getResolution() const;
    void setResolution(const string& value);
    string getStreamURL() const;
    void setStreamURL(const string& value);
    string getStatus() const;
    void setStatus(const string& value);

    void parseFromString(const string& data) override;
    string exportToString() const override;
    vector<string> exportToVector() const override;
};
