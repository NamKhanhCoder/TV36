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

class PackageMenu : public BaseMenu
{
private:
    Repository<Package> repo;
    Repository<Channel> repoChannel;
    Repository<NoiDungVOD> repoVOD;
    Repository<Contract> repoContract;

    void createPackage();
    void readPackage();
    void sortPackage();
    void updatePackage();
    void deletePackage();

    PackageStatus inputPackageStatus(bool isOptional = false, PackageStatus currentStatus = PackageStatus::DANG_CUNG_CAP);
    bool isPackageInActiveContract(const string &packageID);
    vector<string> inputChannelIDs(bool isOptional = false, const vector<string> &currentIDs = {});
    vector<string> inputVodIDs(bool isOptional = false, const vector<string> &currentIDs = {});

protected:
    void printMenu() override;
    void handleChoice(int choice) override;

public:
    PackageMenu();
};
