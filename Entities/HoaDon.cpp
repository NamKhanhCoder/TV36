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
#include "Utils/Validator.h"
#include "Utils/StringHelper.h"
using namespace std;
// Ham khoi tao mac dinh thiet lap gia tri ban dau
HoaDon::HoaDon()
{
    id = "";
    maHopDong = "";
    maKhachHang = "";
    soTien = 0.0;
    trangThai = "Unpaid";
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
    if (Validator::isValidID(id, "HDN", 6))
    {
        this->id = id;
    }
    else
        throw invalid_argument("[INVOICE] the invoice id of the data is not valid: " + id);
}
void HoaDon::setMaHopDong(string maHopDong)
{
    if (Validator::isValidID(maHopDong, "HD", 5))
    {
        this->maHopDong = maHopDong;
    }
    else
        throw invalid_argument("[INVOICE] the contract id of the data is not valid: " + maHopDong);
}
void HoaDon::setMaKhachHang(string maKhachHang)
{
    if (Validator::isValidID(maKhachHang, "KH", 5))
    {
        this->maKhachHang = maKhachHang;
    }
    else
        throw invalid_argument("[INVOICE] the customer id of the data is not valid: " + maKhachHang);
}
void HoaDon::setSoTien(double soTien)
{
    if (soTien > 0.0)
    {
        this->soTien = soTien;
    }
    else
        throw invalid_argument("[INVOICE] the amount of the data is not valid: " + to_string(soTien));
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
    if (this->trangThai == "Paid" && trangThai != "Paid")
    {
        throw invalid_argument("Hoa don da thanh toan (Paid), khong the thay doi trang thai!");
    }
    if (trangThai == "Paid" || trangThai == "Unpaid")
    {
        this->trangThai = trangThai;
    }
    else
        throw invalid_argument("[INVOICE] the status of the data is not valid: " + trangThai);
}
// Chuyen doi doi tuong hoa don thanh chuoi ghi vao file (xu ly NONE neu chua thanh toan)
string HoaDon::exportToString() const
{
    string strNgayTra;
    if (ngayTraTien.isEmpty())
    {
        strNgayTra = "NONE";
    }
    else
    {
        strNgayTra = ngayTraTien.toString();
    }
    return id + "|" + maHopDong + "|" + maKhachHang + "|" + to_string((long long)soTien) + "|" + ngayLap.toString() + "|" + hanTraTien.toString() + "|" + strNgayTra + "|" + trangThai;
}
// Doc va phan tich chuoi tu file de nap vao doi tuong hoa don (xu ly NONE cho ngayTraTien)
void HoaDon::parseFromString(const string &data)
{
    vector<string> words = splitString(data);
    if (words.size() != 8)
    {
        throw invalid_argument("[INVOICE] The line of data is not valid: " + data);
    }
    setID(words[0]);
    setMaHopDong(words[1]);
    setMaKhachHang(words[2]);
    try
    {
        setSoTien(stod(words[3]));
    }
    catch (const exception &)
    {
        throw invalid_argument("[INVOICE] Amount is not a valid number in: " + data);
    }
    try
    {
        setNgayLap(words[4]);
        setHanTraTien(words[5]);
    }
    catch (const exception &)
    {
        throw invalid_argument("[INVOICE] Date of issuance or due date is not a valid date in: " + data);
    }
    if (words[6] == "NONE")
    {
        setNgayTraTien(Date());
    }
    else
    {
        try
        {
            setNgayTraTien(words[6]);
        }
        catch (const exception &)
        {
            throw invalid_argument("[INVOICE] Payment date is not a valid date in: " + data);
        }
    }

    setTrangThai(words[7]);
}
// Xuat cac thuoc tinh thanh vector string phuc vu in bang can cot TablePrinter
vector<string> HoaDon::exportToVector() const
{
    string strNgayTra;
    if (ngayTraTien.isEmpty())
    {
        strNgayTra = "NONE";
    }
    else
    {
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
