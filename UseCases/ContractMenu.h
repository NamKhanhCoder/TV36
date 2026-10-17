#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/Contract.h"
#include "Entities/Customer.h"
#include "Entities/Package.h"
#include "Entities/SetTopBox.h"
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

    // Table display helpers
    void printCustomerTable();
    void printPackageTable();
    void printSetTopBoxTable();
    void printContractTable(const vector<vector<string>> &data);

    // Selection & Validation helpers
    string selectCustomerID();
    string selectPackageID();
    string selectSetTopBoxID(const string &currentContractID = "");
    bool isSetTopBoxInUse(const string &stbID, const string &excludeContractID = "");

    Date inputValidStartDate(const Date &signDate);
    Date calculateEndDate(const Date &startDate);

    ContractStatus inputStatus();
    ContractStatus inputOptionalStatus(ContractStatus currentStatus);

    // CRUD operations
    void createContract();
    void readContract();
    void updateContract();
    void deleteContract();

    // Modular Update sub-functions
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