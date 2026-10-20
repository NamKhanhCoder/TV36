// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193) - Truong nhom
// Chuong trinh chinh (Main Application & Tich hop he thong)
// ===============================================================
#include <iostream>
#include "UseCases/CustomerMenu.h"
#include "UseCases/SetTopBoxMenu.h"
#include "UseCases/ChannelCategoryMenu.h"
#include "UseCases/ChannelMenu.h"
#include "UseCases/ScheduleMenu.h"
#include "UseCases/PackageMenu.h"
#include "UseCases/ContractMenu.h"
#include "UseCases/HoaDonMenu.h"
#include "UseCases/NoiDungVODMenu.h"
#include "UseCases/SupportTicketMenu.h"
#include "Utils/InputHelper.h"
#include "Utils/SystemHelper.h"

using namespace std;

int main()
{
    while (true)
    {
        SystemHelper::clearScreen();
        cout << "========================================================\n";
        cout << "           HE THONG QUAN LY TRUYEN HINH IPTV            \n";
        cout << "========================================================\n\n";
        cout << " 1. Quan ly Khach hang (Customer Menu)\n";
        cout << " 2. Quan ly Thiet bi Set-top Box (STB Menu)\n";
        cout << " 3. Quan ly The loai kenh (Channel Category Menu)\n";
        cout << " 4. Quan ly Kenh truyen hinh (Channel Menu)\n";
        cout << " 5. Quan ly Lich phat song (Broadcast Schedule Menu)\n";
        cout << " 6. Quan ly Goi kenh (Package Menu)\n";
        cout << " 7. Quan ly Hop dong (Contract Menu)\n";
        cout << " 8. Quan ly Hoa don (Invoice Menu)\n";
        cout << " 9. Quan ly Noi dung VOD (VOD Content Menu)\n";
        cout << "10. Quan ly Phieu ho tro ky thuat (Support Ticket Menu)\n";
        cout << "--------------------------------------------------------\n";
        cout << " 0. Thoat chuong trinh\n";
        cout << "========================================================\n";

        int choice = InputHelper::getInputInt("Nhap lua chon cua ban: ");

        switch (choice)
        {
            case 1:
            {
                CustomerMenu customerMenu;
                customerMenu.run();
                break;
            }
            case 2:
            {
                SetTopBoxMenu stbMenu;
                stbMenu.run();
                break;
            }
            case 3:
            {
                ChannelCategoryMenu categoryMenu;
                categoryMenu.run();
                break;
            }
            case 4:
            {
                ChannelMenu channelMenu;
                channelMenu.run();
                break;
            }
            case 5:
            {
                ScheduleMenu scheduleMenu;
                scheduleMenu.run();
                break;
            }
            case 6:
            {
                PackageMenu pkgMenu;
                pkgMenu.run();
                break;
            }
            case 7:
            {
                ContractMenu contractMenu;
                contractMenu.run();
                break;
            }
            case 8:
            {
                HoaDonMenu hoaDonMenu;
                hoaDonMenu.run();
                break;
            }
            case 9:
            {
                NoiDungVODMenu vodMenu;
                vodMenu.run();
                break;
            }
            case 10:
            {
                SupportTicketMenu ticketMenu;
                ticketMenu.run();
                break;
            }
            case 0:
            {
                cout << "\nCam on ban da su dung he thong! Hen gap lai.\n";
                return 0;
            }
            default:
            {
                cout << "\n[Loi] Lua chon khong hop le! Vui long nhap tu 0 den 10.\n";
                InputHelper::getInputOptionalString("Nhan Enter de tiep tuc...");
                break;
            }
        }
    }

    return 0;
}