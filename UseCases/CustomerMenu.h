#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/Customer.h"
#include "Entities/Contract.h"
class CustomerMenu : public BaseMenu
{

private:
    Repository<Customer> repoCustomer;
    Repository<Contract> repoContract;
    void createCustomer();
    void readCustomer();
    void updateCustomer();
    void deleteCustomer();
protected:
    void printMenu() override;
    void handleChoice(int choice) override;
public:
    CustomerMenu();
};