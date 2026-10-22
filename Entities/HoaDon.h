// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Khac Hieu (Thanh vien 4)
// Use Case 2: Quan ly Hoa don
// ===============================================================
#pragma once
#include <string>
#include <vector>
#include "Utils/Date.h"
#include "Entity.h"

using namespace std;

/**
 * @brief Lop thuc the HoaDon quan ly thong tin cuoc phi thanh toan cua khach hang.
 * Ke thua lop truu tuong Entity, bao gom cac rang buoc ve khoa ngoai, so tien, han tra va trang thai.
 */
class HoaDon : public Entity {
private:
    string maHopDong;   ///< Khoa ngoai tham chieu Contract (HDxxx)
    string maKhachHang;  ///< Khoa ngoai tham chieu Customer (KHxxx)
    double soTien;       ///< Cuoc phi hoa don (VND, so nguyen duong huu han)
    Date ngayLap;        ///< Ngay xuat hoa don (<= ngay het han hop dong)
    Date hanTraTien;     ///< Han cuoi thanh toan (>= ngayLap)
    Date ngayTraTien;    ///< Ngay thuc te thanh toan (NONE neu chua thanh toan)
    string trangThai;    ///< Trang thai thanh toan ("Paid" hoac "Unpaid")

public:
    /**
     * @brief Ham khoi tao mac dinh cua lop HoaDon.
     * Khoi tao ngayTraTien rong (NONE) va trangThai mac dinh la "Unpaid".
     */
    HoaDon();

    // ===============================================================
    // CAC HAM GETTER TRUY XUAT THONG TIN (KHONG THAY DOI TRANG THAI)
    // ===============================================================

    /**
     * @brief Lay ma hop dong lien ket voi hoa don.
     * @return string Ma hop dong (HDxxx).
     */
    string getMaHopDong() const;

    /**
     * @brief Lay ma khach hang dai dien cua hop dong.
     * @return string Ma khach hang (KHxxx).
     */
    string getMaKhachHang() const;

    /**
     * @brief Lay so tien cuoc phi hoa don.
     * @return double So tien VND (so nguyen).
     */
    double getSoTien() const;

    /**
     * @brief Lay ngay lap hoa don.
     * @return Date Ngay xuat hoa don.
     */
    Date getNgayLap() const;

    /**
     * @brief Lay han chot thanh toan hoa don.
     * @return Date Han thanh toan.
     */
    Date getHanTraTien() const;

    /**
     * @brief Lay ngay thuc te khach hang thanh toan.
     * @return Date Ngay thanh toan (Date rong neu chua tra).
     */
    Date getNgayTraTien() const;

    /**
     * @brief Lay trang thai thanh toan hien tai.
     * @return string "Paid" neu da tra, "Unpaid" neu chua tra.
     */
    string getTrangThai() const; 

    // ===============================================================
    // CAC HAM GHI DE TU ENTITY (SERIALIZATION & HIEN THI BANG)
    // ===============================================================

    /**
     * @brief Chuyen doi doi tuong hoa don thanh chuoi ghi file, ngan cach boi '|'.
     * Xu ly ngayTraTien thanh "NONE" neu hoa don chua thanh toan.
     * @return string Chuoi dinh dang luu tru co so du lieu.
     */
    string exportToString() const override;

    /**
     * @brief Phan tich chuoi du lieu tu file de nap vao doi tuong hoa don.
     * Kiem tra vi tri ket thuc so tien, rang buoc logic ngay va trang thai cheo.
     * @param data Chuoi dong du lieu doc tu file hoadon.txt.
     * @throws std::invalid_argument Neu dong du lieu sai so cot, sai dinh dang so/ngay hoac quan he mau thuan.
     */
    void parseFromString(const string &data) override;

    /**
     * @brief Xuat cac thuoc tinh cua hoa don thanh vector chuoi.
     * @return vector<string> Vector chua 8 cot thong tin phuc vu in bang can cot TablePrinter.
     */
    vector<string> exportToVector() const override;

    // ===============================================================
    // CAC HAM SETTER CAP NHAT THUOC TINH (DEFENSIVE VALIDATION)
    // ===============================================================

    /**
     * @brief Thiet lap ma hoa don.
     * @param id Ma hoa don moi (yeu cau bat dau bang HDN, do dai 6).
     * @throws std::invalid_argument Neu ma khong hop le theo chuan Validator.
     */
    void setID(string id);

    /**
     * @brief Thiet lap ma hop dong lien ket.
     * @param maHopDong Ma hop dong (HDxxx, do dai 5).
     * @throws std::invalid_argument Neu ma khong hop le theo chuan Validator.
     */
    void setMaHopDong(string maHopDong);

    /**
     * @brief Thiet lap ma khach hang.
     * @param maKhachHang Ma khach hang (KHxxx, do dai 5).
     * @throws std::invalid_argument Neu ma khong hop le theo chuan Validator.
     */
    void setMaKhachHang(string maKhachHang);

    /**
     * @brief Thiet lap so tien cuoc hoa don.
     * @param soTien So tien VND moi.
     * @throws std::invalid_argument Neu soTien khong huu han (inf/NaN), <= 0, chua phan le thap phan hoac vuot qua 10 ty VND.
     */
    void setSoTien(double soTien);

    /**
     * @brief Thiet lap ngay lap hoa don.
     * @param ngayLap Doi tuong Date ngay lap.
     */
    void setNgayLap(Date ngayLap);

    /**
     * @brief Thiet lap han cuoi thanh toan.
     * @param hanTraTien Doi tuong Date han tra.
     */
    void setHanTraTien(Date hanTraTien);

    /**
     * @brief Thiet lap ngay thuc te thanh toan.
     * @param ngayTraTien Doi tuong Date ngay tra (hoac Date rong neu chua tra).
     */
    void setNgayTraTien(Date ngayTraTien);

    /**
     * @brief Thiet lap trang thai thanh toan.
     * @param trangThai Gia tri trang thai ("Paid" hoac "Unpaid").
     * @throws std::invalid_argument Neu trangThai khong hop le hoac co tinh sua tu Paid ve Unpaid.
     */
    void setTrangThai(string trangThai);

    // ===============================================================
    // NAP CHONG TOAN TU SO SANH PHUC VU SAP XEP
    // ===============================================================

    /**
     * @brief Nap chong toan tu < so sanh hai hoa don theo so tien tang dan.
     * @param other Hoa don can so sanh.
     * @return true Neu so tien cua hoa don hien tai nho hon other.
     */
    bool operator < (const HoaDon &other) const;

    /**
     * @brief Nap chong toan tu > so sanh hai hoa don theo so tien giam dan.
     * @param other Hoa don can so sanh.
     * @return true Neu so tien cua hoa don hien tai lon hon other.
     */
    bool operator > (const HoaDon &other) const;
};
