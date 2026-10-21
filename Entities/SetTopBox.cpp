// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193)
// Use Case 2: Quan ly Thiet bi Set-top Box (STB)
// ===============================================================
#include "SetTopBox.h"
#include <stdexcept>
#include "Utils/StringHelper.h"

using namespace std;

SetTopBox::SetTopBox(string id, string serial, string manufacturer, string model, Date importDate, TechnicalStatus technicalStatus)
    : Entity(id)
{
    setSerial(serial);
    setManufacturer(manufacturer);
    setModel(model);
    setImportDate(importDate);
    setTechnicalStatus(technicalStatus);
}

void SetTopBox::setSerial(const string &serial)
{
    if (serial.empty())
        throw invalid_argument("[SetTopBox] Serial number cannot be empty!");
    if (serial.find_first_of("|\r\n") != string::npos)
        throw invalid_argument("[SetTopBox] Serial number must not contain '|' or line breaks!");
    this->serial = serial;
}

void SetTopBox::setManufacturer(const string &manufacturer)
{
    if (manufacturer.empty())
        throw invalid_argument("[SetTopBox] Manufacturer cannot be empty!");
    if (manufacturer.find_first_of("|\r\n") != string::npos)
        throw invalid_argument("[SetTopBox] Manufacturer must not contain '|' or line breaks!");
    this->manufacturer = manufacturer;
}

void SetTopBox::setModel(const string &model)
{
    if (model.empty())
        throw invalid_argument("[SetTopBox] Model cannot be empty!");
    if (model.find_first_of("|\r\n") != string::npos)
        throw invalid_argument("[SetTopBox] Model must not contain '|' or line breaks!");
    this->model = model;
}

void SetTopBox::setImportDate(const Date &importDate)
{
    if (importDate.isEmpty())
        throw invalid_argument("[SetTopBox] Import date cannot be empty!");

    Date currentDate = Date::getCurrentDate();

    if (importDate > currentDate)
        throw invalid_argument("[SetTopBox] Import date cannot be in the future!");

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
    string s = statusStr;
    while (!s.empty() && (s.back() == '\r' || s.back() == ' ' || s.back() == '\t'))
        s.pop_back();

    if (s == "TOT")
        return TOT;
    if (s == "HONG")
        return HONG;
    if (s == "BAO_TRI")
        return BAO_TRI;
    if (s == "NGUNG_SU_DUNG")
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
        getTechnicalStatusString()};
    return result;
}

void SetTopBox::parseFromString(const string &data)
{
    vector<string> words = splitString(data);
    if (words.size() != 6)
    {
        throw invalid_argument("[SetTopBox] The line of data is not valid: " + data);
    }
    id = words[0];
    if (Validator::isValidID(id, "STB", 6) == false)
    {
        throw invalid_argument("[SetTopBox] The ID is not valid: " + data);
    }
    setSerial(words[1]);
    setManufacturer(words[2]);
    setModel(words[3]);
    setImportDate(Date(words[4]));
    setTechnicalStatus(parseTechnicalStatus(words[5]));
}

bool SetTopBox::operator<(const SetTopBox &other) const
{
    if (this->importDate != other.importDate)
        return this->importDate < other.importDate;
    return this->id < other.id;
}

bool SetTopBox::operator>(const SetTopBox &other) const
{
    return other < *this;
}

bool SetTopBox::operator==(const SetTopBox &other) const
{
    return this->id == other.id;
}
