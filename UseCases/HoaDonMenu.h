// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Khac Hieu (Thanh vien 4)
// Use Case 2: Quan ly Hoa don (Menu dieu khien)
// ===============================================================
#pragma once
#include "Utils/SystemHelper.h"
#include "Repositories/Repository.h"
#include "Entities/HoaDon.h"
#include "Entities/Customer.h"
#include "Entities/Contract.h"
#include "UseCases/BaseMenu.h"
#include <vector>
#include <string>
using namespace std;
// Lop dieu khien menu quan ly Hoa don ke thua BaseMenu
class HoaDonMenu : public BaseMenu
{
private:
    // Cac repository quan ly hoa don va tham chieu du lieu khach hang (TV1), hop dong (TV2)
    Repository<HoaDon> repoHoaDon;
    Repository<Customer> repoCustomer;
    Repository<Contract> repoContract;
public:
    // Ham khoi tao nap du lieu hoa don, khach hang va hop dong tu file
    HoaDonMenu();
    // Chuc nang xem danh sach hoa don va tong doanh thu
    void xemDanhSach();
    // Chuc nang them hoa don moi co kiem tra ton tai khach hang va hop dong
    void themHoaDon();
    // Chuc nang tim kiem hoa don theo nhieu tieu chi
    void timKiem();
    // Chuc nang sap xep hoa don (so tien, ngay lap, han tra, ngay tra) va luu file
    void sapXep();
    // Chuc nang cap nhat thong tin hoa don
    void capNhat();
    // Chuc nang xoa hoa don theo ma
    void xoaHoaDon();
    // Hien thi menu quan ly hoa don
    void printMenu() override;
    // Xu ly lua chon chuc nang menu
    void handleChoice(int choice) override;
    // Kiem tra ton tai ma khach hang tu repoCustomer (TV1)
    bool kiemTraKhachHangTonTai(string maKhachHang)
    {   
        return repoCustomer.readByID(maKhachHang) != nullptr;
    }
    // Kiem tra ton tai ma hop dong tu repoContract (TV2)
    bool kiemTraHopDongTonTai(string maHopDong)
    {
        return repoContract.readByID(maHopDong) != nullptr;
    }
};
