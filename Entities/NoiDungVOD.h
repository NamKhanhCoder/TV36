// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Khac Hieu (Thanh vien 4)
// Use Case: Quan ly Noi dung VOD (Video On Demand)
// ===============================================================
#pragma once
#include <string>
#include <vector>
#include "Entity.h"
#include "Utils/Date.h"

using namespace std;

/**
 * @brief Lop thuc the NoiDungVOD quan ly danh muc phim va video theo yeu cau.
 * Ke thua lop truu tuong Entity, quan ly cac thuoc tinh ve tieu de, the loai, thoi luong, URL va trang thai.
 */
class NoiDungVOD : public Entity {
private: 
    string tenNoiDung;   ///< Ten tieu de noi dung VOD (khong rong, khong chua '|')
    string theLoai;      ///< The loai noi dung (Hanh dong, Hai, Hoat hinh...)
    int thoiLuong;       ///< Thoi luong phat (tinh theo phut, > 0)
    string moTa;         ///< Tom tat noi dung video (khong chua '|')
    int namPhatHanh;     ///< Nam cong chieu/phat hanh (1950 <= nam <= nam hien tai)
    string url;          ///< Duong dan phat video (khong rong, khong chua '|')
    string trangThai;    ///< Trang thai phat hanh ("ACTIVE" hoac "INACTIVE")

public:
    /**
     * @brief Ham khoi tao mac dinh cua lop NoiDungVOD.
     * Khoi tao cac thuoc tinh so ve 0 va chuoi rong, trang thai mac dinh la "ACTIVE".
     */
    NoiDungVOD();

    // ===============================================================
    // CAC HAM GETTER TRUY XUAT THONG TIN (KHONG THAY DOI TRANG THAI)
    // ===============================================================

    /**
     * @brief Lay ten tieu de noi dung VOD.
     * @return string Ten noi dung.
     */
    string getTenNoiDung() const;

    /**
     * @brief Lay the loai cua noi dung VOD.
     * @return string The loai phim/video.
     */
    string getTheLoai() const;

    /**
     * @brief Lay thoi luong cua video tinh theo phut.
     * @return int Thoi luong (phut).
     */
    int getThoiLuong() const;

    /**
     * @brief Lay mo ta tom tat noi dung.
     * @return string Doan van ban mo ta.
     */
    string getMoTa() const;

    /**
     * @brief Lay nam phat hanh cua noi dung.
     * @return int Nam phat hanh.
     */
    int getNamPhatHanh() const;

    /**
     * @brief Lay duong dan URL phat video.
     * @return string Duong dan URL.
     */
    string getUrl() const;

    /**
     * @brief Lay trang thai phat hanh hien tai cua video.
     * @return string "ACTIVE" hoac "INACTIVE".
     */
    string getTrangThai() const;

    // ===============================================================
    // CAC HAM GHI DE TU ENTITY (SERIALIZATION & HIEN THI BANG)
    // ===============================================================

    /**
     * @brief Chuyen doi doi tuong thanh dong chuoi ngan cach boi '|' de luu vao file database.
     * @return string Chuoi dinh dang luu tru.
     */
    string exportToString() const override;

    /**
     * @brief Phan tich chuoi doc tu file de nap cac thuoc tinh vao doi tuong VOD.
     * Kiem tra vi tri ket thuc stoi(&idx) de loai bo cac chuoi so co hau to chu rac (nhu 45junk).
     * @param data Chuoi dong du lieu doc tu file noidungvod.txt.
     * @throws std::invalid_argument Neu dong du lieu sai dinh dang, sai so truong hoac gia tri khong hop le.
     */
    void parseFromString(const string &data) override;

    /**
     * @brief Xuat cac thuoc tinh cua VOD thanh vector chuoi.
     * @return vector<string> Vector chua 8 cot thong tin phuc vu in bang can cot TablePrinter.
     */
    vector<string> exportToVector() const override;

    // ===============================================================
    // CAC HAM SETTER CAP NHAT THUOC TINH (DEFENSIVE VALIDATION)
    // ===============================================================

    /**
     * @brief Thiet lap ma noi dung VOD.
     * @param id Ma VOD moi (yeu cau bat dau bang VOD, do dai 6).
     * @throws std::invalid_argument Neu ma khong hop le theo chuan Validator.
     */
    void setID(string id);

    /**
     * @brief Thiet lap ten tieu de noi dung.
     * @param tenNoiDung Ten noi dung moi.
     * @throws std::invalid_argument Neu ten rong hoac chua ky tu '|'.
     */
    void setTenNoiDung(string tenNoiDung);

    /**
     * @brief Thiet lap the loai phim.
     * @param theLoai The loai moi.
     * @throws std::invalid_argument Neu the loai rong hoac chua ky tu '|'.
     */
    void setTheLoai(string theLoai);

    /**
     * @brief Thiet lap thoi luong video (phut).
     * @param thoiLuong Thoi luong tinh theo phut.
     * @throws std::invalid_argument Neu thoiLuong <= 0.
     */
    void setThoiLuong(int thoiLuong);

    /**
     * @brief Thiet lap mo ta noi dung.
     * @param moTa Doan van ban mo ta moi.
     * @throws std::invalid_argument Neu moTa chua ky tu phan cach '|'.
     */
    void setMoTa(string moTa);

    /**
     * @brief Thiet lap nam phat hanh.
     * @param namPhatHanh Nam cong chieu phim.
     * @throws std::invalid_argument Neu namPhatHanh < 1950 hoac lon hon nam hien tai cua he thong.
     */
    void setNamPhatHanh(int namPhatHanh);

    /**
     * @brief Thiet lap duong dan URL phat video.
     * @param url Duong dan URL moi.
     * @throws std::invalid_argument Neu url rong hoac chua ky tu phan cach '|'.
     */
    void setUrl(string url);

    /**
     * @brief Thiet lap trang thai phat hanh.
     * @param trangThai Gia tri trang thai ("ACTIVE" hoac "INACTIVE").
     * @throws std::invalid_argument Neu trangThai khong thuoc hai gia tri cho phep.
     */
    void setTrangThai(string trangThai);

    // ===============================================================
    // NAP CHONG TOAN TU SO SANH PHUC VU SAP XEP
    // ===============================================================

    /**
     * @brief Nap chong toan tu < so sanh hai noi dung VOD theo ten tieu de tu A den Z.
     * @param other Doi tuong VOD can so sanh.
     * @return true Neu ten cua doi tuong hien tai nho hon other.
     */
    bool operator < (const NoiDungVOD &other) const;

    /**
     * @brief Nap chong toan tu > so sanh hai noi dung VOD theo ten tieu de tu Z den A.
     * @param other Doi tuong VOD can so sanh.
     * @return true Neu ten cua doi tuong hien tai lon hon other.
     */
    bool operator > (const NoiDungVOD &other) const;
};
