#include "HoaDonMenu.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include "Utils/TablePrinter.h"
#include "Utils/InputHelper.h"
#include "Utils/Validator.h"
using namespace std;
HoaDonMenu::HoaDonMenu() : repoHoaDon("Data/hoadon.txt", "Data/hdn_counter.txt", "HDN"),
repoCustomer("Data/customers.txt", "Data/customers_counter.txt", "KH"),
repoContract("Data/contracts.txt", "Data/contracts_counter.txt", "HD")
{
    repoHoaDon.loadFromFile();
    repoCustomer.loadFromFile();
    repoContract.loadFromFile();
}
void HoaDonMenu::xemDanhSach()
{
    if (repoHoaDon.readAll().empty())
    {
        cout << "Danh sach hoa don hien dang trong!" << endl;
        return;
    }
    vector<int> widths = {8, 8, 8, 12, 12, 14, 15, 17};
    vector<string> headers = {"Ma HDN", "Ma HD", "Ma KH", "So tien", "Ngay lap", "Han tra tien", "Ngay tra tien", "Trang thai"};
    TablePrinter::printTable(widths, headers, repoHoaDon.exportToVector());
    vector<HoaDon> danhSach = repoHoaDon.readAll();
    double tongDoanhThu = 0;
    int s = danhSach.size();
    for (int i = 0; i < s; i++)
    {
        tongDoanhThu += danhSach[i].getSoTien();
    }
    cout << "Tong doanh thu: " << (long long)tongDoanhThu << " VND" << endl;
}
void HoaDonMenu::themHoaDon()
{
    string id = repoHoaDon.generateNextID();
    string maHopDong = InputHelper::getInputString("Nhap ma hop dong: ");
    if (!kiemTraHopDongTonTai(maHopDong))
    {
        cout << "Ma hop dong khong ton tai hoac chua hop le, vui long thu lai!" << endl;
        return;
    }
    string maKhachHang = InputHelper::getInputString("Nhap ma khach hang: ");
    double soTien = InputHelper::getInputDouble("Nhap so tien: ");
    if (soTien <= 0.0)
    {
        cout << "So tien khong hop le, vui long thu lai" << endl;
        return;
    }
    /*Contract* p = repoContract.readByID(maHopDong);
    Date ngayLap = p->get_signDate();
    */
    Date ngayLap = InputHelper::getInputDate("Nhap ngay lap (DD/MM/YYYY): ");
    Date hanTraTien = InputHelper::getInputDate("Nhap han tra tien (DD/MM/YYYY): ");
    string trangThai;
    Date ngayTraTien;
    while (true)
    {
        int choice = InputHelper::getInputInt("Nhap trang thai (1/2) (1. Da thanh toan/ 2. Chua thanh toan): ");
        switch (choice)
        {
        case 1:
            trangThai = "da_thanh_toan";
            ngayTraTien = InputHelper::getInputDate("Nhap ngay tra tien (DD/MM/YYYY): ");
            break;
        case 2:
            trangThai = "chua_thanh_toan";
            ngayTraTien = Date();
            break;
        default:
            cout << "Lua chon khong hop le, vui long thu lai" << endl;
            continue;
        }
        break;
    }
    HoaDon hoaDonMoi;
    hoaDonMoi.setID(id);
    hoaDonMoi.setMaHopDong(maHopDong);
    hoaDonMoi.setMaKhachHang(maKhachHang);
    hoaDonMoi.setSoTien(soTien);
    hoaDonMoi.setNgayLap(ngayLap);
    hoaDonMoi.setHanTraTien(hanTraTien);
    hoaDonMoi.setNgayTraTien(ngayTraTien);
    hoaDonMoi.setTrangThai(trangThai);
    repoHoaDon.create(hoaDonMoi);
    repoHoaDon.writeToFile();
}
void HoaDonMenu::timKiem()
{
    cout << "+---TIM KIEM HOA DON---+" << endl;
    cout << "1. Tim theo ma hoa don" << endl;
    cout << "2. Tim theo ma hop dong" << endl;
    cout << "3. Tim theo ma khach hang" << endl;
    cout << "4. Tim theo ngay lap hoa don" << endl;
    cout << "5. Tim theo han tra tien hoa don" << endl;
    cout << "6. Tim theo ngay tra tien hoa don" << endl;
    cout << "7. Tim theo trang thai thanh toan" << endl;
    cout << "0. Quay lai" << endl;
    int choice = InputHelper::getInputInt("Nhap lua chon: ");
    repoHoaDon.loadFromFile();
    vector<int> widths = {8, 8, 8, 12, 12, 14, 15, 17};
    vector<string> headers = {"Ma HDN", "Ma HD", "Ma KH", "So tien", "Ngay lap", "Han tra tien", "Ngay tra tien", "Trang thai"};
    const vector<HoaDon> &danhSach = repoHoaDon.readAll();
    int s = danhSach.size();
    switch (choice)
    {
    case 1:
    {
        string id = InputHelper::getInputString("Nhap ma hoa don (HDNXXX): ");
        HoaDon *p = repoHoaDon.readByID(id);
        if (p == nullptr)
        {
            cout << "Khong tim thay hoa don co ma hoa don: " << id << endl;
            cout << "Vui long thu lai!" << endl;
            return;
        }
        else
            TablePrinter::printSingle(widths, headers, p->exportToVector());
        break;
    }
    case 2:
    {
        string maHopDong = InputHelper::getInputString("Nhap ma hop dong (HDXXX): ");
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            if (maHopDong == danhSach[i].getMaHopDong())
            {
                data.push_back(danhSach[i].exportToVector());
            }
        }
        if (data.empty())
        {
            cout << "Khong tim thay hoa don co ma hop dong: " << maHopDong << endl;
            cout << "Vui long thu lai!" << endl;
            return;
        }
        else
            TablePrinter::printTable(widths, headers, data);
        break;
    }
    case 3:
    {
        string maKhachHang = InputHelper::getInputString("Nhap ma khach hang (KHXXX): ");
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            if (maKhachHang == danhSach[i].getMaKhachHang())
            {
                data.push_back(danhSach[i].exportToVector());
            }
        }
        if (data.empty())
        {
            cout << "Khong tim thay hoa don co ma khach hang: " << maKhachHang << endl;
            cout << "Vui long thu lai!" << endl;
            return;
        }
        else
            TablePrinter::printTable(widths, headers, data);
        break;
    }
    case 4:
    {
        Date ngayLap = InputHelper::getInputDate("Nhap ngay lap hoa don (DD/MM/YYYY): ");
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            if (ngayLap == danhSach[i].getNgayLap())
            {
                data.push_back(danhSach[i].exportToVector());
            }
        }
        if (data.empty())
        {
            cout << "Khong tim thay hoa don co ngay lap: " << ngayLap.toString() << endl;
            cout << "Vui long thu lai!" << endl;
            return;
        }
        else
            TablePrinter::printTable(widths, headers, data);
        break;
    }
    case 5:
    {
        Date hanTraTien = InputHelper::getInputDate("Nhap han tra tien hoa don (DD/MM/YYYY): ");
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            if (hanTraTien == danhSach[i].getHanTraTien())
            {
                data.push_back(danhSach[i].exportToVector());
            }
        }
        if (data.empty())
        {
            cout << "Khong tim thay hoa don co han tra: " << hanTraTien.toString() << endl;
            cout << "Vui long thu lai!" << endl;
            return;
        }
        else
            TablePrinter::printTable(widths, headers, data);
        break;
    }
    case 6:
    {
        Date ngayTraTien = InputHelper::getInputOptionalDate("Nhap ngay tra tien hoa don (DD/MM/YYYY): ");
        vector<vector<string>> data;
        if (ngayTraTien.isEmpty())
        {
            for (int i = 0; i < s; i++)
            {
                if (danhSach[i].getNgayTraTien().isEmpty())
                {
                    data.push_back(danhSach[i].exportToVector());
                }
            }
            if (data.empty())
            {
                cout << "Khong tim thay hoa don co ngay tra: " << "NONE" << endl;
                cout << "Vui long thu lai!" << endl;
                return;
            }
            else
                TablePrinter::printTable(widths, headers, data);
        }
        else
        {
            for (int i = 0; i < s; i++)
            {
                if (ngayTraTien == danhSach[i].getNgayTraTien())
                {
                    data.push_back(danhSach[i].exportToVector());
                }
            }
            if (data.empty())
            {
                cout << "Khong tim thay hoa don co ngay tra: " << ngayTraTien.toString() << endl;
                cout << "Vui long thu lai!" << endl;
                return;
            }
            else
                TablePrinter::printTable(widths, headers, data);
        }

        break;
    }

    case 7:
    {
        int choice = InputHelper::getInputInt("Nhap trang thai thanh toan (1/2) (1. Da thanh toan/ 2. Chua thanh toan)");
        vector<vector<string>> data;
        if (choice == 1)
        {
            for (int i = 0; i < s; i++)
            {
                if (danhSach[i].getTrangThai() == "da_thanh_toan")
                {
                    data.push_back(danhSach[i].exportToVector());
                }
            }
        }
        else if (choice == 2)
        {
            for (int i = 0; i < s; i++)
            {
                if (danhSach[i].getTrangThai() == "chua_thanh_toan")
                {
                    data.push_back(danhSach[i].exportToVector());
                }
            }
        }
        else
        {
            cout << "Lua chon khong hop le, vui long thu lai!" << endl;
            return;
        }
        TablePrinter::printTable(widths, headers, data);
        break;
    }
    case 0:
    {
        return;
        break;
    }
    }
}
bool soSanhTheoNgayLapTangDan(const HoaDon &a, const HoaDon &b)
{
    return a.getNgayLap() < b.getNgayLap();
}
bool soSanhTheoNgayLapGiamDan(const HoaDon &a, const HoaDon &b)
{
    return a.getNgayLap() > b.getNgayLap();
}
bool soSanhTheoHanTraTienTangDan(const HoaDon &a, const HoaDon &b)
{
    return a.getHanTraTien() < b.getHanTraTien();
}
bool soSanhTheoHanTraTienGiamDan(const HoaDon &a, const HoaDon &b)
{
    return a.getHanTraTien() > b.getHanTraTien();
}
bool soSanhTheoNgayTraTienTangDan(const HoaDon &a, const HoaDon &b)
{
    if (a.getNgayTraTien().isEmpty())
        return false;
    if (b.getNgayTraTien().isEmpty())
        return true;
    return a.getNgayTraTien() < b.getNgayTraTien();
}
bool soSanhTheoNgayTraTienGiamDan(const HoaDon &a, const HoaDon &b)
{
    if (a.getNgayTraTien().isEmpty())
        return false;
    if (b.getNgayTraTien().isEmpty())
        return true;
    return a.getNgayTraTien() > b.getNgayTraTien();
}
void HoaDonMenu::sapXep()
{
    cout << "+---SAP XEP DANH SACH HOA DON---+" << endl;
    cout << "1. Sap xep theo so tien (Min -> Max)" << endl;
    cout << "2. Sap xep theo so tien (Max -> Min)" << endl;
    cout << "3. Sap xep theo ngay lap (Moi nhat -> Cu nhat)" << endl;
    cout << "4. Sap xep theo ngay lap (Cu nhat -> Moi nhat)" << endl;
    cout << "5. Sap xep theo han tra tien (Moi nhat -> Cu nhat)" << endl;
    cout << "6. Sap xep theo han tra tien (Cu nhat -> Moi nhat)" << endl;
    cout << "7. Sap xep theo ngay tra tien (Moi nhat -> Cu nhat)" << endl;
    cout << "8. Sap xep theo ngay tra tien (Cu nhat -> Moi nhat)" << endl;
    cout << "0. Quay lai" << endl;
    int choice = InputHelper::getInputInt("Nhap lua chon cua ban : ");
    vector<HoaDon> danhSach = repoHoaDon.readAll();
    int s = danhSach.size();
    vector<int> widths = {8, 8, 8, 12, 12, 14, 15, 17};
    vector<string> headers = {"Ma HDN", "Ma HD", "Ma KH", "So tien", "Ngay lap", "Han tra tien", "Ngay tra tien", "Trang thai"};
    switch (choice)
    {
    case 1:
    {
        sort(danhSach.begin(), danhSach.end());
        ofstream tenFile("Data/hoadon.txt");
        cout << "Danh sach hoa don sau khi sap xep xong: " << endl;
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repoHoaDon.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Da cap nhat thu tu sap xep vao file thanh cong!" << endl;
        break;
    }
    case 2:
    {
        sort(danhSach.begin(), danhSach.end(), greater<HoaDon>());
        ofstream tenFile("Data/hoadon.txt");
        cout << "Danh sach hoa don sau khi sap xep xong: " << endl;
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repoHoaDon.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Da cap nhat thu tu sap xep vao file thanh cong!" << endl;
        break;
    }
    case 3:
    {
        sort(danhSach.begin(), danhSach.end(), soSanhTheoNgayLapGiamDan);
        ofstream tenFile("Data/hoadon.txt");
        cout << "Danh sach hoa don sau khi sap xep xong: " << endl;
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repoHoaDon.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Da cap nhat thu tu sap xep vao file thanh cong!" << endl;
        break;
    }
    case 4:
    {
        sort(danhSach.begin(), danhSach.end(), soSanhTheoNgayLapTangDan);
        ofstream tenFile("Data/hoadon.txt");
        cout << "Danh sach hoa don sau khi sap xep xong: " << endl;
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repoHoaDon.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Da cap nhat thu tu sap xep vao file thanh cong!" << endl;
        break;
    }
    case 5:
    {
        sort(danhSach.begin(), danhSach.end(), soSanhTheoHanTraTienGiamDan);
        ofstream tenFile("Data/hoadon.txt");
        cout << "Danh sach hoa don sau khi sap xep xong: " << endl;
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repoHoaDon.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Da cap nhat thu tu sap xep vao file thanh cong!" << endl;
        break;
    }
    case 6:
    {
        sort(danhSach.begin(), danhSach.end(), soSanhTheoHanTraTienTangDan);
        ofstream tenFile("Data/hoadon.txt");
        cout << "Danh sach hoa don sau khi sap xep xong: " << endl;
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repoHoaDon.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Da cap nhat thu tu sap xep vao file thanh cong!" << endl;
        break;
    }
    case 7:
    {
        sort(danhSach.begin(), danhSach.end(), soSanhTheoNgayTraTienGiamDan);
        ofstream tenFile("Data/hoadon.txt");
        cout << "Danh sach hoa don sau khi sap xep xong: " << endl;
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repoHoaDon.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Da cap nhat thu tu sap xep vao file thanh cong!" << endl;
        break;
    }
    case 8:
    {
        sort(danhSach.begin(), danhSach.end(), soSanhTheoNgayTraTienTangDan);
        ofstream tenFile("Data/hoadon.txt");
        cout << "Danh sach hoa don sau khi sap xep xong: " << endl;
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repoHoaDon.loadFromFile();
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
}
void HoaDonMenu::capNhat()
{
    string id = InputHelper::getInputString("Nhap ma hoa don: ");
    HoaDon *p = repoHoaDon.readByID(id);
    if (p == nullptr)
    {
        cout << "Ma hoa don khong hop le, vui long thu lai!" << endl;
        return;
    }
    else
    {
        cout << "+---THONG TIN HOA DON HIEN TAI---+" << endl;
        cout << "Ma hoa don: " << id << endl;
        cout << "Ma hop dong: " << p->getMaHopDong() << endl;
        cout << "Ma khach hang: " << p->getMaKhachHang() << endl;
        cout << "So tien: " << p->getSoTien() << endl;
        cout << "Ngay lap: " << (p->getNgayLap()).toString() << endl;
        cout << "Han tra tien: " << (p->getHanTraTien()).toString() << endl;
        cout << "Ngay tra tien: " << (p->getNgayTraTien()).toString() << endl;
        cout << "Trang thai: " << p->getTrangThai() << endl;
        double soTien = InputHelper::getInputOptionalDouble("Nhap so tien: ");
        Date ngayLap = InputHelper::getInputOptionalDate("Nhap ngay lap: ");
        Date hanTraTien = InputHelper::getInputOptionalDate("Nhap han tra tien: ");
        int choice = InputHelper::getInputOptionalInt("Nhap trang thai (1.da_thanh_toan/ 2.chua_thanh_toan)");
        if (soTien > 0.0)
        {
            p->setSoTien(soTien);
            cout << "Cap nhat so tien thanh cong" << endl;
        }
        if (!ngayLap.isEmpty())
        {
            p->setNgayLap(ngayLap);
            cout << "Cap nhat ngay lap thanh cong" << endl;
        }
        if (!hanTraTien.isEmpty())
        {
            p->setHanTraTien(hanTraTien);
            cout << "Cap nhat han tra tien thanh cong" << endl;
        }
        if (choice != -1)
        {
            switch (choice)
            {
            case 1:
            {
                p->setTrangThai("da_thanh_toan");
                cout << "Cap nhat trang thai thanh cong!" << endl;
                Date ngayTraTien = InputHelper::getInputDate("Nhap ngay tra tien: ");
                if (!ngayTraTien.isEmpty())
                {
                    p->setNgayTraTien(ngayTraTien);
                    cout << "Cap nhat ngay tra tien thanh cong" << endl;
                }
                break;
            }
            case 2:
            {
                p->setTrangThai("chua_thanh_toan");
                cout << "Cap nhat trang thai thanh cong!" << endl;
                p->setNgayTraTien(Date());
                break;
            }
            default:
            {
                cout << "Lua chon khong hop le, vui long thu lai" << endl;
                break;
            }
            }
        }
    }
    repoHoaDon.writeToFile();
    cout << "Cap nhat hoa don thanh cong" << endl;
}
void HoaDonMenu::xoaHoaDon()
{
    string id = InputHelper::getInputString("Nhap ma hoa don: ");
    HoaDon *p = repoHoaDon.readByID(id);
    if (p == nullptr)
    {
        cout << "Ma hoa don khong hop le, vui long thu lai!" << endl;
        return;
    }
    else
    {
        cout << p->exportToString() << endl;
        string xacNhan = InputHelper::getInputString("Ban co chac muon xoa hoa don nay khong ? (y/n): ");
        if (xacNhan == "Y" || xacNhan == "y" || xacNhan == "YES" || xacNhan == "yes" || xacNhan == "Yes")
        {
            repoHoaDon.remove(id);
            repoHoaDon.writeToFile();
            cout << "Xoa hoa don thanh cong!" << endl;
        }
        else if (xacNhan == "N" || xacNhan == "n" || xacNhan == "no" || xacNhan == "NO" || xacNhan == "No")
        {
            cout << "Da huy thao tac xoa hoa don!" << endl;
            return;
        }
        else
            return;
    }
}
void HoaDonMenu::handleChoice(int choice)
{
    switch (choice)
    {
    case 1:
        xemDanhSach();
        break;
    case 2:
        themHoaDon();
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
        xoaHoaDon();
        break;
    default:
        cout << "Lua chon khong hop le, vui long thu lai!" << endl;
        break;
    }
}
void HoaDonMenu::printMenu()
{
    cout << "======QUAN LY HOA DON======" << endl;
    cout << "1. Xem danh sach hoa don" << endl;
    cout << "2. Them hoa don moi" << endl;
    cout << "3. Tim kiem hoa don" << endl;
    cout << "4. Sap xep danh sach hoa don" << endl;
    cout << "5. Cap nhat hoa don" << endl;
    cout << "6. Xoa hoa don" << endl;
}
