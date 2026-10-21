// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Duong Bao Long (B24DCVT231)
// Use Case 1: Quan ly Goi kenh Package 
// ===============================================================
#pragma once
#include "Entity.h"
#include <string>
#include <vector>

using namespace std;

/**
 * @brief Enum bieu dien trang thai cua goi kenh dich vu.
 */
enum class PackageStatus
{
    DANG_CUNG_CAP, ///< Goi kenh dang mo dang ky moi
    NGUNG_DANG_KY  ///< Goi kenh da dong, chi giu phuc vu hop dong cu
};

/**
 * @brief Chuyen enum PackageStatus sang chuoi ky tu.
 * @param status Trang thai goi.
 * @return Chuoi "DANG_CUNG_CAP" hoac "NGUNG_DANG_KY".
 */
string packageStatusToString(PackageStatus status);

/**
 * @brief Chuyen chuoi ky tu sang enum PackageStatus.
 * @param str Chuoi bieu dien trang thai.
 * @return Gia tri enum PackageStatus tuong ung.
 * @throw std::invalid_argument neu chuoi khong hop le.
 */
PackageStatus stringToPackageStatus(const string &str);

/**
 * @brief Lop thuc the quan ly thong tin Goi kenh truyen hinh.
 */
class Package : public Entity
{
private:
    string name;                ///< Ten goi dich vu
    long long monthlyPrice;     ///< Gia cuoc niem yet hang thang (VND)
    PackageStatus status;       ///< Trang thai kinh doanh
    vector<string> channelIDs;  ///< Danh sach ma kenh thuoc goi (> 1 kenh, khong trung lap)
    vector<string> vodIDs;      ///< Danh sach ma noi dung VOD thuoc goi (> 1 VOD, khong trung lap)

public:
    /**
     * @brief Constructor mac dinh.
     */
    Package();

    /**
     * @brief Constructor day du tham so khoi tao goi kenh.
     * @param id Ma goi (vi du GOI001).
     * @param name Ten goi dich vu.
     * @param monthlyPrice Gia cuoc thang (> 0).
     * @param status Trang thai goi.
     * @param channelIDs Danh sach ma kenh (> 1 kenh, khac nhau).
     * @param vodIDs Danh sach ma VOD (> 1 VOD, khac nhau).
     */
    Package(string id, string name, long long monthlyPrice, PackageStatus status, const vector<string> &channelIDs, const vector<string> &vodIDs);

    /**
     * @brief Thiet lap ten goi.
     * @param name Ten goi (khong rong, khong chua ky tu cam '|', xuong dong).
     */
    void setName(const string &name);

    /**
     * @brief Thiet lap gia cuoc thang.
     * @param monthlyPrice Gia cuoc (> 0 VND).
     */
    void setMonthlyPrice(long long monthlyPrice);

    /**
     * @brief Thiet lap trang thai goi kenh.
     * @param status Trang thai (DANG_CUNG_CAP / NGUNG_DANG_KY).
     */
    void setStatus(PackageStatus status);

    /**
     * @brief Thiet lap danh sach kenh thuoc goi.
     * @param channelIDs Vector chua ma cac kenh (> 1 kenh, khong trung lap).
     */
    void setChannelIDs(const vector<string> &channelIDs);

    /**
     * @brief Thiet lap danh sach VOD thuoc goi.
     * @param vodIDs Vector chua ma cac noi dung VOD (> 1 VOD, khong trung lap).
     */
    void setVodIDs(const vector<string> &vodIDs);

    string getName() const;
    long long getMonthlyPrice() const;
    PackageStatus getStatus() const;
    string getStatusString() const;
    const vector<string>& getChannelIDs() const;
    const vector<string>& getVodIDs() const;

    string getChannelIDsString() const;
    string getVodIDsString() const;

    /**
     * @brief Xuat thong tin goi kenh thanh dong van ban luu file.
     * @return Chuoi dinh dang id|name|monthlyPrice|status|channelIDs|vodIDs.
     */
    string exportToString() const override;

    /**
     * @brief Xuat thong tin thanh mang chuoi phuc vu TablePrinter.
     * @return Vector chua cac truong thong tin can cot.
     */
    vector<string> exportToVector() const override;

    /**
     * @brief Phan tich dong du lieu doc tu file va nap vao doi tuong.
     * @param data Dong du lieu doc tu file packages.txt.
     */
    void parseFromString(const string &data) override;

    // Nạp chồng toán tử so sánh (Rubric OOP)
    bool operator<(const Package &other) const;
    bool operator>(const Package &other) const;
    bool operator==(const Package &other) const;
};
