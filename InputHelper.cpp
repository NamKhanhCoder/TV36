#include "InputHelper.h"
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
            cout << "[InputHelper]: Invalid input, please enter an Int\n";

            // clean up leftover input
            cin.clear();
            cin.ignore(10000, '\n');
        }
        else
        {
            cout << "[InputHelper]: Valid input\n";

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

        cout << "[InputHelper]: Input cannot be empty\n";
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
            cout << "[InputHelper]: Invalid input, please enter a Double\n";
            cin.clear();
            cin.ignore(10000, '\n');
        }
        else
        {
            cout << "[InputHelper]: Valid input\n";
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