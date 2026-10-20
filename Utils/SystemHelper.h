// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193)
// Tien ich: SystemHelper (Ho tro he thong & xoa man hinh console)
// ===============================================================
#pragma once
#include<iostream>
#include <cstdlib>
using namespace std;
class SystemHelper
{
    public:
    static void clearScreen()
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }
};
