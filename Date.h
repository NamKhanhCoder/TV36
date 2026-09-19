
// This is for the date file, description can be found
#pragma once //Header guard, used for when you import Date from multiple places, then that places are imported to main, it wont throw an exception (Date defined twice)

#include<string>
using namespace std;
class Date
{
private:
    int day, month, year;
public:
    Date(); // initialize, by default les make it 1/1/2000
    Date(int d, int m, int y); // with initializer, now set the date directly
    Date(string input);//over load, now support input w string

    bool isValid(); //Check the validity of date, month, year

    //comparision
    bool operator<(Date); 
    bool operator>(Date);
    bool operator==(Date);

    string toString();

};