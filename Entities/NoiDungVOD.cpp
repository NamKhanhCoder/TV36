// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Khac Hieu (Thanh vien 4)
// Use Case: Quan ly Noi dung VOD (Video On Demand)
// ===============================================================
#include "NoiDungVOD.h"
#include <iostream>
#include "Utils/Validator.h"
#include "Utils/StringHelper.h"
using namespace std;
// Ham khoi tao mac dinh thiet lap cac gia tri ban dau
NoiDungVOD::NoiDungVOD()
{
    id = "";
    tenNoiDung = "";
    theLoai = "";
    thoiLuong = 0;
    moTa = "";
    namPhatHanh = 0;
    url = "";
    trangThai = "INACTIVE";
    hinhThucXem = "NON_HOURLY";
}
// Cac ham getter lay gia tri thuoc tinh
string NoiDungVOD::getTenNoiDung() const
{
    return tenNoiDung;
}
string NoiDungVOD::getTheLoai() const
{
    return theLoai;
}
int NoiDungVOD::getThoiLuong() const
{
    return thoiLuong;
}
string NoiDungVOD::getMoTa() const
{
    return moTa;
}
int NoiDungVOD::getNamPhatHanh() const
{
    return namPhatHanh;
}
string NoiDungVOD::getUrl() const
{
    return url;
}
string NoiDungVOD::getTrangThai() const
{
    return trangThai;
}
string NoiDungVOD::getHinhThucXem() const
{
    return hinhThucXem;
}
// Cac ham setter gan gia tri cho thuoc tinh
void NoiDungVOD::setID(string id)
{
    if (Validator::isValidID(id, "VOD", 6))
    {
        this->id = id;
    }
    else
        throw invalid_argument("[VOD] the VOD id of the data is not valid: " + id);
}
void NoiDungVOD::setTenNoiDung(string tenNoiDung)
{
    if (!tenNoiDung.empty() && tenNoiDung.find('|') == string::npos)
    {
        this->tenNoiDung = tenNoiDung;
    }
    else
        throw invalid_argument("[VOD] the content name of the data is not valid: " + tenNoiDung);
}
void NoiDungVOD::setTheLoai(string theLoai)
{
    if (!theLoai.empty() && theLoai.find('|') == string::npos)
    {
        this->theLoai = theLoai;
    }
    else
        throw invalid_argument("[VOD] the genre of the data is not valid: " + theLoai);
}
void NoiDungVOD::setThoiLuong(int thoiLuong)
{
    if (thoiLuong > 0)
    {
        this->thoiLuong = thoiLuong;
    }
    else
        throw invalid_argument("[VOD] the duration(min) of the data is not valid: " + to_string(thoiLuong));
}
void NoiDungVOD::setMoTa(string moTa)
{
    if (moTa.find('|') == string::npos)
    {
        this->moTa = moTa;
    }
    else
        throw invalid_argument("[VOD] the description of the data is not valid: " + moTa);
}
void NoiDungVOD::setNamPhatHanh(int namPhatHanh)
{
    if (namPhatHanh >= 1950 && namPhatHanh <= Date::getCurrentDate().getYear())
    {
        this->namPhatHanh = namPhatHanh;
    }
    else
        throw invalid_argument("[VOD] the release year of the data is not valid: " + to_string(namPhatHanh));
}
void NoiDungVOD::setUrl(string url)
{
    if (!url.empty() && url.find('|') == string::npos)
    {
        this->url = url;
    }
    else
    {
        throw invalid_argument("[VOD] The URL of the data is not valid: " + url);
    }
}
void NoiDungVOD::setTrangThai(string trangThai)
{
    if (trangThai == "ACTIVE" || trangThai == "INACTIVE")
    {
        this->trangThai = trangThai;
    }
    else
        throw invalid_argument("[VOD] the status of the data is not valid: " + trangThai);
}
void NoiDungVOD::setHinhThucXem(string hinhThucXem)
{
    if (hinhThucXem == "HOURLY" || hinhThucXem == "NON_HOURLY")
    {
        this->hinhThucXem = hinhThucXem;
    }
    else
        throw invalid_argument("[VOD] the access type of the data is not valid: " + hinhThucXem);
}
// Chuyen doi doi tuong thanh chuoi dinh dang luu vao file
string NoiDungVOD::exportToString() const
{
    return id + "|" + tenNoiDung + "|" + theLoai + "|" + to_string(thoiLuong) + "|" + moTa + "|" + to_string(namPhatHanh) + "|" + url + "|" + trangThai + "|" + hinhThucXem;
}
// Doc va phan tich chuoi tu file de nap vao doi tuong
void NoiDungVOD::parseFromString(const string &data)
{
    vector<string> words = splitString(data);
    if (words.size() != 9)
    {
        throw invalid_argument("[VOD] The line of data is not valid: " + data);
    }
    setID(words[0]);
    setTenNoiDung(words[1]);
    setTheLoai(words[2]);

    try
    {
        size_t idx3 = 0;
        size_t idx5 = 0;
        int thoiLuong = stoi(words[3], &idx3);
        int namPhatHanh = stoi(words[5], &idx5);
        if (idx3 != words[3].length())
        {
            throw invalid_argument("[VOD] Invalid characters in duration(minutes): " + words[3]);
        }
        setThoiLuong(thoiLuong);
        if (idx5 != words[5].length())
        {
            throw invalid_argument("[VOD] Invalid characters in release year: " + words[5]);
        }
        setNamPhatHanh(namPhatHanh);
    }
    catch (const exception &e)
    {
        throw invalid_argument(string("[VOD] ") + e.what() + " in: " + data);
    }
    setMoTa(words[4]);
    setUrl(words[6]);
    setTrangThai(words[7]);
    setHinhThucXem(words[8]);
}
// Xuat cac thuoc tinh thanh vector string de hien thi bang TablePrinter
vector<string> NoiDungVOD::exportToVector() const
{
    return {id, tenNoiDung, theLoai, to_string(thoiLuong), moTa, to_string(namPhatHanh), url, trangThai, hinhThucXem};
}
// Nap chong toan tu < so sanh theo ten A-Z
bool NoiDungVOD::operator<(const NoiDungVOD &other) const
{
    return tenNoiDung < other.tenNoiDung;
}
// Nap chong toan tu > so sanh theo ten Z-A
bool NoiDungVOD::operator>(const NoiDungVOD &other) const
{
    return tenNoiDung > other.tenNoiDung;
}
