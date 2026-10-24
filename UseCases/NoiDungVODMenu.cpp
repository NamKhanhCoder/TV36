// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Khac Hieu (Thanh vien 4)
// Use Case 1: Quan ly Noi dung VOD (Menu dieu khien)
// ===============================================================
#include "NoiDungVODMenu.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include "Utils/TablePrinter.h"
#include "Utils/InputHelper.h"
#include "Utils/SystemHelper.h"
#include "Utils/StringHelper.h"
using namespace std;
namespace
{
    const vector<int> widths = {8, 34, 11, 16, 43, 14, 38, 12, 14};
    const vector<string> headers = {"VOD ID", "Content Name", "Genre", "Duration (min)", "Description", "Release Year", "Video URL", "Status", "Access type"};
}
// Ham khoi tao: khoi tao 2 repository (NoiDungVOD va Package) va nap du lieu tu file
NoiDungVODMenu::NoiDungVODMenu() : repo("Data/noidungvod.txt", "Data/vod_counter.txt", "VOD"),
                                   repopackage("Data/packages.txt", "Data/packages_counter.txt", "GOI")
{
    repo.loadFromFile();
    repopackage.loadFromFile();
}
// Chuc nang xem danh sach: xuat toan bo ban ghi VOD ra bang can cot TablePrinter
void NoiDungVODMenu::xemDanhSach()
{
    if (repo.readAll().empty())
    {
        cout << "[Error] VOD content list is empty!" << endl;
        return;
    }

    TablePrinter::printTable(widths, headers, repo.exportToVector());
    InputHelper::getInputOptionalString("Press any key to go back: ");
    SystemHelper::clearScreen();
}
// Chuc nang them noi dung moi: nhap thong tin, validate du lieu, check trung URL va ghi file
void NoiDungVODMenu::themNoiDung()
{
    NoiDungVOD noiDungMoi;
    while (true)
    {
        string tenNoiDung = InputHelper::getInputString("Enter content name: ");
        try
        {
            noiDungMoi.setTenNoiDung(tenNoiDung);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[VODMenu] Invalid name input during creation: " + string(e.what()));
        }
    }
    while (true)
    {
        string theLoai = InputHelper::getInputString("Enter content genre: ");
        try
        {
            noiDungMoi.setTheLoai(theLoai);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[VODMenu] Invalid genre input during creation: " + string(e.what()));
        }
    }

    while (true)
    {
        int thoiLuong = InputHelper::getInputInt("Enter duration (minutes): ");
        try
        {
            noiDungMoi.setThoiLuong(thoiLuong);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[VODMenu] Invalid duration(minutes) input during creation: " + string(e.what()));
        }
    }

    while (true)
    {
        string moTa = InputHelper::getInputOptionalString("Enter content description: ");
        try
        {
            noiDungMoi.setMoTa(moTa);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[VODMenu] Invalid description input during creation: " + string(e.what()));
        }
    }

    while (true)
    {
        int namPhatHanh = InputHelper::getInputInt("Enter release year: ");
        try
        {
            noiDungMoi.setNamPhatHanh(namPhatHanh);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[VODMenu] Invalid release year input during creation: " + string(e.what()));
        }
    }
    string url;
    vector<NoiDungVOD> danhSach = repo.readAll();
    int s = danhSach.size();
    while (true)
    {
        int choice = InputHelper::getInputInt("Choose URL format (1. Netflix / 2. TV360): ");
        switch (choice)
        {
        case 1:
        {
            int num = InputHelper::getInputInt("Enter content ID: ");
            url = "https://www.netflix.com/vn/phim" + to_string(num);
            break;
        }
        case 2:
        {
            int num = InputHelper::getInputInt("Enter content ID: ");
            url = "https://www.tv360.com/vn/phim" + to_string(num);
            break;
        }
        default:
            cout << "[Error] Invalid option, try again!" << endl;
            continue;
        }
        bool check = true;
        for (int i = 0; i < s; i++)
        {
            if (url == danhSach[i].getUrl())
            {
                check = false;
                break;
            }
        }
        if (!check)
        {
            cout << "[Error] URL already exists, try again!" << endl;
            continue;
        }
        else
            noiDungMoi.setUrl(url);
        break;
    }
    while (true)
    {
        int choice = InputHelper::getInputInt("Choose content status (1. ACTIVE / 2. INACTIVE): ");
        switch (choice)
        {
        case 1:
        {
            noiDungMoi.setTrangThai("ACTIVE");
            break;
        }
        case 2:
        {
            noiDungMoi.setTrangThai("INACTIVE");
            break;
        }
        default:
            cout << "[Error] Invalid option, try again!" << endl;
            continue;
        }
        break;
    }
    while (true)
    {
        int choice = InputHelper::getInputInt("Choose content access type (1. HOURLY / 2. NON_HOURLY): ");
        switch (choice)
        {
        case 1:
        {
            noiDungMoi.setHinhThucXem("HOURLY");
            break;
        }
        case 2:
        {
            noiDungMoi.setHinhThucXem("NON_HOURLY");
            break;
        }
        default:
            cout << "[Error] Invalid option, try again!" << endl;
            continue;
        }
        break;
    }
    string id = repo.generateNextID();
    noiDungMoi.setID(id);
    if (repo.create(noiDungMoi)) // gọi hàm trong repository đêu phải thông qua repo
    {
        try
        {
            repo.writeToFile();
            cout << "VOD content created successfully!" << endl;
            LOG("[VODMenu] VOD content created successfully, ID: " + id);
        }
        catch (const exception &e)
        {
            cout << "[Error] Failed to save vod content to file: " << e.what() << endl;
            LOG("[VODMenu] File write failed: " + string(e.what()));
            repo.loadFromFile();
            cout << "[Notice] Memory rolled back to last saved state on disk." << endl;
        }
    }
    else
    {
        cout << "[Error] Failed to create vod content (Duplicate ID or counter error)!" << endl;
        LOG("[VODMenu] Failed to create vod content, duplicate ID: " + id);
    }
    InputHelper::getInputOptionalString("Press any key to go back: ");
    SystemHelper::clearScreen();
}
// Chuc nang tim kiem: ho tro tim theo ma VOD, ten, the loai, nam phat hanh, trang thai
void NoiDungVODMenu::timKiem()
{
    cout << "=====Search VOD content=====" << endl;
    cout << "1. Search by VOD ID" << endl;
    cout << "2. Search by content name" << endl;
    cout << "3. Search by genre" << endl;
    cout << "4. Search by release year" << endl;
    cout << "5. Search by status" << endl;
    cout << "6. Search by access type" << endl;
    cout << "Enter any other option to go back." << endl;
    int luaChon = InputHelper::getInputInt("Enter your choice: ");
    repo.loadFromFile();
    const vector<NoiDungVOD> &danhSach = repo.readAll(); // ta chỉ đọc để tìm kiếm  nên  có thể dùng tham chiếu const &
    int s = danhSach.size();
    switch (luaChon)
    {
    case 1:
    { // khiến cho biến string id có thể bị bỏ qua quá trình khởi tạo nên khắc phục bằng thêm {} trc các case rồi mới thi hành lệnh
        string id = InputHelper::getInputString("Enter VOD ID (VODXXX): ");
        id = convertToUpper(id);
        NoiDungVOD *p = repo.readByID(id);
        if (p == nullptr)
        {
            cout << "[Error] VOD content not found, ID: " << id << endl;
            cout << "Try again!" << endl;
            break;
        }
        else
            TablePrinter::printSingle(widths, headers, p->exportToVector()); // Dấu -> có nghĩa là: "Hãy đi đến địa chỉ ô nhớ mà p đang chỉ tới, rồi gọi hàm exportToVector() của đối tượng tại đó"
        break;
    }
    case 2:
    {
        string tuKhoa = InputHelper::getInputString("Enter content name: ");
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            if (danhSach[i].getTenNoiDung().find(tuKhoa) != string::npos)
            {
                data.push_back(danhSach[i].exportToVector());
            }
        }
        if (data.empty())
        {
            cout << "[Error] No content found with name: " << tuKhoa << endl;
            cout << "Try again!" << endl;
            break;
        }
        else
            TablePrinter::printTable(widths, headers, data);
        break;
    }
    case 3:
    {
        string theLoai = InputHelper::getInputString("Enter genre: ");
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            if (theLoai == danhSach[i].getTheLoai())
            {
                data.push_back(danhSach[i].exportToVector());
            }
        }
        if (data.empty())
        {
            cout << "[Error] No content found with genre: " << theLoai << endl;
            cout << "Try again!" << endl;
            break;
        }
        else
            TablePrinter::printTable(widths, headers, data);
        break;
    }
    case 4:
    {
        int namPhatHanh = InputHelper::getInputInt("Enter release year: ");
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            if (namPhatHanh == danhSach[i].getNamPhatHanh())
            {
                data.push_back(danhSach[i].exportToVector());
            }
        }
        if (data.empty())
        {
            cout << "[Error] No content found with release year: " << namPhatHanh << endl;
            cout << "Try again!" << endl;
            break;
        }
        else
            TablePrinter::printTable(widths, headers, data);
        break;
    }
    case 5:
    {
        int choice = InputHelper::getInputInt("Choose status (1. ACTIVE / 2. INACTIVE): ");
        switch (choice)
        {
        case 1:
        {
            vector<vector<string>> data;
            for (int i = 0; i < s; i++)
            {
                if (danhSach[i].getTrangThai() == "ACTIVE")
                    data.push_back(danhSach[i].exportToVector());
            }
            if (data.empty())
            {
                cout << "[Error] No content found with status: ACTIVE" << endl;
                cout << "Try again!" << endl;
                break;
            }
            else
                TablePrinter::printTable(widths, headers, data);
            break;
        }
        case 2:
        {
            vector<vector<string>> data;
            for (int i = 0; i < s; i++)
            {
                if (danhSach[i].getTrangThai() == "INACTIVE")
                    data.push_back(danhSach[i].exportToVector());
            }
            if (data.empty())
            {
                cout << "[Error] No content found with status: INACTIVE" << endl;
                cout << "Try again!" << endl;
                break;
            }
            else
                TablePrinter::printTable(widths, headers, data);
            break;
        }
        default:
        {
            cout << "[Error] Invalid option, try again!" << endl;
            break;
        }
        }
        break;
    }
    case 6:
    {
        int choice = InputHelper::getInputInt("Choose access type (1. HOURLY / 2. NON_HOURLY): ");
        switch (choice)
        {
        case 1:
        {
            vector<vector<string>> data;
            for (int i = 0; i < s; i++)
            {
                if (danhSach[i].getHinhThucXem() == "HOURLY")
                    data.push_back(danhSach[i].exportToVector());
            }
            if (data.empty())
            {
                cout << "[Error] No content found with access type: HOURLY" << endl;
                cout << "Try again!" << endl;
                break;
            }
            else
                TablePrinter::printTable(widths, headers, data);
            break;
        }
        case 2:
        {
            vector<vector<string>> data;
            for (int i = 0; i < s; i++)
            {
                if (danhSach[i].getHinhThucXem() == "NON_HOURLY")
                    data.push_back(danhSach[i].exportToVector());
            }
            if (data.empty())
            {
                cout << "[Error] No content found with access type: NON_HOURLY" << endl;
                cout << "Try again!" << endl;
                break;
            }
            else
                TablePrinter::printTable(widths, headers, data);
            break;
        }
        default:
        {
            cout << "[Error] Invalid option, try again!" << endl;
            break;
        }
        }
        break;
    }
    default:
        SystemHelper::clearScreen();
        break;
    }
    InputHelper::getInputOptionalString("Press any key to go back: ");
    SystemHelper::clearScreen();
}
// ham duoi dung cho ham sapXep
bool soSanhNamPhatHanhTangDan(const NoiDungVOD &a, const NoiDungVOD &b)
{
    return a.getNamPhatHanh() < b.getNamPhatHanh(); // dấu < là theo thứ tự tăng dần còn dấu > là giảm dần
}
bool soSanhTheoNamPhatHanhGiamDan(const NoiDungVOD &c, const NoiDungVOD &d)
{
    return c.getNamPhatHanh() > d.getNamPhatHanh(); // dấu < là theo thứ tự tăng dần còn dấu > là giảm dần
}
// Chuc nang sap xep: sap xep theo ten (A-Z, Z-A) hoac nam phat hanh va cap nhat file
void NoiDungVODMenu::sapXep()
{
    cout << "=====Sort VOD content list=====" << endl;
    cout << "1. Sort by name (A -> Z)" << endl;
    cout << "2. Sort by name (Z -> A)" << endl;
    cout << "3. Sort by release year (newest -> oldest)" << endl;
    cout << "4. Sort by release year (oldest -> newest)" << endl;
    cout << "Press any key to go back: " << endl;
    int luaChon = InputHelper::getInputInt("Enter your choice: ");

    vector<NoiDungVOD> danhSach = repo.readAll();
    switch (luaChon)
    {
    case 1:
    {
        sort(danhSach.begin(), danhSach.end()); // tự động gọi toán tử operator <, ko cần thêm tiêu chí so sánh
        cout << "VOD content list after sorting: " << endl;
        vector<vector<string>> data;
        int s = danhSach.size();
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
        }
        TablePrinter::printTable(widths, headers, data);
        cout << "Sort order successfully!" << endl;
        break;
    }
    case 2:
    {
        sort(danhSach.begin(), danhSach.end(), greater<NoiDungVOD>()); // tự động gọi toán tử operator >, ko cần thêm tiêu chí so sánh
        cout << "VOD content list after sorting: " << endl;
        vector<vector<string>> data;
        int s = danhSach.size();
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
        }
        TablePrinter::printTable(widths, headers, data);
        cout << "Sort order successfully!" << endl;
        break;
    }
    case 3:
    {
        sort(danhSach.begin(), danhSach.end(), soSanhTheoNamPhatHanhGiamDan);
        cout << "VOD content list after sorting: " << endl;
        vector<vector<string>> data;
        int s = danhSach.size();
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
        }
        TablePrinter::printTable(widths, headers, data);
        cout << "Sort order successfully!" << endl;
        break;
    }
    case 4:
    {
        sort(danhSach.begin(), danhSach.end(), soSanhNamPhatHanhTangDan);
        cout << "VOD content list after sorting: " << endl;
        vector<vector<string>> data;
        int s = danhSach.size();
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
        }
        TablePrinter::printTable(widths, headers, data);
        cout << "Sort order successfully!" << endl;
        break;
    }
    default:
        SystemHelper::clearScreen();
        break;
    }
    InputHelper::getInputOptionalString("Press any key to go back: ");
    SystemHelper::clearScreen();
}
// Chuc nang cap nhat: tim theo ma VOD, hien thi thong tin hien tai va cho phep sua tung truong
void NoiDungVODMenu::capNhat()
{
    string id = InputHelper::getInputString("Enter VOD ID to update: ");
    id = convertToUpper(id);
    NoiDungVOD *p = repo.readByID(id);
    if (p == nullptr)
    {
        cout << "[Error] VOD ID not found, try again!" << endl;
        InputHelper::getInputOptionalString("Press any key to go back: ");
        return;
    }
    else
    {

        cout << "=====Current VOD content information=====" << endl;
        cout << "CURRENT VOD ID: " << id << endl;
        cout << "Content name: " << p->getTenNoiDung() << endl;
        cout << "Genre: " << p->getTheLoai() << endl;
        cout << "Duration (minutes): " << p->getThoiLuong() << endl;
        cout << "Description: " << p->getMoTa() << endl;
        cout << "Release year: " << p->getNamPhatHanh() << endl;
        cout << "URL: " << p->getUrl() << endl;
        cout << "Status: " << p->getTrangThai() << endl;
        cout << "Access type: " << p->getHinhThucXem() << endl;
        cout << "(Press Enter to keep current value)" << endl;
        while (true)
        {
            string tenNoiDung = InputHelper::getInputOptionalString("Update content name: ");
            if (tenNoiDung.empty())
                break;
            else
            {
                try
                {
                    p->setTenNoiDung(tenNoiDung);
                    cout << "Content name updated successfully!" << endl;
                    break;
                }
                catch (const exception &e)
                {
                    cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
                    LOG("[VODMenu] Invalid content name input during update: " + string(e.what()));
                }
            }
        }

        while (true)
        {
            string theLoai = InputHelper::getInputOptionalString("Update genre: ");
            if (theLoai.empty())
                break;
            else
            {
                try
                {
                    p->setTheLoai(theLoai);
                    cout << "Genre updated successfully!" << endl;
                    break;
                }
                catch (const exception &e)
                {
                    cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
                    LOG("[VODMenu] Invalid genre input during update: " + string(e.what()));
                }
            }
        }

        while (true)
        {
            int thoiLuong = InputHelper::getInputOptionalInt("Update duration (minutes): ");
            if (thoiLuong == -1)
                break;
            else
            {
                try
                {
                    p->setThoiLuong(thoiLuong);
                    cout << "Duration updated successfully!" << endl;
                    break;
                }
                catch (const exception &e)
                {
                    cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
                    LOG("[VODMenu] Invalid duration(minutes) input during update: " + string(e.what()));
                }
            }
        }
        while (true)
        {
            string moTa = InputHelper::getInputOptionalString("Update description: ");
            if (moTa.empty())
                break;
            else
            {
                try
                {
                    p->setMoTa(moTa);
                    cout << "Description updated successfully!" << endl;
                    break;
                }
                catch (const exception &e)
                {
                    cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
                    LOG("[VODMenu] Invalid description input during update: " + string(e.what()));
                }
            }
        }
        while (true)
        {
            int namPhatHanh = InputHelper::getInputOptionalInt("Update release year: ");
            if (namPhatHanh == -1)
                break;
            else
            {
                try
                {
                    p->setNamPhatHanh(namPhatHanh);
                    cout << "Release year updated successfully!" << endl;
                    break;
                }
                catch (const exception &e)
                {
                    cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
                    LOG("[VODMenu] Invalid release year input during update: " + string(e.what()));
                }
            }
        }
        vector<NoiDungVOD> danhSach = repo.readAll();
        int s = danhSach.size();
        while (true)
        {
            int choice = InputHelper::getInputOptionalInt("Choose URL format (1. Netflix / 2. TV360): ");
            if (choice == -1)
                break;
            else
            {
                try
                {
                    if (choice == 1)
                    {
                        int num = InputHelper::getInputInt("Enter content ID: ");
                        string url = "https://www.netflix.com/vn/phim" + to_string(num);
                        if (url != p->getUrl())
                        {
                            bool isDuplicate = false;
                            for (int i = 0; i < s; i++)
                            {
                                if (danhSach[i].getID() != p->getID() && url == danhSach[i].getUrl())
                                {
                                    isDuplicate = true;
                                    break;
                                }
                            }
                            if (isDuplicate)
                            {
                                cout << "[Error] URL already exists, please try another URL!" << endl;
                                continue;
                            }
                        }
                        p->setUrl(url);
                        cout << "URL updated successfully!" << endl;
                        break;
                    }
                    else if (choice == 2)
                    {

                        int num = InputHelper::getInputInt("Enter content ID: ");
                        string url = "https://www.tv360.com/vn/phim" + to_string(num);
                        if (url != p->getUrl())
                        {
                            bool isDuplicate = false;
                            for (int i = 0; i < s; i++)
                            {
                                if (danhSach[i].getID() != p->getID() && url == danhSach[i].getUrl())
                                {
                                    isDuplicate = true;
                                    break;
                                }
                            }
                            if (isDuplicate)
                            {
                                cout << "[Error] URL already exists, please try another URL!" << endl;
                                continue;
                            }
                        }
                        p->setUrl(url);
                        cout << "URL updated successfully!" << endl;
                        break;
                    }
                    else
                    {
                        cout << "[Error] Invalid option, try again!" << endl;
                        continue;
                    }
                }
                catch (const exception &e)
                {
                    cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
                    LOG("[VODMenu] Invalid URL input during update: " + string(e.what()));
                }
            }
        }
        while (true)
        {
            int choice = InputHelper::getInputOptionalInt("Update status (1. ACTIVE / 2. INACTIVE): ");
            if (choice == -1)
                break;
            else
            {
                try
                {
                    if (choice == 1)
                    {
                        p->setTrangThai("ACTIVE");
                        cout << "Status updated successfully!" << endl;
                        break;
                    }
                    else if (choice == 2)
                    {
                        p->setTrangThai("INACTIVE");
                        cout << "Status updated successfully!" << endl;
                        break;
                    }
                    cout << "[Error] Invalid option, please enter 1 or 2!\n";
                }
                catch (const exception &e)
                {
                    cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
                    LOG("[VODMenu] Invalid status input during update: " + string(e.what()));
                }
            }
        }
        while (true)
        {
            int choice = InputHelper::getInputOptionalInt("Update access type (1. HOURLY / 2. NON_HOURLY): ");
            if (choice == -1)
                break;
            else
            {
                try
                {
                    if (choice == 1)
                    {
                        p->setHinhThucXem("HOURLY");
                        cout << "Access type updated successfully!" << endl;
                        break;
                    }
                    else if (choice == 2)
                    {
                        p->setHinhThucXem("NON_HOURLY");
                        cout << "Access type updated successfully!" << endl;
                        break;
                    }
                    cout << "[Error] Invalid option, please enter 1 or 2!\n";
                }
                catch (const exception &e)
                {
                    cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
                    LOG("[VODMenu] Invalid access type input during update: " + string(e.what()));
                }
            }
        }
    }
    try
    {
        repo.writeToFile();
        cout << "VOD content updated successfully!" << endl;
        LOG("[VODMenu] VOD content updated successfully, ID: " + id);
    }
    catch (const exception &e)
    {
        cout << "[Error] Failed to save VOD content to file: " << e.what() << endl;
        LOG("[VODMenu] File write failed during update: " + string(e.what()));
        repo.loadFromFile();
        cout << "[Notice] Memory rolled back to last saved state on disk." << endl;
    }
    InputHelper::getInputOptionalString("Press any key to go back: ");
    SystemHelper::clearScreen();
}
// Chuc nang xoa noi dung: tim theo ma VOD, xac nhan Y/N truoc khi xoa va luu file
void NoiDungVODMenu::xoaNoiDung()
{
    repo.loadFromFile();
    string id = InputHelper::getInputString("Enter VOD ID to delete: ");
    id = convertToUpper(id);
    NoiDungVOD *p = repo.readByID(id);
    if (p == nullptr)
    {
        cout << "[Error] Invalid VOD ID, try again" << endl;
        InputHelper::getInputOptionalString("Press any key to go back: ");
        return;
    }
    else
    {
        // Kiem tra rang buoc toan ven: chan xoa neu VOD dang nam trong goi cuoc (Package)
        repopackage.loadFromFile();
        vector<Package> danhSachPackage = repopackage.readAll();
        int sPackage = danhSachPackage.size();
        int check = 0;
        string tenGoiCuoc;
        // Duyet qua tung goi cuoc de kiem tra danh sach vodIDs
        for (int i = 0; i < sPackage; i++)
        {
            const vector<string> &idVODs = danhSachPackage[i].getVodIDs();
            int sidVODS = idVODs.size();
            // Duyet qua tung ma VOD co trong goi cuoc hien tai
            for (int j = 0; j < sidVODS; j++)
            {
                if (idVODs[j] == id)
                {
                    check = 1;
                    tenGoiCuoc = danhSachPackage[i].getName(); // Lay ten goi cuoc dang chua VOD nay
                    break;
                }
            }
            if (check)
                break;
        }

        // Neu VOD dang duoc su dung boi it nhat 1 goi cuoc thi chan xoa va thong bao
        if (check)
        {
            cout << "[Error] Cannot delete! VOD content belongs to package: " << tenGoiCuoc << endl;
            InputHelper::getInputOptionalString("Press any key to go back: ");
            return;
        }
        string line = p->exportToString();
        cout << line << endl;
        string xacNhan = InputHelper::getInputString("Are you sure you want to delete this content? (y/n): ");
        if (xacNhan == "Y" || xacNhan == "y" || xacNhan == "YES" || xacNhan == "yes" || xacNhan == "Yes")
        {
            repo.remove(id);
            try
            {
                repo.writeToFile();
                cout << "VOD content deleted successfully!" << endl;
            }
            catch (const exception &e)
            {
                cout << "[Error] Failed to save deletion to file: " << e.what() << endl;
                LOG("[VODMenu] File write failed during delete: " + string(e.what()));
                repo.loadFromFile(); // Khoi phuc lai ban ghi tren RAM
                cout << "[Notice] Memory rolled back to last saved state on disk." << endl;
            }
        }
        else if (xacNhan == "N" || xacNhan == "n" || xacNhan == "no" || xacNhan == "NO" || xacNhan == "No")
        {
            cout << "[Abort] Deletion aborted" << endl;
        }
        else
            return;
    }
    InputHelper::getInputOptionalString("Press any key to go back: ");
    SystemHelper::clearScreen();
}
// Xu ly lua chon chuc nang tu menu VOD
void NoiDungVODMenu::handleChoice(int choice)
{
    switch (choice)
    {
    case 2:
        xemDanhSach();
        break;
    case 1:
        themNoiDung();
        break;
    case 3:
        timKiem();
        break;
    case 4:
        sapXep();
        break;
    case 5:
        capNhat();
        break;
    case 6:
        xoaNoiDung();
        break;
    default:
        cout << "[Error] Invalid option, try again!" << endl;
        break;
    }
}
// Hien thi giao dien menu quan ly noi dung VOD
void NoiDungVODMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "=====VOD Content Management=====" << endl;
    cout << "1. Add new VOD content" << endl;
    cout << "2. View all VOD content" << endl;
    cout << "3. Search VOD content" << endl;
    cout << "4. Sort VOD content list" << endl;
    cout << "5. Update VOD content" << endl;
    cout << "6. Delete VOD content" << endl;
}
