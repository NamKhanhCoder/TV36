#pragma once
#include <vector>
#include <string>
using namespace std;
class TablePrinter
{
private:
    static void printSeparator(const vector<int> &widths);
    static void printRow(const vector<int> &widths, const vector<string> &row);
    static string formatCell(const string& data, int width);

public:
    static void printTable(const vector<int> &widths, const vector<string> &headers, const vector<vector<string>> &data);
    static void printSingle(const vector<int> &widths, const vector<string> &headers, const vector<string> &data);
};