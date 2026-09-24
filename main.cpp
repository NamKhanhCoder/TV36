#include <iostream>
#include "UseCases/CustomerMenu.h"

using namespace std;

int main()
{
    cout << "========================================\n";
    cout << "   HE THONG QUAN LY TRUYEN HINH IPTV    \n";
    cout << "========================================\n\n";

    // Khởi tạo menu quản lý khách hàng
    // Quá trình này sẽ tự động gọi repo.loadFromFile() bên trong constructor
    CustomerMenu customerMenu;
    
    // Bắt đầu vòng lặp menu (kế thừa từ BaseMenu)
    customerMenu.run();

    cout << "\nCam on ban da su dung he thong!\n";
    
    return 0;
}