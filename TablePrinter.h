// this is for printing datas on table
// it takes in the width of each column and the data, then print em accordingly
// there will be a overflow protection, as in the string will get ... if it exceeds the given width

#pragma once
#include <iostream>
#include <string>
#include<vector>
using namespace std;

class TablePrinter
{
public:
    static void printFrame(vector<int> widths);//will print +-------+----+---+
    static void printLine(vector<int>widths, vector <string> data);//print |Nguyen Van A | sdhfahd|aosjdioajsdo you get the point
};