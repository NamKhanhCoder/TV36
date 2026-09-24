#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/Customer.h"

class CustomerMenu : public BaseMenu
{

private:
    Repository<Customer> repo;
    void createCustomer();
    void readCustomer();
    void updateCustomer();
    void deleteCustomer();
protected:
    void printMenu();
    void handleChoice(int choice);
public:
    CustomerMenu();
};