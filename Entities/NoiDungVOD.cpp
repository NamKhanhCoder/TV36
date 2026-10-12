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
// Chuyen doi doi tuong thanh chuoi dinh dang luu vao file
string NoiDungVOD::exportToString() const
{
    return id + "|" + tenNoiDung + "|" + theLoai + "|" + to_string(thoiLuong) + "|" + moTa + "|" + to_string(namPhatHanh) + "|" + url + "|" + trangThai;
}
// Doc va phan tich chuoi tu file de nap vao doi tuong
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
