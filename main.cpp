#include <iostream>
#include <iostream>
#include <vector>
#include <string>
#include "InputHelper.h"
#include "Validator.h"
#include "TablePrinter.h"
using namespace std;

string inp;
int main()
{
    while(1)
    {
        cin>>inp;
        TablePrinter::printFrame({2,2,2});
        TablePrinter::printLine({2,2,2},{"Nguyuyakisofbahisdhflsdbfaf","Lmao","Dummy"});
    }
    
    return 0;
}