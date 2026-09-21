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
void testInputHelper()
{
    string inp;
    inp = InputHelper::getInputInt("Enter some int:");
    inp = InputHelper::getInputDouble("Enter some double:");
    inp = InputHelper::getInputString("Enter some string (non empty)");
    inp = InputHelper::getInputOptionalString("Enter some string");
    inp = InputHelper::getInputEmail("Enter some email:");
    inp = InputHelper::getInputPhone("Enter some phon:");
}
int main()
{
    while (1)
    {
        // test
    }
}