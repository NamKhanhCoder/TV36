#include "Validator.h"
#include <stdexcept> //for invalid arg and overflow (int and double check)
#include <regex>     //for email checking
#include "Utils/Debugger.h"

bool Validator::isValidInt(const string &input)
{
    if (input.empty()) // blank number case
    {
        LOG("[Validator] Invalid, Integer is blank");
        return false;
    }
    // block leading zero for positive numbers (e.g. "0123")
    if (input.length() > 1 && input[0] == '0')
    {
        LOG("[Validator] Invalid, Integer has leading zero");
        return false;
    }
    // block leading zero for negative numbers (e.g. "-0123" or "-0")
    if (input.length() > 2 && input[0] == '-' && input[1] == '0')
    {
        LOG("[Validator] Invalid, Negative integer has leading zero");
        return false;
    }
    try
    {
        // detect chars appearances in number
        size_t pos;
        std::stoi(input, &pos);
        if (pos == input.length())
        {
            LOG("[Validator] Valid, Integer accepted");
            return true;
        }
        else
        {
            LOG("[Validator] Invalid, Integer illegal characters found");
            return false;
        }
    }
    catch (const std::invalid_argument &) // in case the stoi failed
    {
        LOG("[Validator] Invalid, Integer failed to convert");
        return false;
    }
    catch (const std::out_of_range &) // overflow case
    {
        LOG("[Validator] Invalid, Integer overflow");
        return false;
    }
}

bool Validator::isValidDouble(const string &input)
{
    if (input.empty())
    {
        LOG("[Validator] Invalid, Double is empty");
        return false;
    }
    if (input.length() > 1 && input[0] == '0' && input[1] != '.')
    {
        LOG("[Validator] Invalid, Double has leading zero");
        return false;
    }
    if (input.length() > 2 && input[0] == '-' && input[1] == '0' && input[2] != '.')
    {
        LOG("[Validator] Invalid, Negative double has leading zero");
        return false;
    }
    try
    {
        size_t pos;
        std::stod(input, &pos);
        if (pos == input.length())
        {
            LOG("[Validator] Valid, Double accepted");
            return true;
        }
        else
        {
            LOG("[Validator] Invalid, Double illegal characters found");
            return false;
        }
    }
    catch (const std::invalid_argument &)
    {
        LOG("[Validator] Invalid, Double convert failed");
        return false;
    }
    catch (const std::out_of_range &)
    {
        LOG("[Validator] Invalid, Double overflow");
        return false;
    }
}
bool Validator::isValidID(const string &input, const string &prefix, short length)
{
    if (input.length() != length) // length mismatch case
    {
        LOG("[Validator] Invalid, ID length mismatch");
        return false;
    }

    if (prefix != input.substr(0, prefix.length())) // prefix mismatch case
    {
        LOG("[Validator] Invalid, ID prefix mismatch");
        return false;
    }
    for (int i = prefix.length(); i < length; i++) // not a number case
    {
        if (input[i] < '0' || input[i] > '9')
        {
            LOG("[Validator] Invalid, ID illegal characters found");
            return false;
        }
    }
    LOG("[Validator] Valid, ID is accepted");
    return true;
}
bool Validator::isValidPhone(const string &input, short length) // by default the length is 10
{
    if (input.length() != length)
    {
        LOG("[Validator] Invalid, Phone length mismatch");
        return false;
    }

    if (input[0] != '0')
    {
        LOG("[Validator] Invalid, Phone must start with '0'");
        return false;
    }

    for (int i = 0; i < length; i++)
    {
        if (input[i] < '0' || input[i] > '9')
        {
            LOG("[Validator] Invalid, Phone illegal characters found");
            return false;
        }
    }
    LOG("[Validator] Valid, Phone is accepted");
    return true;
}
bool Validator::isValidEmail(const string &input)
{
    if (input.empty())
    {
        LOG("[Validator] Invalid, Email is empty");
        return false;
    }
        
    // pattern matching using regex, the \\ is especially for cpp of the one backslash
    const regex email_pattern("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    if(regex_match(input, email_pattern))
    {
        LOG("[Validator] Valid, Email is accepted");
        return true;
    }
    else
    {
        LOG("[Validator] Invalid, Email pattern mismatch");
        return false;
    }
}