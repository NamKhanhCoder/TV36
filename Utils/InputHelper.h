#pragma once
#include <string>
#include "Utils/Date.h"
#include "Time.h"
using namespace std;

class InputHelper
{
public:
    static int getInputInt(const string &message = "");
    static double getInputDouble(const string &message = "");
    static string getInputString(const string &message = "");
    static string getInputEmail(const string &message = "");
    static string getInputPhone(const string &message = "");
    static Date getInputDate(const string &message = "");
    static Time getInputTime(const string &message = "");

    static int getInputOptionalInt(const string &message = "");
    static double getInputOptionalDouble(const string &message = "");
    static string getInputOptionalString(const string &message = "");
    static string getInputOptionalEmail(const string &message = "");
    static string getInputOptionalPhone(const string &message = "");
    static Date getInputOptionalDate(const string &message = "");
    static Time getInputOptionalTime(const string &message = "");
};