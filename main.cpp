#include <iostream>
#include <iostream>
#include <vector>
#include <string>
#include "TablePrinter.h"
#include "InputHelper.h"

using namespace std;

int main()
{
    cout << "=== TEST TABLE PRINTER ===\n";
    
    // 1. Cấu hình độ rộng cho 3 cột
    vector<int> cot = {7, 20, 15};

    // 2. In bảng
    TablePrinter::inDongKe(cot);
    TablePrinter::inDong({"Ma KH", "Ho va Ten", "So Dien Thoai"}, cot);
    TablePrinter::inDongKe(cot);
    
    TablePrinter::inDong({"KH001", "Nguyen Van An", "0912345678"}, cot);
    TablePrinter::inDong({"KH002", "Le Thi Bich Ngoc", "0987654321"}, cot);
    TablePrinter::inDong({"KH003", "Tran C", "0901234567"}, cot);
    
    TablePrinter::inDongKe(cot);

    cout << "\nTest thanh cong! Nhan Enter de thoat...";
    cin.get();
    
    return 0;
}