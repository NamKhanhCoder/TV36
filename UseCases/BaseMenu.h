#pragma once
#include "Utils/InputHelper.h"
#include "Utils/SystemHelper.h"
class BaseMenu
{
protected:
    virtual void printMenu()=0;
    virtual void handleChoice(int choice)=0;
public:
    virtual ~BaseMenu(){}
    void run()
    {
        while(true)
        {
            printMenu();
            int choice = InputHelper::getInputInt("Select an option (enter 0 to go back): ");
            if(choice == 0)
            {
                return;
            }
            handleChoice(choice);
        }
    }
};