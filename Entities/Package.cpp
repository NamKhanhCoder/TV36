#include "Package.h"
#include "Utils/StringHelper.h"
#include <sstream>

using namespace std;

string packageStatusToString(PackageStatus status)
{
    switch (status)
    {
    case PackageStatus::DANG_CUNG_CAP:
        return "DANG_CUNG_CAP";
    case PackageStatus::NGUNG_DANG_KY:
        return "NGUNG_DANG_KY";
    default:
        return "DANG_CUNG_CAP";
    }
}

PackageStatus stringToPackageStatus(const string &str)
{
    if (str == "NGUNG_DANG_KY")
        return PackageStatus::NGUNG_DANG_KY;
    return PackageStatus::DANG_CUNG_CAP;
}

Package::Package() : Entity("")
{
    this->name = "";
    this->monthlyPrice = 0;
    this->status = PackageStatus::DANG_CUNG_CAP;
    this->channelIDs = {};
    this->vodIDs = {};
}

Package::Package(string id, string name, long long monthlyPrice, PackageStatus status, const vector<string> &channelIDs, const vector<string> &vodIDs)
    : Entity(id)
{
    this->name = name;
    this->monthlyPrice = monthlyPrice;
    this->status = status;
    this->channelIDs = channelIDs;
    this->vodIDs = vodIDs;
}

void Package::setName(const string &name)
{
    this->name = name;
}

void Package::setMonthlyPrice(long long monthlyPrice)
{
    this->monthlyPrice = monthlyPrice;
}

void Package::setStatus(PackageStatus status)
{
    this->status = status;
}

void Package::setChannelIDs(const vector<string> &channelIDs)
{
    this->channelIDs = channelIDs;
}

void Package::setVodIDs(const vector<string> &vodIDs)
{
    this->vodIDs = vodIDs;
}

string Package::getName() const
{
    return name;
}

long long Package::getMonthlyPrice() const
{
    return monthlyPrice;
}

PackageStatus Package::getStatus() const
{
    return status;
}

string Package::getStatusString() const
{
    return packageStatusToString(status);
}

const vector<string>& Package::getChannelIDs() const
{
    return channelIDs;
}

const vector<string>& Package::getVodIDs() const
{
    return vodIDs;
}

string Package::getChannelIDsString() const
{
    string res = "";
    for (size_t i = 0; i < channelIDs.size(); ++i)
    {
        res += channelIDs[i];
        if (i + 1 < channelIDs.size())
            res += ",";
    }
    return res;
}

string Package::getVodIDsString() const
{
    string res = "";
    for (size_t i = 0; i < vodIDs.size(); ++i)
    {
        res += vodIDs[i];
        if (i + 1 < vodIDs.size())
            res += ",";
    }
    return res;
}

string Package::exportToString() const
{
    // id|name|monthlyprice|status|channelids|vodids
    return id + "|" + name + "|" + to_string(monthlyPrice) + "|" + getStatusString() + "|" + getChannelIDsString() + "|" + getVodIDsString();
}

vector<string> Package::exportToVector() const
{
    return {
        id,
        name,
        to_string(monthlyPrice),
        getStatusString(),
        getChannelIDsString(),
        getVodIDsString()
    };
}

void Package::parseFromString(const string &data)
{
    vector<string> words = splitString(data);
    if (words.size() >= 4)
    {
        id = words[0];
        name = words[1];
        try {
            monthlyPrice = stoll(words[2]);
        } catch (...) {
            monthlyPrice = 0;
        }
        status = stringToPackageStatus(words[3]);

        channelIDs.clear();
        if (words.size() >= 5 && !words[4].empty())
        {
            channelIDs = splitString(words[4], ',');
        }

        vodIDs.clear();
        if (words.size() >= 6 && !words[5].empty())
        {
            vodIDs = splitString(words[5], ',');
        }
    }
}

