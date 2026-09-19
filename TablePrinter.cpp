#include "TablePrinter.h"
#include <iomanip>
void TablePrinter::printFrame(vector<int> widths)
{
    cout<<"+";
    for(int i =0;i<widths.size();i++)
    {
        for(int j=0;j<widths[i]+2;j++)//padd 2 spaces on he 2 sides of the data
        {
            cout<<"-";
        }
        cout<<"+";
    }
    cout<<"\n";
    //basically print +---+----+
}

void TablePrinter::printLine(vector<int> widths, vector<string> data)
{
    cout<<"|";
    for(int i=0;i<widths.size();i++)
    {
        string value;
        //data and frame mismatch protection
        if(i>=data.size())
        {
            value ="";
        }
        else 
        {
            value = data[i];
        }
        if (widths[i]<=3)
        {
            value = string(widths[i],'#');
        }
        else if(value.length()>widths[i]-3)//overflow catch, replace the extended with ... so the table wont be shifted
        {
            value = value.substr(0,widths[i]-3) + "...";
        }
        cout<<" "<<left<<setw(widths[i])<<value<<" ";

        cout <<"|";
    }
    cout<<endl;

}