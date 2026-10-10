#pragma once
#include <string>
#include <vector>
#include "Utils/Date.h"
#include "Entity.h"
using namespace std;
class HoaDon : public Entity{
    private:
            string maHopDong;
            string maKhachHang;
            double soTien;
            Date ngayLap;
            Date hanTraTien;
            Date ngayTraTien;
            string trangThai;
    public:
            HoaDon();
            string getMaHopDong() const;
            string getMaKhachHang() const;
            double getSoTien() const;
            Date getNgayLap() const;
            Date getHanTraTien() const;
            Date getNgayTraTien() const;
            string getTrangThai() const; 
            string exportToString() const override;
            void parseFromString(const string &data) override;
            vector<string> exportToVector() const override;
            void setID(string id);
            void setMaHopDong(string maHopDong);
            void setMaKhachHang(string maKhachHang);
            void setSoTien(double soTien);
            void setNgayLap(Date ngayLap);
            void setHanTraTien(Date hanTraTien);
            void setNgayTraTien(Date ngayTraTien);
            void setTrangThai(string trangThai);
            bool operator < (const HoaDon &other) const;
            bool operator > (const HoaDon &other) const;
};
