// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Pham Quoc Trung (B24DCVT361)
// Use Case 7: Quan ly Hop dong (Contract)
// ===============================================================
#pragma once
#include "Entity.h"
#include "Utils/Date.h"
#include <string.h>

using namespace std;

enum class ContractStatus
{
    ACTIVE,
    SUSPENDED,
    CANCELED,
    EXPIRED
};

string ContractStatus_toString(ContractStatus status);
ContractStatus stringToStatus(const string &s);


class Contract : public Entity
{
private:
    string customerID;
    string packageID;
    string stbID;
    Date signDate;
    Date startDate;
    Date endDate;
    string installationAddress;
    string monthlyFee;
    ContractStatus status;

public:
    Contract() : Entity() {}
    Contract(string id, string customerID, string packageID, string stbID, Date signDate, Date startDate, Date endDate, string installationAddress, string monthlyFee, ContractStatus status);
    void set_customerID(const string &customerID);
    void set_packageID(const string &packageID);
    void set_stbID(const string &stbID);
    void set_signDate(const Date &signDate);
    void set_startDate(const Date &startDate);
    void set_endDate(const Date &endDate);
    void set_installationAddress(const string &installationAddress);
    void set_monthlyFee(const string &monthlyFee);
    void set_status(ContractStatus status);

    string get_customerID() const;
    string get_packageID() const;
    string get_stbID() const;
    Date get_signDate() const;
    Date get_startDate() const;
    Date get_endDate() const;
    string get_installationAddress() const;
    string get_monthlyFee() const;
    ContractStatus get_status() const;

    string exportToString() const override;
    vector<string> exportToVector() const override;

    void parseFromString(const string &data) override;

    bool operator<(const Contract &other) const;
};