
// The sole purpose of this file, is to prevent the natural stupidity of customers
#include<string>
using namespace std;

class InputHelper
{
private:
public:
    //static is for not having to create a helper variable then call it
    //rather, just call the function
    //usually used when a func dont need the internal variable of the class

    static int getInputInt(string display_message);//deny other datatypes and empty
    static string getInputString(string);//deny other datatypes and empty
    static double getInputDouble(string display_message);//deny other datatypes and empty
    static string getInputOptionalString(string display_message);////deny other datatypes, allow empty
    static string normalizePhone(string phone);//normalize phone number, format any +84 to 0 (Vietnamese only, so theres no other +0..)
};