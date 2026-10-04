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
