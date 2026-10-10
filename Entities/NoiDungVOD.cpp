#include "NoiDungVOD.h"
#include <iostream>
#include <sstream>
using namespace std;
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
void NoiDungVOD::setID(string id)
{
    this->id = id;
}
void NoiDungVOD::setTenNoiDung(string tenNoiDung)
{
    this->tenNoiDung = tenNoiDung;
}
void NoiDungVOD::setTheLoai(string theLoai)
{
    this->theLoai = theLoai;
}
void NoiDungVOD::setThoiLuong(int thoiLuong)
{
    this->thoiLuong = thoiLuong;
}
void NoiDungVOD::setMoTa(string moTa)
{
    this->moTa = moTa;
}
void NoiDungVOD::setNamPhatHanh(int namPhatHanh)
{
    this->namPhatHanh = namPhatHanh;
}
void NoiDungVOD::setUrl(string url)
{
    this->url = url;
}
void NoiDungVOD::setTrangThai(string trangThai)
{
    this->trangThai = trangThai;
}
string NoiDungVOD::exportToString() const
{
    return id + "|" + tenNoiDung + "|" + theLoai + "|" + to_string(thoiLuong) + "|" + moTa + "|" + to_string(namPhatHanh) + "|" + url + "|" + trangThai;
}
void NoiDungVOD::parseFromString(const string &data)
{
    stringstream ss(data);
    string token;
    getline(ss, token, '|');
    this->id = token;
    getline(ss, token, '|');
    this->tenNoiDung = token;
    getline(ss, token, '|');
    this->theLoai = token;
    getline(ss, token, '|');
    this->thoiLuong = stoi(token);
    getline(ss, token, '|');
    this->moTa = token;
    getline(ss, token, '|');
    this->namPhatHanh = stoi(token);
    getline(ss, token, '|');
    this->url = token;
    getline(ss, token);
    this->trangThai = token;
}
vector<string> NoiDungVOD::exportToVector() const
{
    return {id, tenNoiDung, theLoai, to_string(thoiLuong), moTa, to_string(namPhatHanh), url, trangThai};
}
bool NoiDungVOD::operator<(const NoiDungVOD &other) const
{
    return tenNoiDung < other.tenNoiDung;
}
bool NoiDungVOD::operator>(const NoiDungVOD &other) const
{
    return tenNoiDung > other.tenNoiDung;
}
