
// this one is the valid check
#pragma once
#include<string>
using namespace std;
class Validator
{
public:
    static bool isValidEmail(string);//check for @ and . (. is after @)
    static bool isValidPhone(string);//check if phone starts at 0, and the length is 10, please call InputHelper::normalizePhone before checking validity
    static bool isValidID(string input, string prefix, int length);//check for prefix match and length match in input
};