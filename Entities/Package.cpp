// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Duong Bao Long (B24DCVT231)
// Use Case 1: Quan ly Goi kenh Package 
// ===============================================================
#include "Package.h"
#include "Utils/StringHelper.h"
#include "Utils/Validator.h"
#include <sstream>
#include <stdexcept>
#include <set>
#include <cctype>

using namespace std;

namespace {
    string trimStr(const string &s)
    {
        size_t start = 0;
        while (start < s.size() && isspace(static_cast<unsigned char>(s[start])))
        {
            start++;
        }
        size_t end = s.size();
        while (end > start && isspace(static_cast<unsigned char>(s[end - 1])))
        {
            end--;
        }
        return s.substr(start, end - start);
    }
}

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
    string s = trimStr(str);
    if (s == "NGUNG_DANG_KY")
        return PackageStatus::NGUNG_DANG_KY;
    if (s == "DANG_CUNG_CAP")
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
    
    set<string> seen;
    for (const string &cid : channelIDs)
    {
        if (cid.empty() || cid.find_first_of("|\r\n") != string::npos)
            throw invalid_argument("[Package] Ma kenh khong hop le: " + cid);
        if (seen.find(cid) != seen.end())
            throw invalid_argument("[Package] Danh sach kenh khong duoc chua cac ma trung lap: " + cid);
        seen.insert(cid);
    }
    this->channelIDs = channelIDs;
}

void Package::setVodIDs(const vector<string> &vodIDs)
{
    if (vodIDs.size() <= 1)
        throw invalid_argument("[Package] Goi phai co nhieu hon 1 noi dung VOD (> 1 VOD)!");
    
    set<string> seen;
    for (const string &vid : vodIDs)
    {
        if (vid.empty() || vid.find_first_of("|\r\n") != string::npos)
            throw invalid_argument("[Package] Ma VOD khong hop le: " + vid);
        if (seen.find(vid) != seen.end())
            throw invalid_argument("[Package] Danh sach VOD khong duoc chua cac ma trung lap: " + vid);
        seen.insert(vid);
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

    string packageID = trimStr(words[0]);
    // Chan bare-prefix (phai bat dau bang GOI va co it nhat 3 chu so, length >= 6)
    if (packageID.length() < 6 || !Validator::isValidID(packageID, "GOI", static_cast<short>(packageID.length())))
    {
        throw invalid_argument("[Package] Ma goi khong hop le (phai bat dau bang GOI kem it nhat 3 chu so): " + packageID);
    }
    id = packageID;

    setName(words[1]);

    // Chan gia co hau to chu (vd: 120000vnd)
    string priceStr = trimStr(words[2]);
    if (priceStr.empty())
    {
        throw invalid_argument("[Package] Gia cuoc thang khong duoc de trong!");
    }
    for (char c : priceStr)
    {
        if (!isdigit(static_cast<unsigned char>(c)))
        {
            throw invalid_argument("[Package] Gia cuoc thang chua ky tu khong hop le: " + priceStr);
        }
    }
    long long price = 0;
    try {
        price = stoll(priceStr);
    } catch (...) {
        throw invalid_argument("[Package] Dinh dang gia cuoc thang khong hop le: " + priceStr);
    }
    setMonthlyPrice(price);

    setStatus(stringToPackageStatus(words[3]));

    if (words[4].empty())
    {
        throw invalid_argument("[Package] Danh sach kenh khong duoc de trong: " + data);
    }
    vector<string> channels = splitString(words[4], ',');
    for (auto &c : channels) c = trimStr(c);
    setChannelIDs(channels); // setChannelIDs se kiem tra > 1 va khong trung lap

    if (words[5].empty())
    {
        throw invalid_argument("[Package] Danh sach VOD khong duoc de trong: " + data);
    }
    string vodStr = words[5];
    if (!vodStr.empty() && vodStr.back() == '\r')
        vodStr.pop_back();
    vector<string> vods = splitString(vodStr, ',');
    for (auto &v : vods) v = trimStr(v);
    setVodIDs(vods); // setVodIDs se kiem tra > 1 va khong trung lap
}

bool Package::operator<(const Package &other) const
{
    if (this->monthlyPrice != other.monthlyPrice)
        return this->monthlyPrice < other.monthlyPrice;
    return this->id < other.id;
}

bool Package::operator>(const Package &other) const
{
    return other < *this;
}

bool Package::operator==(const Package &other) const
{
    return this->id == other.id;
}
