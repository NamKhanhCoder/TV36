// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Pham Quoc Trung (B24DCVT361)
// Use Case 7: Quan ly Hop dong (Contract Menu)
// ===============================================================
#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/Contract.h"
#include "Entities/Customer.h"
#include "Entities/Package.h"
#include "Entities/SetTopBox.h"
#include "Entities/HoaDon.h"
#include "Entities/SupportTicket.h"
#include <string>
#include <vector>

using namespace std;

class ContractMenu : public BaseMenu
{
private:
    Repository<Contract> repo;
    Repository<Customer> repoCustomer;
    Repository<Package> repoPackage;
    Repository<SetTopBox> repoSetTopBox;
    Repository<HoaDon> repoHoaDon;
    Repository<SupportTicket> repoSupportTicket;

    void printCustomerTable();
    void printPackageTable();
    void printSetTopBoxTable();
    void printContractTable(const vector<vector<string>> &data);

    string selectCustomerID();
    string selectPackageID();
    string selectSetTopBoxID(const string &currentContractID = "");
    bool isSetTopBoxInUse(const string &stbID, const string &excludeContractID = "");

    Date inputValidStartDate(const Date &signDate);
    Date calculateEndDate(const Date &startDate);

    ContractStatus inputStatus();
    ContractStatus inputOptionalStatus(ContractStatus currentStatus);

    void createContract();
    void readContract();
    void sortContracts();
    void updateContract();
    void deleteContract();

    void updatePackage(Contract *contract);
    void updateSetTopBox(Contract *contract);
    void extendContract(Contract *contract);
    void updateAddress(Contract *contract);
    void updateStatus(Contract *contract);

protected:
    void printMenu() override;
    void handleChoice(int choice) override;

public:
    ContractMenu();
};