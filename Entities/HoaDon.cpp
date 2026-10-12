// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Khac Hieu (Thanh vien 4)
// Use Case 2: Quan ly Hoa don
// ===============================================================
#include "HoaDon.h"
#include <iostream>
#include <sstream>
using namespace std;
// Ham khoi tao mac dinh thiet lap gia tri ban dau
HoaDon::HoaDon()
{
    id = "";
    maHopDong = "";
    maKhachHang = "";
    soTien = 0.0;
    trangThai = "chua_thanh_toan";
}
// Cac ham getter lay gia tri thuoc tinh
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
// Cac ham setter gan gia tri cho thuoc tinh
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
// Chuyen doi doi tuong hoa don thanh chuoi ghi vao file (xu ly NONE neu chua thanh toan)
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
// Doc va phan tich chuoi tu file de nap vao doi tuong hoa don (xu ly NONE cho ngayTraTien)
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
// Xuat cac thuoc tinh thanh vector string phuc vu in bang can cot TablePrinter
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
// Nap chong toan tu < so sanh theo so tien tang dan
bool HoaDon::operator<(const HoaDon &other) const
{
    return soTien < other.soTien;
}
// Nap chong toan tu > so sanh theo so tien giam dan
bool HoaDon::operator>(const HoaDon &other) const
{
    return soTien > other.soTien;
}
