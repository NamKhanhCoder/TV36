// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Khac Hieu (Thanh vien 4)
// Use Case: Quan ly Noi dung VOD (Video On Demand)
// ===============================================================
#pragma once
#include<string>
#include"Entity.h"
#include "Utils/Date.h"
using namespace std;
// Lop thuc the NoiDungVOD ke thua lop truu tuong Entity
class NoiDungVOD : public Entity{
    private: 
            // Cac thuoc tinh cua noi dung VOD
            string tenNoiDung;
            string theLoai;
            int thoiLuong;
            string moTa;
            int namPhatHanh;
            string url;
            string trangThai;
    public:
            // Ham khoi tao mac dinh
            NoiDungVOD();
            // Cac ham getter lay thong tin
            string getTenNoiDung()const;
            string getTheLoai()const;
            int getThoiLuong()const;
            string getMoTa()const;
            int getNamPhatHanh()const;
            string getUrl() const;
            string getTrangThai() const;
            // Cac ham ghi de tu Entity phuc vu doc ghi file va xuat bang
            string exportToString() const override;
            void parseFromString(const string &data) override;
            vector<string> exportToVector() const override;
            // Cac ham setter cap nhat thong tin
            void setID(string id);
            void setTenNoiDung(string tenNoiDung);
            void setTheLoai(string theLoai);
            void setThoiLuong(int thoiLuong);
            void setMoTa(string moTa);
            void setNamPhatHanh(int namPhatHanh);
            void setUrl(string url);
            void setTrangThai(string trangThai);
            // Nap chong toan tu so sanh theo ten phuc vu sap xep
            bool operator < (const NoiDungVOD &other) const; // để so sánh theo tên ( cho đủ tiêp chí nạp chồng toán tử của btl)
            bool operator > (const NoiDungVOD &other) const;
};