#include "Contract.h"
#include "Utils/StringHelper.h"

// ETC --------------------------------------------------
string ContractStatus_toString(ContractStatus status)
{
    switch (status)
    {
    case ContractStatus::ACTIVE:
        return "ACTIVE";
    case ContractStatus::SUSPENDED:
        return "SUSPENDED";
    case ContractStatus::EXPIRED:
        return "EXPIRED";
    default:
        return "UNKNOWN";
    }
}

ContractStatus stringToStatus(const string &s)
{
    if (s == "SUSPENDED") return ContractStatus::SUSPENDED;
    if (s == "EXPIRED") return ContractStatus::EXPIRED;
    return ContractStatus::ACTIVE;
}

// INIT --------------------------------------------------
Contract::Contract(string id, string customerID, string packageID, string stbID, Date signDate, Date startDate, Date endDate, string installationAddress, string monthlyFee, ContractStatus status) : Entity(id)
{
    this->customerID = customerID;
    this->packageID = packageID;
    this->stbID = stbID;
    this->signDate = signDate;
    this->startDate = startDate;
    this->endDate = endDate;
    this->installationAddress = installationAddress;
    this->monthlyFee = monthlyFee;
    this->status = status;
}

// SET --------------------------------------------------
void Contract::set_customerID(string &customerID)
{
    this->customerID = customerID;
}
void Contract::set_packageID(string &packageID)
{
    this->packageID = packageID;
}
void Contract::set_stbID(string &stbID)
{
    this->stbID = stbID;
}
void Contract::set_signDate(Date &signDate)
{
    this->signDate = signDate;
}
void Contract::set_startDate(Date &startDate)
{
    this->startDate = startDate;
}
void Contract::set_endDate(Date &endDate)
{
    this->endDate = endDate;
}
void Contract::set_installationAddress(string &installationAddress)
{
    this->installationAddress = installationAddress;
}
void Contract::set_monthlyFee(string &monthlyFee)
{
    this->monthlyFee = monthlyFee;
}
void Contract::set_status(ContractStatus &status)
{
    this->status = status;
}

// GET --------------------------------------------------
string Contract::get_customerID() const
{
    return customerID;
}
string Contract::get_packageID() const
{
    return packageID;
}
string Contract::get_stbID() const
{
    return stbID;
}
Date Contract::get_signDate() const
{
    return signDate;
}
Date Contract::get_startDate() const
{
    return startDate;
}
Date Contract::get_endDate() const
{
    return endDate;
}
string Contract::get_installationAddress() const
{
    return installationAddress;
}
string Contract::get_monthlyFee() const
{
    return monthlyFee;
}
ContractStatus Contract::get_status() const
{
    return status;
}

//OPS
string Contract::exportToString() const
{
    return id + "|" + customerID + "|" + packageID + "|" + stbID + "|" 
         + signDate.toString() + "|" + startDate.toString() + "|" + endDate.toString() + "|" 
         + installationAddress + "|" + monthlyFee + "|" + ContractStatus_toString(status);
}
vector<string> Contract::exportToVector() const
{
    vector<string> result = 
    {
        id,
        customerID,
        packageID,
        stbID,
        signDate.toString(),
        startDate.toString(),
        endDate.toString(),
        installationAddress,
        monthlyFee,
        ContractStatus_toString(status)
    };
    return result;
}
void Contract::parseFromString(const string &data)
{
    vector<string> words;
    words=splitString(data);

    if (words.size() >= 10) 
    {
        id                      = words[0];
        customerID              = words[1];
        packageID               = words[2];
        stbID                   = words[3];
        signDate                = words[4];
        startDate               = words[5];
        endDate                 = words[6];
        installationAddress     = words[7];
        monthlyFee              = words[8];
        status                  = stringToStatus(words[9]);
    }
}
