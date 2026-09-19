#include<iostream>
#include"InputHelper.h"
using namespace std;
int number;
int main()
{
    while(1)
    {
        number = InputHelper::getInputInt("Hello from main, enter some int:");
    }
    return 0;
}