#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/Contract.h"

class ContractMenu : public BaseMenu
{
private:
    Repository<Contract> repo;
    void createContract();
    void readContract();
    void updateContract();
    void deleteContract();

    ContractStatus inputStatus();
    ContractStatus inputOptionalStatus(ContractStatus currentStatus);
protected:
    void printMenu();
    void handleChoice(int choice);
public:
    ContractMenu();
};