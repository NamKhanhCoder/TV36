// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193)
// Use Case 2: Quan ly Thiet bi Set-top Box (STB Menu)
// ===============================================================
#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/SetTopBox.h"
#include "Entities/Contract.h"
#include "Entities/SupportTicket.h"

class SetTopBoxMenu : public BaseMenu
{
private:
    Repository<SetTopBox> repoSetTopBox;
    Repository<Contract> repoContract;
    Repository<SupportTicket> repoSupportTicket;

    void createSetTopBox();
    void readSetTopBox();
    void searchSetTopBox();
    void sortSetTopBox();
    void updateSetTopBox();
    void deleteSetTopBox();

    TechnicalStatus inputTechnicalStatus(bool isOptional = false, TechnicalStatus defaultStatus = TOT);

protected:
    void printMenu() override;
    void handleChoice(int choice) override;

public:
    SetTopBoxMenu();
};
