// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Khac Hieu (Thanh vien 4)
// Use Case 2: Quan ly Hoa don
// ===============================================================
#include "HoaDon.h"
#include <iostream>
#include <cmath>
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
    // chong so vo han ( inf / -inf ) hoac ko phai so (NaN)
    if (!std::isfinite(soTien))
    {
        throw invalid_argument("[INVOICE] Amount must be a finite number: " + to_string(soTien));
    }
    if (soTien <= 0.0)
    {
        throw invalid_argument("[INVOICE] the amount of the data is not valid: " + to_string(soTien));
    }
    // tien te VND la dong nguyen , ko chap nhan phan le thap phan ( chong mat tien 0.5)
    if (std::floor(soTien) != soTien)
    {
        throw invalid_argument("[INVOICE] Amount cannot contain decimal fractions (VND must be integer): " + to_string(soTien));
    }
    // gioi han can tren la 10 ty VND
    if (soTien > 10000000000.0)
    {
        throw invalid_argument("[INVOICE] Amount exceeds maximum allowed limit: " + to_string(soTien));
    }
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
        size_t idx = 0;
        // &idx là con tro nhan dia chi cua idx
        // nhiem vu cua idx: sau khi stod chay xong , no se ghi vao idx so luon ki tu ma no thuc su doc (vi du nhap 1000junk thi stod chi doc duoc 1000, nen idx se la 4)
        double soTien = stod(words[3], &idx);
        // neu chuoi co ki tu la (1000junk) -> nem loi ngay
        if (idx != words[3].length())
        {
            throw invalid_argument("[INVOICE] Invalid characters in amount: " + words[3]);
        }
        setSoTien(soTien);
    }
    catch (const exception &)
    {
        throw invalid_argument("[INVOICE] Amount is not a valid number in: " + data);
    }
    try
    {
        Date ngayLap = words[4];
        Date hanTraTien = words[5];
        if (ngayLap > hanTraTien)
        {
            throw invalid_argument("[INVOICE] Due date cannot be earlier than issuance date in: " + data);
        }
        setNgayLap(ngayLap);
        setHanTraTien(hanTraTien);
    }
    catch (const exception &)
    {
        throw invalid_argument("[INVOICE] Date of issuance or due date is not a valid date in: " + data);
    }
    Date ngayTraTien;
    if (words[6] == "NONE")
    {
        ngayTraTien = Date();
    }
    else
    {
        ngayTraTien = Date(words[6]);
    }
    string trangThai = words[7];
    if (trangThai == "Paid")
    {
        if (ngayTraTien.isEmpty())
        {
            throw invalid_argument("[INVOICE] Paid invoice must have a payment date (cannot be NONE) in: " + data);
        }
        if (ngayTraTien < ngayLap)
        {
            throw invalid_argument("[INVOICE] Payment date cannot be earlier than issuance date in: " + data);
        }
    }
    else if (trangThai == "Unpaid")
    {
        if (!ngayTraTien.isEmpty())
        {
            throw invalid_argument("[INVOICE] Unpaid invoice must have NONE payment date in: " + data);
        }
    }
    setNgayTraTien(ngayTraTien);
    setTrangThai(trangThai);
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
