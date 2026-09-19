
// this one is the valid check
#pragma once
#include<string>
#include<iostream>
using namespace std;
class Validator
{
public:
//note I will need very much information on how far we need the validator to check, especially the email
    static bool isValidEmail(string);//check for @ and . (. is after @)
    static bool isValidPhone(string);//check if phone starts at 0, and the length is 10, please call InputHelper::normalizePhone before checking validity
    static bool isValidID(string input, string prefix, int length);//check for prefix match and length match in input
};