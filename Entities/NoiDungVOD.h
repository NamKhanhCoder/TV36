#pragma once
#include<string>
#include"Entity.h"
using namespace std;
class NoiDungVOD : public Entity{
    private: 
            string tenNoiDung;
            string theLoai;
            int thoiLuong;
            string moTa;
            int namPhatHanh;
            string url;
            string trangThai;
    public:
            NoiDungVOD();
            string getTenNoiDung()const;
            string getTheLoai()const;
            int getThoiLuong()const;
            string getMoTa()const;
            int getNamPhatHanh()const;
            string getUrl() const;
            string getTrangThai() const;
            string exportToString() const override;
            void parseFromString(const string &data) override;
            vector<string> exportToVector() const override;
            void setID(string id);
            void setTenNoiDung(string tenNoiDung);
            void setTheLoai(string theLoai);
            void setThoiLuong(int thoiLuong);
            void setMoTa(string moTa);
            void setNamPhatHanh(int namPhatHanh);
            void setUrl(string url);
            void setTrangThai(string trangThai);
            bool operator < (const NoiDungVOD &other) const; // để so sánh theo tên ( cho đủ tiêp chí nạp chồng toán tử của btl)
            bool operator > (const NoiDungVOD &other) const;
};