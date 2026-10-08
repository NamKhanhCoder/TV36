#include "SetTopBox.h"
#include "Utils/StringHelper.h"

using namespace std;

SetTopBox::SetTopBox(string id, string serial, string manufacturer, string model, Date importDate, TechnicalStatus technicalStatus)
    : Entity(id)
{
    this->serial = serial;
    this->manufacturer = manufacturer;
    this->model = model;
    this->importDate = importDate;
    this->technicalStatus = technicalStatus;
}

void SetTopBox::setSerial(string &serial)
{
    this->serial = serial;
}

void SetTopBox::setManufacturer(string &manufacturer)
{
    this->manufacturer = manufacturer;
}

void SetTopBox::setModel(string &model)
{
    this->model = model;
}

void SetTopBox::setImportDate(Date importDate)
{
    this->importDate = importDate;
}

void SetTopBox::setTechnicalStatus(TechnicalStatus status)
{
    this->technicalStatus = status;
}

string SetTopBox::getSerial() const
{
    return serial;
}

string SetTopBox::getManufacturer() const
{
    return manufacturer;
}

string SetTopBox::getModel() const
{
    return model;
}

Date SetTopBox::getImportDate() const
{
    return importDate;
}

TechnicalStatus SetTopBox::getTechnicalStatus() const
{
    return technicalStatus;
}

string SetTopBox::getTechnicalStatusString() const
{
    switch (technicalStatus)
    {
    case TOT:
        return "TOT";
    case HONG:
        return "HONG";
    case BAO_TRI:
        return "BAO_TRI";
    case NGUNG_SU_DUNG:
        return "NGUNG_SU_DUNG";
    default:
        return "UNKNOWN";
    }
}

TechnicalStatus SetTopBox::parseTechnicalStatus(const string &statusStr)
{
    if (statusStr == "TOT")
        return TOT;
    if (statusStr == "HONG")
        return HONG;
    if (statusStr == "BAO_TRI")
        return BAO_TRI;
    if (statusStr == "NGUNG_SU_DUNG")
        return NGUNG_SU_DUNG;
    return TOT; // default
}

string SetTopBox::exportToString() const
{
    return id + "|" + serial + "|" + manufacturer + "|" + model + "|" + importDate.toFileString() + "|" + getTechnicalStatusString();
}

vector<string> SetTopBox::exportToVector() const
{
    vector<string> result = {
        id,
        serial,
        manufacturer,
        model,
        importDate.toString(),
        getTechnicalStatusString()
    };
    return result;
}

void SetTopBox::parseFromString(const string &data)
{
    vector<string> words = splitString(data);

    if (words.size() >= 6)
    {
        id = words[0];
        serial = words[1];
        manufacturer = words[2];
        model = words[3];
        importDate = Date(words[4]);
        technicalStatus = parseTechnicalStatus(words[5]);
    }
}
