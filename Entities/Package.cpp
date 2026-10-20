#include "Package.h"
#include "Utils/StringHelper.h"
#include "Utils/Validator.h"
#include <sstream>
#include <stdexcept>

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
    if (str == "DANG_CUNG_CAP")
        return PackageStatus::DANG_CUNG_CAP;
    throw invalid_argument("[Package] Trang thai goi khong hop le (phai la DANG_CUNG_CAP hoac NGUNG_DANG_KY): " + str);
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
    setName(name);
    setMonthlyPrice(monthlyPrice);
    setStatus(status);
    setChannelIDs(channelIDs);
    setVodIDs(vodIDs);
}

void Package::setName(const string &name)
{
    if (name.empty())
        throw invalid_argument("[Package] Ten goi khong duoc de trong!");
    if (name.find_first_of("|\r\n") != string::npos)
        throw invalid_argument("[Package] Ten goi khong duoc chua ky tu '|' hoac xuong dong!");
    this->name = name;
}

void Package::setMonthlyPrice(long long monthlyPrice)
{
    if (monthlyPrice <= 0)
        throw invalid_argument("[Package] Gia cuoc thang phai lon hon 0!");
    this->monthlyPrice = monthlyPrice;
}

void Package::setStatus(PackageStatus status)
{
    this->status = status;
}

void Package::setChannelIDs(const vector<string> &channelIDs)
{
    if (channelIDs.size() <= 1)
        throw invalid_argument("[Package] Goi phai co nhieu hon 1 kenh truyen hinh (> 1 channel)!");
    for (const string &cid : channelIDs)
    {
        if (cid.empty() || cid.find_first_of("|\r\n") != string::npos)
            throw invalid_argument("[Package] Ma kenh khong hop le: " + cid);
    }
    this->channelIDs = channelIDs;
}

void Package::setVodIDs(const vector<string> &vodIDs)
{
    if (vodIDs.size() <= 1)
        throw invalid_argument("[Package] Goi phai co nhieu hon 1 noi dung VOD (> 1 VOD)!");
    for (const string &vid : vodIDs)
    {
        if (vid.empty() || vid.find_first_of("|\r\n") != string::npos)
            throw invalid_argument("[Package] Ma VOD khong hop le: " + vid);
    }
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
    if (words.size() != 6)
    {
        throw invalid_argument("[Package] Dong du lieu goi khong hop le (yeu cau 6 truong): " + data);
    }

    string packageID = words[0];
    if (!Validator::isValidID(packageID, "GOI", static_cast<short>(packageID.length())))
    {
        throw invalid_argument("[Package] Ma goi khong hop le: " + packageID);
    }
    id = packageID;

    setName(words[1]);

    long long price = 0;
    try {
        price = stoll(words[2]);
    } catch (...) {
        throw invalid_argument("[Package] Dinh dang gia cuoc thang khong hop le: " + words[2]);
    }
    setMonthlyPrice(price);

    setStatus(stringToPackageStatus(words[3]));

    if (words[4].empty())
    {
        throw invalid_argument("[Package] Danh sach kenh khong duoc de trong: " + data);
    }
    vector<string> channels = splitString(words[4], ',');
    setChannelIDs(channels);

    if (words[5].empty())
    {
        throw invalid_argument("[Package] Danh sach VOD khong duoc de trong: " + data);
    }
    string vodStr = words[5];
    if (!vodStr.empty() && vodStr.back() == '\r')
        vodStr.pop_back();
    vector<string> vods = splitString(vodStr, ',');
    setVodIDs(vods);
}
