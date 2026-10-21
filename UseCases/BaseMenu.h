// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193)
// Framework cot loi: BaseMenu (Lop co so dieu khien Menu console)
// ===============================================================
#pragma once
#include <iostream>
#include <exception>
#include "Utils/InputHelper.h"
#include "Utils/SystemHelper.h"

class BaseMenu
{
protected:
    virtual void printMenu() = 0;
    virtual void handleChoice(int choice) = 0;

public:
    virtual ~BaseMenu() {}

    void run()
    {
        while (true)
        {
            printMenu();
            int choice = InputHelper::getInputInt("Select an option (enter 0 to go back): ");
            if (choice == 0)
            {
                return;
            }
            try
            {
                handleChoice(choice);
            }
            catch (const std::exception &e)
            {
                std::cout << "\n[System Error] An exception occurred: " << e.what() << "\n";
                InputHelper::getInputOptionalString("Press Enter to continue...");
            }
        }
    }
};