// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193)
// Use Case 2: Quan ly Thiet bi Set-top Box (STB)
// ===============================================================
#pragma once
#include "Entity.h"
#include <string>
#include <vector>
#include "Utils/Date.h"

using namespace std;

enum TechnicalStatus
{
    TOT,
    HONG,
    BAO_TRI,
    NGUNG_SU_DUNG
};

class SetTopBox : public Entity
{
private:
    string serial;
    string manufacturer;
    string model;
    Date importDate;
    TechnicalStatus technicalStatus;

public:
    SetTopBox() : Entity() {}
    SetTopBox(string id, string serial, string manufacturer, string model, Date importDate, TechnicalStatus technicalStatus);

    void setSerial(const string &serial);
    void setManufacturer(const string &manufacturer);
    void setModel(const string &model);
    void setImportDate(const Date &importDate);
    void setTechnicalStatus(TechnicalStatus status);

    string getSerial() const;
    string getManufacturer() const;
    string getModel() const;
    Date getImportDate() const;
    TechnicalStatus getTechnicalStatus() const;

    string getTechnicalStatusString() const;
    static TechnicalStatus parseTechnicalStatus(const string &statusStr);

    string exportToString() const override;
    vector<string> exportToVector() const override;

    void parseFromString(const string &data) override;

    // Operator Overloading cho tieu chi OOP
    bool operator<(const SetTopBox &other) const;
    bool operator>(const SetTopBox &other) const;
    bool operator==(const SetTopBox &other) const;
};
