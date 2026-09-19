// This is for the date file, description can be found
#include<string>
using namespace std;
class Date
{
private:
    int day, month, year;
public:
    Date(); // initialize, by default les make it 1/1/2000
    Date(int d, int m, int y); // with initializer, now set the date directly
    Date(string input);

    bool isValid(); //Check the validity of date, month, year
    bool operator<(Date);
    bool operator>(Date);
    bool operator==(Date);

    string toString();

};