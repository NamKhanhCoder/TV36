#include "Validator.h"
#include "Logger.h"
bool Validator::isValidID(string input, string prefix, int length)
{
    if (input.length() != length)
    {
        LOG("[Validator] ID length mismatch");
        return false; // length mismatch case
    }

    if (input.substr(0, prefix.length()) != prefix)
    {
        LOG("[Validator] ID prefix mismatch");
        return false; // prefix mismatch case
    }

    for (int i = prefix.length(); i < input.length(); i++) // not a number ID case
    {
        if (input[i] < '0' || input[i] > '9') // if not a number
        {
            LOG("[Validator] ID contents contains non-numbers");
            return false;
        }
    }
    LOG("[Validator] ID is valid");
    return true;
}

bool Validator::isValidPhone(string input)
{
    // given the phone number is normalized (+84 converted)
    // just have to check for giberish and phone length

    if (input.length() != 10)
    {
        LOG("[Validator] Phone length is invalid, must be 10");
        return false;
    }

    for (int i = 0; i < 10; i++) // not a number phone
    {
        if (input[i] < '0' || input[i] > '9') // if not a number
        {
            LOG("[Validator] Phone contains non-numbers");
            return false;
        }
    }

    if (input[0] != '0')
    {
        LOG("[Validator] Phone starting character is invalid, did you normalize it yet?");
        return false;
    }
    
    LOG("[Validator] Phone is valid");
    return true;
}

bool Validator::isValidEmail(string input)
{
    int pos_at = input.find('@');
    if (pos_at == string::npos) //@ not found case
    {
        LOG("[Validator] Email @ not found");
        return false;
    }

    int pos_dot = input.find('.', pos_at + 1);
    if (pos_dot == string::npos) //. after @ not found case
    {
        LOG("[Validator] Email has no `.` after @");
        return false;
    }

    if (pos_dot == input.length() - 1 || pos_dot == pos_at + 1) // last char is . case and @. case
    {
        LOG("[Validator] Email illegal naming");
        return false;
    }
    LOG("[Validator] Email is valid");
    return true;
}