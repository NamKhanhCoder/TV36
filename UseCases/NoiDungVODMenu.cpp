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
#include "Utils/Validator.h"
#include "Utils/SystemHelper.h"
using namespace std;
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
        cout << "Danh sach noi dung hien dang rong !" << endl;
        return;
    }
    vector<int> widths = {8, 34, 10, 18, 42, 14, 38, 15};
    vector<string> headers = {"Ma VOD", "Ten noi dung", "The loai", "Thoi luong (phut)", "Mo ta", "Nam phat hanh", "Video URL", "Trang thai"};
    TablePrinter::printTable(widths, headers, repo.exportToVector());
    InputHelper::getInputOptionalString("Nhan bat ky de quay lai: ");
    SystemHelper::clearScreen();
}
// Chuc nang them noi dung moi: nhap thong tin, validate du lieu, check trung URL va ghi file
void NoiDungVODMenu::themNoiDung()
{
    string id = repo.generateNextID();
    string tenNoiDung;
    while (true)
    {
        tenNoiDung = InputHelper::getInputString("Nhap ten noi dung: ");
        if (tenNoiDung.find('|') != string::npos)
        {
            cout << "Du lieu khong duoc chua ky tu '|', vui long thu lai!" << endl;
            continue;
        }
        break;
    }
    string theLoai;
    while (true)
    {
        theLoai = InputHelper::getInputString("Nhap the loai noi dung: ");
        if (theLoai.find('|') != string::npos)
        {
            cout << "Du lieu khong duoc chua ky tu '|', vui long thu lai!" << endl;
            continue;
        }
        break;
    }

    int thoiLuong;
    while (true)
    {
        thoiLuong = InputHelper::getInputInt("Nhap thoi luong cua noi dung (phut): ");
        if (thoiLuong <= 0)
        {
            cout << "Thoi luong phai lon hon 0!" << endl;
            continue;
        }
        else
            break;
    }
    string moTa;
    while (true)
    {
        moTa = InputHelper::getInputOptionalString("Nhap mo ta noi dung: ");
        if (moTa.find('|') != string::npos)
        {
            cout << "Du lieu khong duoc chua ky tu '|', vui long thu lai!" << endl;
            continue;
        }
        break;
    }
    int namPhatHanh;
    while (true)
    {
        namPhatHanh = InputHelper::getInputInt("Nhap nam phat hanh noi dung: ");
        if (namPhatHanh < 1950 || namPhatHanh > 2026)
        {
            cout << "Nam phat hanh phai tu 1950 den nam 2026!" << endl;
            continue;
        }
        else
            break;
    }
    string url;
    vector<NoiDungVOD> danhSach = repo.readAll();
    int s = danhSach.size();
    while (true)
    {
        int choice = InputHelper::getInputInt("Chon dinh dang url (1. Netflix / 2. TV360): ");
        switch (choice)
        {
        case 1:
        {
            int num = InputHelper::getInputInt("Nhap ma cua noi dung can them vao: ");
            url = "https://www.netflix.com/vn/phim" + to_string(num);
            break;
        }
        case 2:
        {
            int num = InputHelper::getInputInt("Nhap ma cua noi dung can them vao: ");
            url = "https://www.tv360.com/vn/phim" + to_string(num);
            break;
        }
        default:
            cout << "Lua chon khong hop le, vui long thu lai!" << endl;
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
            cout << "URL da bi trung voi noi dung khac, vui long thu lai!" << endl;
            continue;
        }
        break;
    }
    string trangThai;
    while (true)
    {
        int choice = InputHelper::getInputInt("Lua chon trang thai cua noi dung (1. DANG_PHUC_VU/ 2. NGUNG_PHUC_VU): ");
        switch (choice)
        {
        case 1:
        {
            trangThai = "DANG_PHUC_VU";
            break;
        }
        case 2:
        {
            trangThai = "NGUNG_PHUC_VU";
            break;
        }
        default:
            cout << "Lua chon khong hop le, vui long thu lai!" << endl;
            continue;
        }
        break;
    }

    NoiDungVOD noiDungMoi;
    noiDungMoi.setID(id);
    noiDungMoi.setTenNoiDung(tenNoiDung);
    noiDungMoi.setTheLoai(theLoai);
    noiDungMoi.setThoiLuong(thoiLuong);
    noiDungMoi.setMoTa(moTa);
    noiDungMoi.setNamPhatHanh(namPhatHanh);
    noiDungMoi.setUrl(url);
    noiDungMoi.setTrangThai(trangThai);
    repo.create(noiDungMoi); // gọi hàm trong repository đêu phải thông qua repo
    repo.writeToFile();
    cout << "Them noi dung moi thanh cong!" << endl;
    InputHelper::getInputOptionalString("Nhan bat ky de quay lai: ");
    SystemHelper::clearScreen();
}
// Chuc nang tim kiem: ho tro tim theo ma VOD, ten, the loai, nam phat hanh, trang thai
void NoiDungVODMenu::timKiem()
{
    cout << "+---TIM KIEM NOI DUNG---+" << endl;
    cout << "1. Tim theo ma VOD" << endl;
    cout << "2. Tim theo Ten noi dung" << endl;
    cout << "3. Tim theo the loai" << endl;
    cout << "4. Tim theo nam phat hanh" << endl;
    cout << "5. Tim kiem theo trang thai" << endl;
    cout << "Bam bat ki de quay lai." << endl;
    int luaChon = InputHelper::getInputInt("Nhap lua chon: ");
    vector<int> widths = {8, 34, 10, 18, 42, 14, 38, 15};
    vector<string> headers = {"Ma VOD", "Ten noi dung", "The loai", "Thoi luong (phut)", "Mo ta", "Nam phat hanh", "Video URL", "Trang thai"};
    repo.loadFromFile();
    const vector<NoiDungVOD> &danhSach = repo.readAll(); // ta chỉ đọc để tìm kiếm chứ không sửa đổi hay hoán đổi vị trí các phần tử như bên sapXep, nên  có thể dùng tham chiếu const &
    int s = danhSach.size();
    switch (luaChon)
    {
    case 1:
    { // khiến cho biến string id có thể bị bỏ qua quá trình khởi tạo nên khắc phục bằng thêm {} trc các case rồi mới thi hành lệnh
        string id = InputHelper::getInputString("Nhap ma VOD (VODXXX): ");
        NoiDungVOD *p = repo.readByID(id);
        if (p == nullptr)
        {
            cout << "Khong tim thay noi dung co ma VOD: " << id << endl;
            cout << "Vui long thu lai!" << endl;
            return;
        }
        else
            TablePrinter::printSingle(widths, headers, p->exportToVector()); // Dấu -> có nghĩa là: "Hãy đi đến địa chỉ ô nhớ mà p đang chỉ tới, rồi gọi hàm exportToVector() của đối tượng tại đó"
        break;
    }
    case 2:
    {
        string tuKhoa = InputHelper::getInputString("Nhap Ten noi dung: ");
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
            cout << "Khong tim thay noi dung co ten gan dung: " << tuKhoa << endl;
            cout << "Vui long thu lai!" << endl;
            return;
        }
        else
            TablePrinter::printTable(widths, headers, data);
        break;
    }
    case 3:
    {
        string theLoai = InputHelper::getInputString("Nhap the loai: ");
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
            cout << "Khong tim thay noi dung co the loai: " << theLoai << endl;
            cout << "Vui long thu lai!" << endl;
            return;
        }
        else
            TablePrinter::printTable(widths, headers, data);
        break;
    }
    case 4:
    {
        int namPhatHanh = InputHelper::getInputInt("Nhap nam phat hanh: ");
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
            cout << "Khong tim thay noi dung co nam phat hanh: " << namPhatHanh << endl;
            cout << "Vui long thu lai!" << endl;
            return;
        }
        else
            TablePrinter::printTable(widths, headers, data);
        break;
    }
    case 5:
    {
        int choice = InputHelper::getInputInt("Lua chon trang thai cua noi dung(1.DANG_PHUC_VU/ 2.NGUNG_PHUC_VU): ");
        switch (choice)
        {
        case 1:
        {
            vector<vector<string>> data;
            for (int i = 0; i < s; i++)
            {
                if (danhSach[i].getTrangThai() == "DANG_PHUC_VU")
                    data.push_back(danhSach[i].exportToVector());
            }
            if (data.empty())
            {
                cout << "Khong tim thay noi dung co trang thai : DANG_PHUC_VU" << endl;
                cout << "Vui long thu lai!" << endl;
                return;
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
                if (danhSach[i].getTrangThai() == "NGUNG_PHUC_VU")
                    data.push_back(danhSach[i].exportToVector());
            }
            if (data.empty())
            {
                cout << "Khong tim thay noi dung co trang thai : NGUNG_PHUC_VU" << endl;
                cout << "Vui long thu lai!" << endl;
                return;
            }
            else
                TablePrinter::printTable(widths, headers, data);
            break;
        }
        }
        break;
    }
    default:
        return;
        break;
    }
    InputHelper::getInputOptionalString("Nhan bat ky de quay lai: ");
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
    cout << "+---SAP XEP DANH SACH NOI DUNG---+" << endl;
    cout << "1. Sap xep theo thu tu (A -> Z)" << endl;
    cout << "2. Sap xep theo thu tu (Z -> A)" << endl;
    cout << "3. Sap xep theo nam phat hanh (moi -> cu)" << endl;
    cout << "4. Sap xep theo nam phat hanh (cu -> moi)" << endl;
    cout << "0. Quay lai" << endl;
    int luaChon = InputHelper::getInputInt("Nhap lua chon: ");
    vector<int> widths = {8, 34, 10, 18, 42, 14, 38, 15};
    vector<string> headers = {"Ma VOD", "Ten noi dung", "The loai", "Thoi luong (phut)", "Mo ta", "Nam phat hanh", "Video URL", "Trang thai"};
    vector<NoiDungVOD> danhSach = repo.readAll();
    switch (luaChon)
    {
    case 1:
    {
        sort(danhSach.begin(), danhSach.end());  // tự động gọi toán tử operator <, ko cần thêm tiêu chí so sánh
        ofstream tenFile("Data/noidungvod.txt"); // goi hàm ofstream là mặc định sẽ xoá nội dung trong file
        cout << "Danh sach noi dung sau khi sap xep xong: " << endl;
        vector<vector<string>> data;
        int s = danhSach.size();
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repo.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Da cap nhat thu tu sap xep vao file thanh cong!" << endl;
        break;
    }
    case 2:
    {
        sort(danhSach.begin(), danhSach.end(), greater<NoiDungVOD>()); // tự động gọi toán tử operator <, ko cần thêm tiêu chí so sánh
        ofstream tenFile("Data/noidungvod.txt");                       // goi hàm ofstream là mặc định sẽ xoá nội dung trong file
        cout << "Danh sach noi dung sau khi sap xep xong: " << endl;
        vector<vector<string>> data;
        int s = danhSach.size();
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repo.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Da cap nhat thu tu sap xep vao file thanh cong!" << endl;
        break;
    }
    case 3:
    {
        sort(danhSach.begin(), danhSach.end(), soSanhTheoNamPhatHanhGiamDan);
        ofstream tenFile("Data/noidungvod.txt");
        cout << "Danh sach noi dung sau khi sap xep xong: " << endl;
        vector<vector<string>> data;
        int s = danhSach.size();
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repo.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Da cap nhat thu tu sap xep vao file thanh cong!" << endl;
        break;
    }
    case 4:
    {
        sort(danhSach.begin(), danhSach.end(), soSanhNamPhatHanhTangDan);
        ofstream tenFile("Data/noidungvod.txt");
        cout << "Danh sach noi dung sau khi sap xep xong: " << endl;
        vector<vector<string>> data;
        int s = danhSach.size();
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repo.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Da cap nhat thu tu sap xep vao file thanh cong!" << endl;
        break;
    }
    case 0:
    {
        return;
        break;
    }
    default:
        cout << "Lua chon khong hop le!" << endl;
        break;
    }
    InputHelper::getInputOptionalString("Nhan bat ky de quay lai: ");
    SystemHelper::clearScreen();
}
// Chuc nang cap nhat: tim theo ma VOD, hien thi thong tin hien tai va cho phep sua tung truong
void NoiDungVODMenu::capNhat()
{
    string id = InputHelper::getInputString("Nhap ma VOD can cap nhat: ");
    if (id.find('|') != string::npos)
    {
        cout << "Du lieu khong duoc chua ky tu '|', vui long thu lai!" << endl;
        return;
    }
    NoiDungVOD *p = repo.readByID(id);
    if (p == nullptr)
    {
        cout << "Khong tim thay ma VOD hop le, vui long thu lai!" << endl;
        return;
    }
    else
    {

        cout << "+---THONG TIN NOI DUNG HIEN TAI---+" << endl;
        cout << "MA NOI DUNG HIEN TAI: " << id << endl;
        cout << "Ten noi dung: " << p->getTenNoiDung() << endl;
        cout << "The loai: " << p->getTheLoai() << endl;
        cout << "Thoi Luong (phut): " << p->getThoiLuong() << endl;
        cout << "Mo ta: " << p->getMoTa() << endl;
        cout << "Nam phat hanh: " << p->getNamPhatHanh() << endl;
        cout << "URL: " << p->getUrl() << endl;
        cout << "Trang thai: " << p->getTrangThai() << endl;
        cout << "(Nhan Enter neu muon giu nguyen gia tri cu)" << endl;
        string tenNoiDung = InputHelper::getInputOptionalString("Cap nhat Ten noi dung: ");
        if (tenNoiDung.find('|') != string::npos)
        {
            cout << "Du lieu khong duoc chua ky tu '|', vui long thu lai!" << endl;
            return;
        }
        string theLoai = InputHelper::getInputOptionalString("Cap nhat the loai: ");
        if (theLoai.find('|') != string::npos)
        {
            cout << "Du lieu khong duoc chua ky tu '|', vui long thu lai!" << endl;
            return;
        }
        int thoiLuong = InputHelper::getInputOptionalInt("Cap nhat thoi luong (phut): ");
        string moTa = InputHelper::getInputOptionalString("Cap nhat mo ta: ");
        if (moTa.find('|') != string::npos)
        {
            cout << "Du lieu khong duoc chua ky tu '|', vui long thu lai!" << endl;
            return;
        }
        int namPhatHanh = InputHelper::getInputOptionalInt("Cap nhat nam phat hanh: ");
        int choice = InputHelper::getInputOptionalInt("Cap nhat trang thai phim (1.DANG_PHUC_VU/ 2. NGUNG_PHUC_VU): ");
        if (!tenNoiDung.empty())
        {
            p->setTenNoiDung(tenNoiDung);
            cout << "Cap nhat Ten noi dung thanh cong!" << endl;
        }
        if (!theLoai.empty())
        {
            p->setTheLoai(theLoai);
            cout << "Cap nhat the loai thanh cong!" << endl;
        }
        if (thoiLuong > 0)
        {
            p->setThoiLuong(thoiLuong);
            cout << "Cap nhat thoi luong thanh cong!" << endl;
        }
        if (!moTa.empty())
        {
            p->setMoTa(moTa);
            cout << "Cap nhat mo ta thanh cong!" << endl;
        }
        if (namPhatHanh >= 1950 && namPhatHanh <= 2026)
        {
            p->setNamPhatHanh(namPhatHanh);
            cout << "Cap nhat nam phat hanh thanh cong!" << endl;
        }
        if (choice == 1)
        {
            p->setTrangThai("DANG_PHUC_VU");
        }
        if (choice == 2)
        {
            p->setTrangThai("NGUNG_PHUC_VU");
        }
    }
    repo.writeToFile();
    cout << "Cap nhat thong tin noi dung thanh cong!" << endl;
    InputHelper::getInputOptionalString("Nhan bat ky de quay lai: ");
    SystemHelper::clearScreen();
}
// Chuc nang xoa noi dung: tim theo ma VOD, xac nhan Y/N truoc khi xoa va luu file
void NoiDungVODMenu::xoaNoiDung()
{
    string id = InputHelper::getInputString("Nhap ma VOD can xoa: ");
    NoiDungVOD *p = repo.readByID(id);
    if (p == nullptr)
    {
        cout << "Ma VOD khong hop le, vui long thu lai" << endl;
    }
    else
    {
        // Kiem tra rang buoc toan ven: chan xoa neu VOD dang nam trong goi cuoc (Package)
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
            cout << "Khong the xoa! Noi dung VOD nay dang thuoc goi cuoc: " << tenGoiCuoc << endl;
            return;
        }
        string line = p->exportToString();
        cout << line << endl;
        string xacNhan = InputHelper::getInputString("Ban co chac muon xoa noi dung nay khong ? (y/n): ");
        if (xacNhan == "Y" || xacNhan == "y" || xacNhan == "YES" || xacNhan == "yes" || xacNhan == "Yes")
        {
            repo.remove(id);
            repo.writeToFile();
            cout << "Xoa noi dung VOD thanh cong!" << endl;
        }
        else if (xacNhan == "N" || xacNhan == "n" || xacNhan == "no" || xacNhan == "NO" || xacNhan == "No")
        {
            cout << "Da huy thao tac xoa noi dung!" << endl;
            return;
        }
        else
            return;
    }
    InputHelper::getInputOptionalString("Nhan bat ky de quay lai: ");
    SystemHelper::clearScreen();
}
// Xu ly lua chon chuc nang tu menu VOD
void NoiDungVODMenu::handleChoice(int choice)
{
    switch (choice)
    {
    case 1:
        SystemHelper::clearScreen();
        xemDanhSach();
        break;
    case 2:
        SystemHelper::clearScreen();
        themNoiDung();
        break;
    case 3:
        SystemHelper::clearScreen();
        timKiem();
        break;
    case 4:
        SystemHelper::clearScreen();
        sapXep();
        break;
    case 5:
        SystemHelper::clearScreen();
        capNhat();
        break;
    case 6:
        SystemHelper::clearScreen();
        xoaNoiDung();
        break;
    default:
        SystemHelper::clearScreen();
        cout << "Lua chon khong hop le, vui long thu lai!" << endl;
        break;
    }
}
// Hien thi giao dien menu quan ly noi dung VOD
void NoiDungVODMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "======QUAN LY NOI DUNG VOD======" << endl;
    cout << "1. Xem danh sach noi dung VOD" << endl;
    cout << "2. Them noi dung VOD moi" << endl;
    cout << "3. Tim kiem noi dung VOD" << endl;
    cout << "4. Sap xep danh sach noi dung VOD" << endl;
    cout << "5. Cap nhat noi dung VOD" << endl;
    cout << "6. Xoa noi dung VOD" << endl;
}
