// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Pham Quoc Trung (B24DCVT361)
// Use Case 7: Quan ly Hop dong (Contract)
// ===============================================================
#include "Contract.h"
#include "Utils/StringHelper.h"
#include "Utils/Validator.h"
#include <stdexcept>

// ETC --------------------------------------------------
string ContractStatus_toString(ContractStatus status)
{
    switch (status)
    {
    case ContractStatus::ACTIVE:
        return "ACTIVE";
    case ContractStatus::SUSPENDED:
        return "SUSPENDED";
    case ContractStatus::CANCELED:
        return "CANCELED";
    case ContractStatus::EXPIRED:
        return "EXPIRED";
    default:
        return "UNKNOWN";
    }
}

ContractStatus stringToStatus(const string &s)
{
    if (s == "ACTIVE") return ContractStatus::ACTIVE;
    if (s == "SUSPENDED") return ContractStatus::SUSPENDED;
    if (s == "EXPIRED") return ContractStatus::EXPIRED;
    if (s == "CANCELED") return ContractStatus::CANCELED;
    throw invalid_argument("[Contract] Invalid contract status: " + s);
}

// INIT --------------------------------------------------
Contract::Contract(string id, string customerID, string packageID, string stbID, Date signDate, Date startDate, Date endDate, string installationAddress, string monthlyFee, ContractStatus status) : Entity(id)
{
    set_customerID(customerID);
    set_packageID(packageID);
    set_stbID(stbID);
    set_signDate(signDate);
    set_startDate(startDate);
    set_endDate(endDate);
    set_installationAddress(installationAddress);
    set_monthlyFee(monthlyFee);
    set_status(status);
}

// SET --------------------------------------------------
void Contract::set_customerID(const string &customerID)
{
    if (customerID.empty())
        throw invalid_argument("[Contract] Customer ID can not be empty!");
    if (customerID.find_first_of("|\r\n") != string::npos)
        throw invalid_argument("[Contract] Customer ID cannot contain '|' or newline characters!");
    this->customerID = customerID;
}
void Contract::set_packageID(const string &packageID)
{
    if (packageID.empty())
        throw invalid_argument("[Contract] Package ID can not be empty!");
    if (packageID.find_first_of("|\r\n") != string::npos)
        throw invalid_argument("[Contract] Package ID cannot contain '|' or newline characters!");
    this->packageID = packageID;
}
void Contract::set_stbID(const string &stbID)
{
    if (stbID.empty())
        throw invalid_argument("[Contract] Set-Top Box ID can not be empty!");
    if (stbID.find_first_of("|\r\n") != string::npos)
        throw invalid_argument("[Contract] Set-Top Box ID cannot contain '|' or newline characters!");
    this->stbID = stbID;
}
void Contract::set_signDate(const Date &signDate)
{
    this->signDate = signDate;
}
void Contract::set_startDate(const Date &startDate)
{
    this->startDate = startDate;
}
void Contract::set_endDate(const Date &endDate)
{
    this->endDate = endDate;
}
void Contract::set_installationAddress(const string &installationAddress)
{
    if (installationAddress.empty())
        throw invalid_argument("[Contract] Installation address can not be empty!");
    if (installationAddress.find_first_of("|\r\n") != string::npos)
        throw invalid_argument("[Contract] Installation address cannot contain '|' or newline characters!");
    this->installationAddress = installationAddress;
}
void Contract::set_monthlyFee(const string &monthlyFee)
{
    if (monthlyFee.empty())
        throw invalid_argument("[Contract] Monthly fee can not be empty!");
    if (monthlyFee.find_first_of("|\r\n") != string::npos)
        throw invalid_argument("[Contract] Monthly fee cannot contain '|' or newline characters!");
    this->monthlyFee = monthlyFee;
}
void Contract::set_status(ContractStatus status)
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
    vector<string> words = splitString(data);

    if (words.size() != 10)
    {
        throw invalid_argument("[Contract] Invalid field count in line: " + data);
    }

    if (!Validator::isValidID(words[0], "HD", 5))
    {
        throw invalid_argument("[Contract] Invalid Contract ID: " + words[0]);
    }

    if (!Validator::isValidID(words[1], "KH", 5))
    {
        throw invalid_argument("[Contract] Invalid Customer ID: " + words[1]);
    }

    if (!Validator::isValidID(words[2], "GOI", 6))
    {
        throw invalid_argument("[Contract] Invalid Package ID: " + words[2]);
    }

    if (!Validator::isValidID(words[3], "STB", 6))
    {
        throw invalid_argument("[Contract] Invalid Set-Top Box ID: " + words[3]);
    }

    Date parsedSignDate(words[4]);
    Date parsedStartDate(words[5]);
    Date parsedEndDate(words[6]);

    if (parsedStartDate < parsedSignDate)
    {
        throw invalid_argument("[Contract] Start date cannot be before sign date: " + words[5] + " < " + words[4]);
    }

    if (parsedEndDate <= parsedStartDate)
    {
        throw invalid_argument("[Contract] End date must be after start date: " + words[6] + " <= " + words[5]);
    }

    if (words[7].empty())
    {
        throw invalid_argument("[Contract] Installation address cannot be empty");
    }

    if (!Validator::isValidInt(words[8]) || stoll(words[8]) <= 0)
    {
        throw invalid_argument("[Contract] Invalid monthly fee: " + words[8]);
    }

    string stStr = words[9];
    if (!stStr.empty() && stStr.back() == '\r')
        stStr.pop_back();

    ContractStatus parsedStatus = stringToStatus(stStr);

    this->id = words[0];
    this->customerID = words[1];
    this->packageID = words[2];
    this->stbID = words[3];
    this->signDate = parsedSignDate;
    this->startDate = parsedStartDate;
    this->endDate = parsedEndDate;
    this->installationAddress = words[7];
    this->monthlyFee = words[8];
    this->status = parsedStatus;
}

bool Contract::operator<(const Contract &other) const
{
    if (this->startDate != other.startDate)
        return this->startDate < other.startDate;
    return this->id < other.id;
}
