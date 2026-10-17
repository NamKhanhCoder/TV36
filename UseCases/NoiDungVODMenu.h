// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Khac Hieu (Thanh vien 4)
// Use Case 1: Quan ly Noi dung VOD (Menu dieu khien)
// ===============================================================
#pragma once
#include "Utils/SystemHelper.h"
#include "Repositories/Repository.h"
#include "Entities/NoiDungVOD.h"
#include "UseCases/BaseMenu.h"
// Tham chieu thuc the Package de kiem tra rang buoc khoa ngoai truoc khi xoa
#include "Entities/Package.h"
#include <vector>
#include <string>
using namespace std;
// Lop dieu khien menu quan ly Noi dung VOD ke thua BaseMenu
class NoiDungVODMenu : public BaseMenu{
    private:
            // Cac repository quan ly noi dung VOD va tham chieu du lieu goi cuoc (Package)
            Repository<NoiDungVOD> repo;
            Repository<Package> repopackage;
    public:
            // Ham khoi tao menu va nap du lieu tu file
            NoiDungVODMenu();
            // Chuc nang xem toan bo danh sach noi dung VOD
            void xemDanhSach();
            // Chuc nang them noi dung VOD moi
            void themNoiDung();
            // Chuc nang tim kiem noi dung VOD theo nhieu tieu chi
            void timKiem();
            // Chuc nang sap xep noi dung VOD va cap nhat file
            void sapXep();
            // Chuc nang cap nhat thong tin noi dung VOD theo ma
            void capNhat();
            // Chuc nang xoa noi dung VOD theo ma
            void xoaNoiDung();
            // Hien thi menu lua chon cac chuc nang VOD
            void printMenu() override;
            // Xu ly lua chon chuc nang tu nguoi dung
            void handleChoice(int choice) override;
};
