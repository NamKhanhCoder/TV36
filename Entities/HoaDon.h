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
// Lop thuc the HoaDon ke thua lop truu tuong Entity
class HoaDon : public Entity{
    private:
            // Cac thuoc tinh cua hoa don (khoa ngoai maHopDong, maKhachHang va thong tin thanh toan)
            string maHopDong;
            string maKhachHang;
            double soTien;
            Date ngayLap;
            Date hanTraTien;
            Date ngayTraTien;
            string trangThai;
    public:
            // Ham khoi tao mac dinh
            HoaDon();
            // Cac ham getter lay thong tin
            string getMaHopDong() const;
            string getMaKhachHang() const;
            double getSoTien() const;
            Date getNgayLap() const;
            Date getHanTraTien() const;
            Date getNgayTraTien() const;
            string getTrangThai() const; 
            // Cac ham ghi de tu Entity phuc vu doc ghi file va xuat bang
            string exportToString() const override;
            void parseFromString(const string &data) override;
            vector<string> exportToVector() const override;
            // Cac ham setter cap nhat thong tin
            void setID(string id);
            void setMaHopDong(string maHopDong);
            void setMaKhachHang(string maKhachHang);
            void setSoTien(double soTien);
            void setNgayLap(Date ngayLap);
            void setHanTraTien(Date hanTraTien);
            void setNgayTraTien(Date ngayTraTien);
            void setTrangThai(string trangThai);
            // Nap chong toan tu so sanh theo so tien phuc vu sap xep
            bool operator < (const HoaDon &other) const;
            bool operator > (const HoaDon &other) const;
};
