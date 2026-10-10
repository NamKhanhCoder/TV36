#include "HoaDon.h"
#include <iostream>
#include <sstream>
using namespace std;
HoaDon::HoaDon()
{
    id = "";
    maHopDong = "";
    maKhachHang = "";
    soTien = 0.0;
    trangThai = "chua_thanh_toan";
}
string HoaDon::getMaHopDong() const
{
    return maHopDong;
}
string HoaDon::getMaKhachHang() const
{
    return maKhachHang;
}
double HoaDon::getSoTien() const
{
    return soTien;
}
Date HoaDon::getNgayLap() const
{
    return ngayLap;
}
Date HoaDon::getHanTraTien() const
{
    return hanTraTien;
}
Date HoaDon::getNgayTraTien() const
{
    return ngayTraTien;
}
string HoaDon::getTrangThai() const
{
    return trangThai;
}
void HoaDon::setID(string id)
{
    this->id = id;
}
void HoaDon::setMaHopDong(string maHopDong)
{
    this->maHopDong = maHopDong;
}
void HoaDon::setMaKhachHang(string maKhachHang)
{
    this->maKhachHang = maKhachHang;
}
void HoaDon::setSoTien(double soTien)
{
    this->soTien = soTien;
}
void HoaDon::setNgayLap(Date ngayLap)
{
    this->ngayLap = ngayLap;
}
void HoaDon::setHanTraTien(Date hanTraTien)
{
    this->hanTraTien = hanTraTien;
}
void HoaDon::setNgayTraTien(Date ngayTraTien)
{
    this->ngayTraTien = ngayTraTien;
}
void HoaDon::setTrangThai(string trangThai)
{
    this->trangThai = trangThai;
}
string HoaDon::exportToString() const
{   
    string strNgayTra;
    if(ngayTraTien.isEmpty()){
        strNgayTra = "NONE";
    }
    else{
        strNgayTra = ngayTraTien.toString();
    }
    return id + "|" + maHopDong + "|" + maKhachHang + "|" + to_string((long long)soTien) + "|" + ngayLap.toString() + "|" + hanTraTien.toString() + "|" + strNgayTra + "|" + trangThai;
}
void HoaDon::parseFromString(const string &data)
{
    stringstream ss(data);
    string token;
    getline(ss, token, '|');
    this->id = token;
    getline(ss, token, '|');
    this->maHopDong = token;
    getline(ss, token, '|');
    this->maKhachHang = token;
    getline(ss, token, '|');
    this->soTien = stod(token);
    getline(ss, token, '|');
    this->ngayLap = Date(token);
    getline(ss, token, '|');
    this->hanTraTien = Date(token);
    getline(ss, token, '|');
    if (token == "NONE")
    {
        this->ngayTraTien = Date();
    }
    else{
    this->ngayTraTien = Date(token);
    }
    getline(ss, token);
    this->trangThai = token;
}
vector<string> HoaDon::exportToVector() const
{
    string strNgayTra;
    if(ngayTraTien.isEmpty()){
        strNgayTra = "NONE";
    }
    else{
        strNgayTra = ngayTraTien.toString();
    }
    return {id, maHopDong, maKhachHang, to_string((long long)soTien), ngayLap.toString(), hanTraTien.toString(), strNgayTra, trangThai};
}
bool HoaDon::operator<(const HoaDon &other) const
{
    return soTien < other.soTien;
}
bool HoaDon::operator>(const HoaDon &other) const
{
    return soTien > other.soTien;
}
