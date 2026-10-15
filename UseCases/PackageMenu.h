#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/Package.h"

class PackageMenu : public BaseMenu
{
private:
    Repository<Package> repo;
    void createPackage();
    void readPackage();
    void updatePackage();
    void deletePackage();

    PackageStatus inputPackageStatus(bool isOptional = false, PackageStatus currentStatus = PackageStatus::DANG_CUNG_CAP);

protected:
    void printMenu() override;
    void handleChoice(int choice) override;

public:
    PackageMenu();
};

