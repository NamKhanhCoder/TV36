// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Khac Hieu (Thanh vien 4)
// Use Case: Quan ly Noi dung VOD (Video On Demand)
// ===============================================================
#include "NoiDungVOD.h"
#include <iostream>
#include <sstream>
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
    trangThai = "";
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
    if (namPhatHanh >= 1950 && namPhatHanh <= 2026)
    {
        this->namPhatHanh = namPhatHanh;
    }
    else
        throw invalid_argument("[VOD] the release year of the data is not valid: " + to_string(namPhatHanh));
}
void NoiDungVOD::setUrl(string url)
{
        this->url = url;
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
// Chuyen doi doi tuong thanh chuoi dinh dang luu vao file
string NoiDungVOD::exportToString() const
{
    return id + "|" + tenNoiDung + "|" + theLoai + "|" + to_string(thoiLuong) + "|" + moTa + "|" + to_string(namPhatHanh) + "|" + url + "|" + trangThai;
}
// Doc va phan tich chuoi tu file de nap vao doi tuong
void NoiDungVOD::parseFromString(const string &data)
{
    vector<string> words = splitString(data);
    if (words.size() != 8)
    {
        throw invalid_argument("[VOD] The line of data is not valid: " + data);
    }
    setID(words[0]);
    setTenNoiDung(words[1]);
    setTheLoai(words[2]);

    try
    {
        setThoiLuong(stoi(words[3]));
        setNamPhatHanh(stoi(words[5]));
    }
    catch(const exception &)
    {
        throw invalid_argument("[VOD] Duration or Release Year is not a valid number in: " + data);
    }
    setMoTa(words[4]);
    setUrl(words[6]);
    setTrangThai(words[7]);
}
// Xuat cac thuoc tinh thanh vector string de hien thi bang TablePrinter
vector<string> NoiDungVOD::exportToVector() const
{
    return {id, tenNoiDung, theLoai, to_string(thoiLuong), moTa, to_string(namPhatHanh), url, trangThai};
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
