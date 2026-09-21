#pragma once
#include <string>
using namespace std;

class InputHelper
{
public:
    static int getInputInt(const string& message = "");
    static double getInputDouble(const string& message = "");
    static string getInputString(const string& message = "");
    static string getInputOptionalString(const string& message = "");

    static string getInputEmail(const string& message = "");
    static string getInputPhone(const string& message = "");
};