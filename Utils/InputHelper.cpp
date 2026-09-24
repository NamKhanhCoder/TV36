#include <iostream>
#include "InputHelper.h"
#include "Validator.h"
#include "Utils/Debugger.h"
using namespace std;
string trim_end_spaces(const string &str) // sanitizer (remove spaces of 2 ends)
{
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first)
        return ""; // full space
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}
static string force_lowercase(string& str) {
    for (char &c : str) {
        c = tolower(c);
    }
    return str;
}
string normalizePhone(string& phone)
{
    string clean_phone="";
    for(int i=0;i<phone.size();i++)
    {
        if(phone[i]=='+' || (phone[i]>='0' && phone[i]<='9'))
        {
            clean_phone+=phone[i];
        }
    }
    if(clean_phone.length()>=3 && clean_phone.substr(0,3)=="+84")
    {
        clean_phone="0"+clean_phone.substr(3);
    }
    return clean_phone;
}

int InputHelper::getInputInt(const string &message)
{

    string input;
    while (true)
    {
        cout << message << endl;
        getline(cin, input);
        input = trim_end_spaces(input);
        LOG("[InputHelper] Integer sanitized to: " + input);
        if (Validator::isValidInt(input))
        {
            LOG("[InputHelper] Integer is accepted, returning");
            return stoi(input);
        }
        else
        {
            cout<<"[Error] The entered value is invalid, please try again"<<endl;
            LOG("[InputHelper] Integer is rejected, please try again");
        }
    }
}
double InputHelper::getInputDouble(const string &message)
{
    string input;
    while (true)
    {
        cout << message << endl;
        getline(cin, input);
        input = trim_end_spaces(input);
        LOG("[InputHelper] Double sanitized to: " + input);
        if (Validator::isValidDouble(input))
        {
            LOG("[InputHelper] Double is accepted, returning");
            return stod(input);
        }
        else
        {
            cout<<"[Error] The entered value is invalid, please try again"<<endl;
            LOG("[InputHelper] Double is rejected, please try again");
        }
    }
}
string InputHelper::getInputString(const string &message)
{
    string input;
    while(true)
    {
        cout<<message<<endl;
        getline(cin,input);
        input = trim_end_spaces(input);
        if(input.length()!=0)//not empty case
        {
            LOG("[InputHelper] String is accepted, returning");
            return input;
        }
        else
        {
            cout<<"[Error] The entered value is invalid, please try again"<<endl;
            LOG("[InputHelper] String is rejected, please try again");
        }
    }
}
string InputHelper::getInputOptionalString(const string &message)
{
    string input;
    cout<<message<<"(Press Enter to skip)"<<endl;
    getline(cin,input);
    input = trim_end_spaces(input);
    LOG("[InputHelper] OptionalString is accepted, returning");
    return input;
}

string InputHelper::getInputEmail(const string &message)
{
    string input;
    while(true)
    {
        cout<<message<<endl;
        getline(cin,input);
        input = trim_end_spaces(input);
        input = force_lowercase(input);
        if(Validator::isValidEmail(input))//not empty case
        {
            LOG("[InputHelper] Email is accepted, returning");
            return input;
        }
        else
        {
            cout<<"[Error] The entered value is invalid, please try again"<<endl;
            LOG("[InputHelper] Email is rejected, please try again");
        }
    }
}
string InputHelper::getInputPhone(const string &message)
{
    string input;
    while(true)
    {
        cout<<message<<endl;
        getline(cin,input);
        input = trim_end_spaces(input);
        input = normalizePhone(input);
        if(Validator::isValidPhone(input))//not empty case
        {
            LOG("[InputHelper] Phone is accepted, returning");
            return input;
        }
        else
        {
            cout<<"[Error] The entered value is invalid, please try again"<<endl;
            LOG("[InputHelper] Phone is rejected, please try again");
        }
    }
}