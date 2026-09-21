#include <iostream>
#include "Utils/Validator.h"
#include "Utils/InputHelper.h"
using namespace std;
string inp;

void testValidator(string inp)
{
    Validator::isValidInt(inp);
    Validator::isValidDouble(inp);
    Validator::isValidID(inp, "KH", 5);
    Validator::isValidPhone(inp);
    Validator::isValidEmail(inp);
}
int main()
{
    while (1)
    {
        //test
    }
}