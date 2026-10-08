#pragma once
#include "Entity.h"
#include <string>
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
    string serial; //Serial không có quy ước chuẩn, thôi cứ cho tùy biến
    string manufacturer;
    string model;
    Date importDate;
    TechnicalStatus technicalStatus;

public:
    SetTopBox() : Entity() {}
    SetTopBox(string id, string serial, string manufacturer, string model, Date importDate, TechnicalStatus technicalStatus);

    void setSerial(string &serial);
    void setManufacturer(string &manufacturer);
    void setModel(string &model);
    void setImportDate(Date importDate);
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
};
