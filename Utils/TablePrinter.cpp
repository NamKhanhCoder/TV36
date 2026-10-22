// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193)
// Tien ich: TablePrinter (Dinh dang & can cot bang console)
// ===============================================================
#include "TablePrinter.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "Utils/Debugger.h"

using namespace std;

void TablePrinter::printSeparator(const vector<int> &widths) // print +----+----+
{
    // Beside the widths, we pad 2 spaces on the 2 sides of the data, so a width is actually +2
    cout << "+";
    for (size_t i = 0; i < widths.size(); i++)
    {
        cout << setfill('-') << setw(widths[i] + 2) << "" << "+";
    }
    cout << setfill(' ') << "\n";
}

void TablePrinter::printRow(const vector<int> &widths, const vector<string> &row)
{
    if (row.size() != widths.size())
    {
        LOG("[TableHelper] Failed to print row, size of widths and rows mismatch");
        return;
    }
    cout << "|";
    for (size_t i = 0; i < row.size(); i++)
    {
        string cell = formatCell(row[i], widths[i]);
        cout << " " << left << setw(widths[i]) << cell << " |";
    }
    cout << "\n";
}

string TablePrinter::formatCell(const string &data, int width)
{
    if (static_cast<int>(data.length()) > width) // case overflow
    {
        if (width <= 3)
        {
            return string(width, '#');
        }
        else
        {
            return data.substr(0, width - 3) + "..."; // truncate data with ...
        }
    }
    // Can le va dem khoang trang tu dong bang std::setw va std::left trong iomanip
    return data;
}

void TablePrinter::printTable(const vector<int> &widths, const vector<string> &headers, const vector<vector<string>> &data)
{
    if (data.empty())
    {
        LOG("[TablePrinter] The data for table print is empty");
        return;
    }

    // Dam bao do rong cot toi thieu bang do dai tieu de (header)
    vector<int> actualWidths = widths;
    for (size_t i = 0; i < actualWidths.size() && i < headers.size(); i++)
    {
        if (static_cast<int>(headers[i].length()) > actualWidths[i])
        {
            actualWidths[i] = static_cast<int>(headers[i].length());
        }
    }

    printSeparator(actualWidths);
    printRow(actualWidths, headers);
    printSeparator(actualWidths);

    for (size_t i = 0; i < data.size(); i++)
    {
        printRow(actualWidths, data[i]);
    }
    printSeparator(actualWidths);

    cout << "Total: " << data.size() << " record(s)\n";
}

void TablePrinter::printSingle(const vector<int> &widths, const vector<string> &headers, const vector<string> &data)
{
    if (data.empty())
    {
        LOG("[TablePrinter] The data for single print is empty");
        return;
    }

    // Man hinh chi tiet (printSingle): tu dong mo rong do rong cot theo du lieu thuc te
    // giup hien thi day du thong tin (dia chi, mo ta...) ma khong bi cat boi "..."
    vector<int> actualWidths = widths;
    for (size_t i = 0; i < actualWidths.size() && i < headers.size() && i < data.size(); i++)
    {
        int required = max(static_cast<int>(headers[i].length()), static_cast<int>(data[i].length()));
        if (required > actualWidths[i])
        {
            actualWidths[i] = required;
        }
    }

    printSeparator(actualWidths);
    printRow(actualWidths, headers);
    printSeparator(actualWidths);
    printRow(actualWidths, data);
    printSeparator(actualWidths);

    cout << "Total 1 record(s)\n";
}
