#include "TablePrinter.h"
#include <iostream>
#include "Utils/Debugger.h"
using namespace std;
void TablePrinter::printSeparator(const vector<int> &widths) // print +----+----+
{
    // beside the widths, we pad 2 spaces on the 2 side of the data, so a width is actually +2
    cout << "+";
    for (int i = 0; i < widths.size(); i++)
    {
        for (int j = 0; j < widths[i]+2; j++)
        {
            cout << "-";
        }
        cout << "+";
    }
    cout << endl;
}
void TablePrinter::printRow(const vector<int> &widths, const vector<string> &row)
{
    if (row.size() != widths.size())
    {
        LOG("[TableHelper] Failed to print row, size of widths and rows mismatch");
        return;
    }
    cout << "|";
    for (int i = 0; i < row.size(); i++)
    {
        cout << " " << formatCell(row[i], widths[i]) << " |";
    }
    cout << endl;
}
string TablePrinter::formatCell(const string &data, int width)
{
    string result = "";
    if (data.length() > width) // case overflow
    {
        result = data.substr(0, width - 3) + "...";//truncate data with...
    }
    else
    {
        result = data + string(width - data.length(), ' ');
    }
    return result;
}

void TablePrinter::printTable(const vector<int> &widths, const vector<string> &headers, const vector<vector<string>> &data)
{
    if (data.empty())
    {
        LOG("[TablePrinter] The data for table print is empty");
        return;
    }
    printSeparator(widths);
    printRow(widths, headers);
    printSeparator(widths);

    for (int i = 0; i < data.size(); i++)
    {
        printRow(widths, data[i]);
    }
    printSeparator(widths);

    cout << "Total: " << data.size() << " record(s)\n";
}
void TablePrinter::printSingle(const vector<int> &widths, const vector<string> &headers, const vector<string> &data)
{
    if (data.empty())
    {
        LOG("[TablePrinter] The data for single print is empty");
        return;
    }
    printSeparator(widths);
    printRow(widths, headers);
    printSeparator(widths);
    printRow(widths, data);
    printSeparator(widths);

    cout << "Total 1 record(s)\n";
}