#pragma once
#include <string>
using namespace std;
class Validator
{
public:
    static bool isValidInt(const string &input);
    static bool isValidDouble(const string &input);
    static bool isValidID(const string &input, const string &prefix, short length);
    static bool isValidPhone(const string &input, short length = 10);
    static bool isValidEmail(const string &input);
};