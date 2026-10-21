// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Duong Bao Long (B24DCVT231)
// Use Case 1: Quan ly Goi kenh Package (Menu dieu khien)
// ===============================================================
#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/Package.h"
#include "Entities/Channel.h"
#include "Entities/NoiDungVOD.h"
#include "Entities/Contract.h"

/**
 * @brief Lop menu dieu khien giao dien console cho chuc nang quan ly Goi kenh (Package).
 */
class PackageMenu : public BaseMenu
{
private:
    Repository<Package> repo;
    Repository<Channel> repoChannel;
    Repository<NoiDungVOD> repoVOD;
    Repository<Contract> repoContract;

    /**
     * @brief Chuc nang them moi mot goi kenh.
     */
    void createPackage();

    /**
     * @brief Chuc nang xem danh sach va tim kiem goi kenh theo ma.
     */
    void readPackage();

    /**
     * @brief Chuc nang sap xep danh sach goi kenh theo gia va trang thai.
     */
    void sortPackage();

    /**
     * @brief Chuc nang cap nhat thong tin goi kenh (co kiem tra hop dong dang hoat dong).
     */
    void updatePackage();

    /**
     * @brief Chuc nang xoa goi kenh (kiem tra rang buoc toan ven tham chieu hop dong).
     */
    void deletePackage();

    /**
     * @brief Ho tro nhap lua chon trang thai goi kenh.
     */
    PackageStatus inputPackageStatus(bool isOptional = false, PackageStatus currentStatus = PackageStatus::DANG_CUNG_CAP);

    /**
     * @brief Kiem tra goi kenh co dang duoc su dung boi hop dong ACTIVE hoac SUSPENDED khong.
     * @param packageID Ma goi can kiem tra.
     * @return true neu goi dang nam trong hop dong co hieu luc / tam ngung.
     */
    bool isPackageInActiveContract(const string &packageID);

    /**
     * @brief Nhap va kiem tra danh sach ma kenh hop le tu repository.
     */
    vector<string> inputChannelIDs(bool isOptional = false, const vector<string> &currentIDs = {});

    /**
     * @brief Nhap va kiem tra danh sach ma VOD hop le tu repository.
     */
    vector<string> inputVodIDs(bool isOptional = false, const vector<string> &currentIDs = {});

protected:
    void printMenu() override;
    void handleChoice(int choice) override;

public:
    /**
     * @brief Constructor nap cac repository phu vu nghiep vu Goi kenh.
     */
    PackageMenu();
};
