// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193)
// Use Case 1: Quan ly Khach hang (Customer Menu)
// ===============================================================
#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/Customer.h"
#include "Entities/Contract.h"
#include "Entities/HoaDon.h"

class CustomerMenu : public BaseMenu
{
private:
    Repository<Customer> repoCustomer;
    Repository<Contract> repoContract;
    Repository<HoaDon> repoHoaDon;

    void createCustomer();
    void readCustomer();
    void searchCustomer();
    void sortCustomer();
    void updateCustomer();
    void deleteCustomer();

protected:
    void printMenu() override;
    void handleChoice(int choice) override;

public:
    CustomerMenu();
};