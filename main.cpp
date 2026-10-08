#include <iostream>
#include "UseCases/CustomerMenu.h"
#include "UseCases/ContractMenu.h"
#include "UseCases/ChannelCategoryMenu.h"
#include "UseCases/SetTopBoxMenu.h"
#include "Utils/InputHelper.h"
#include "Utils/SystemHelper.h"

using namespace std;



int main()
{
    while(1)
    {
        SystemHelper::clearScreen();
        cout << "========================================\n";
        cout << "   HE THONG QUAN LY TRUYEN HINH IPTV    \n";
        cout << "========================================\n\n";        
        cout << "1. Customer Menu" << endl;
        cout << "2. Categories Menu" << endl;
        cout << "3. Contract Menu" << endl;
        cout << "4. Set-top Box Menu" << endl;
        
        int choice = InputHelper::getInputInt("Enter your choice (enter 0 to exit): ");
        int escape = 0;

        switch(choice)
        {
            case 1:
            {
                CustomerMenu customerMenu;
                customerMenu.run();
                break;
            }
            case 2:
            {
                ChannelCategoryMenu categoryMenu;
                categoryMenu.run();
                break;
            }
            case 3:
            {
                ContractMenu contractMenu;
                contractMenu.run();
                break;
            }
            case 4:
            {
                SetTopBoxMenu stbMenu;
                stbMenu.run();
                break;
            }
            default:
            {
                escape = 1;
                break;
            } 
        }

        if(escape == 1)
        {
            break;
        }
    }

    cout << "\nCam on ban da su dung he thong!\n";
    
    return 0;
}