#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/SetTopBox.h"

class SetTopBoxMenu : public BaseMenu
{
private:
    Repository<SetTopBox> repo;
    void createSetTopBox();
    void readSetTopBox();
    void updateSetTopBox();
    void deleteSetTopBox();

    TechnicalStatus inputTechnicalStatus(bool isOptional = false, TechnicalStatus defaultStatus = TOT);

protected:
    void printMenu() override;
    void handleChoice(int choice) override;

public:
    SetTopBoxMenu();
};
