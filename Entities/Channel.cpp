#include "Channel.h"
#include "Utils/StringHelper.h"
#include <algorithm>
#include <cctype>
#include <stdexcept>

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

Channel::Channel() : Entity(), channelNumber(0), resolution("HD"), status("DANG_PHAT") {}

Channel::Channel(const string& id, const string& name, const string& categoryID, int channelNumber, const string& resolution, const string& streamURL, const string& status) : Entity(id)
{
    checkID(id, "KENH");
    setName(name);
    setCategoryID(categoryID);
    setChannelNumber(channelNumber);
    setResolution(resolution);
    setStreamURL(streamURL);
    setStatus(status);
}

string Channel::getName() const { return name; }

void Channel::setName(const string& value)
{
    checkText(value, "name");
    name = value;
}

string Channel::getCategoryID() const { return categoryID; }

void Channel::setCategoryID(const string& value)
{
    checkText(value, "categoryID");
    categoryID = value;
}

int Channel::getChannelNumber() const { return channelNumber; }

void Channel::setChannelNumber(int value)
{
    if (value <= 0) throw invalid_argument("Channel number must be greater than 0.");
    channelNumber = value;
}

string Channel::getResolution() const { return resolution; }

void Channel::setResolution(const string& value)
{
    checkText(value, "resolution");
    if (value != "SD" && value != "HD" && value != "FHD" && value != "UHD")
        throw invalid_argument("Resolution must be SD, HD, FHD or UHD.");
    resolution = value;
}

string Channel::getStreamURL() const { return streamURL; }

void Channel::setStreamURL(const string& value)
{
    checkText(value, "streamURL");
    streamURL = value;
}

string Channel::getStatus() const { return status; }

void Channel::setStatus(const string& value)
{
    checkText(value, "status");
    if (value != "DANG_PHAT" && value != "NGUNG_PHAT")
        throw invalid_argument("Status must be DANG_PHAT or NGUNG_PHAT.");
    status = value;
}

void Channel::parseFromString(const string& data)
{
    vector<string> words = splitString(data);
    if (words.size() != 7)
        throw invalid_argument("Channel row must have 7 fields.");
    size_t used = 0;
    int number = stoi(words[3], &used);
    if (used != words[3].size())
        throw invalid_argument("Invalid channel number in file.");
    // Chi gan sau khi toan bo dong du lieu hop le.
    *this = Channel(words[0], words[1], words[2], number, words[4], words[5], words[6]);
}

string Channel::exportToString() const
{
    return id + "|" + name + "|" + categoryID + "|" + to_string(channelNumber) + "|" + resolution + "|" + streamURL + "|" + status;
}

vector<string> Channel::exportToVector() const
{
    return {id, name, categoryID, to_string(channelNumber), resolution, streamURL, status};
}
