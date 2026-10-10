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
class HoaDonMenu : public BaseMenu
{
private:
    Repository<HoaDon> repoHoaDon;
    Repository<Customer> repoCustomer;
    Repository<Contract> repoContract;
public:
    HoaDonMenu();
    void xemDanhSach();
    void themHoaDon();
    void timKiem();
    void sapXep();
    void capNhat();
    void xoaHoaDon();
    void printMenu() override;
    void handleChoice(int choice) override;
    bool kiemTraKhachHangTonTai(string maKhachHang)
    {   
        return repoCustomer.readByID(maKhachHang) != nullptr;
    }
    bool kiemTraHopDongTonTai(string maHopDong)
    {
        return repoContract.readByID(maHopDong) != nullptr;
    }
};
