#include "InputHelper.h"
#include "Logger.h"
#include <iostream>
using namespace std;

int InputHelper::getInputInt(string display_message)
{
    int input;
    while (true)
    {
        // reads the entered value
        cout << display_message << endl;
        cin >> input;

        if (cin.fail() == true) // error detected
        {
            LOG("[InputHelper] Invalid input, please enter an Int");

            // clean up leftover input
            cin.clear();
            cin.ignore(10000, '\n');
        }
        else
        {
            LOG("[InputHelper] Valid input");

            // clean up leftover input
            cin.ignore(10000, '\n');
            return input;
        }
    }
}

string InputHelper::getInputString(string display_message)
{
    string input;
    while (true)
    {
        cout << display_message << endl;
        getline(cin, input);
        if (input.empty() == false)
        {
            return input;
        }

        LOG("[InputHelper] Input cannot be empty");
    }
}

double InputHelper::getInputDouble(string display_message)
{
    double input;
    while (true)
    {
        cout << display_message << endl;
        cin >> input;

        if (cin.fail() == true)
        {
            LOG("[InputHelper] Invalid input, please enter a Double");
            cin.clear();
            cin.ignore(10000, '\n');
        }
        else
        {
            LOG("[InputHelper] Valid input");
            cin.ignore(10000, '\n');
            return input;
        }
    }
}

string InputHelper::getInputOptionalString(string display_message)
{
    //no constraints
    string input;
    cout << display_message << endl;
    getline(cin, input);
    return input;
}

string InputHelper::normalizePhone(string input)
{
    if(input.length() >=3 && input.substr(0,3) == "+84")//if the length is smaller than 3 substr will throw an exception
    {
        LOG("[InputHelper] Phone normalized");
        return "0"+input.substr(3);
    }
    else
    {
        LOG("[InputHelper] Phone keeped as it is");
        return input;
    }
}