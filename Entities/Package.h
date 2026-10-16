#pragma once
#include "Entity.h"
#include <string>
#include <vector>

using namespace std;

enum class PackageStatus
{
    DANG_CUNG_CAP,
    NGUNG_DANG_KY
};

string packageStatusToString(PackageStatus status);
PackageStatus stringToPackageStatus(const string &str);

class Package : public Entity
{
private:
    string name;
    long long monthlyPrice;
    PackageStatus status;
    vector<string> channelIDs;
    vector<string> vodIDs;

public:
    Package();
    Package(string id, string name, long long monthlyPrice, PackageStatus status, const vector<string> &channelIDs, const vector<string> &vodIDs);

    void setName(const string &name);
    void setMonthlyPrice(long long monthlyPrice);
    void setStatus(PackageStatus status);
    void setChannelIDs(const vector<string> &channelIDs);
    void setVodIDs(const vector<string> &vodIDs);

    string getName() const;
    long long getMonthlyPrice() const;
    PackageStatus getStatus() const;
    string getStatusString() const;
    const vector<string>& getChannelIDs() const;
    const vector<string>& getVodIDs() const;

    string getChannelIDsString() const;
    string getVodIDsString() const;

    string exportToString() const override;
    vector<string> exportToVector() const override;
    void parseFromString(const string &data) override;
};

