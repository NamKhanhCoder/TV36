// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Khac Hieu (Thanh vien 4)
// Use Case 2: Quan ly Hoa don (Menu dieu khien)
// ===============================================================
#include "HoaDonMenu.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include "Utils/TablePrinter.h"
#include "Utils/InputHelper.h"
#include "Utils/StringHelper.h"
using namespace std;
namespace
{
    const vector<int> widths = {12, 12, 12, 12, 12, 14, 15, 17};
    const vector<string> headers = {"Invoice ID", "Contract ID", "Customer ID", "Amount", "Issue Date", "Due Date", "Payment Date", "Status"};
}
// Ham khoi tao: khoi tao 3 repository (HoaDon, Customer, Contract) va nap du lieu tu file
HoaDonMenu::HoaDonMenu() : repoHoaDon("Data/hoadon.txt", "Data/hdn_counter.txt", "HDN"),
                           repoCustomer("Data/customers.txt", "Data/customers_counter.txt", "KH"),
                           repoContract("Data/contracts.txt", "Data/contracts_counter.txt", "HD")
{
    repoHoaDon.loadFromFile();
    repoCustomer.loadFromFile();
    repoContract.loadFromFile();
}
// Chuc nang xem danh sach: hien thi bang hoa don 8 cot can bang TablePrinter va tinh tong doanh thu
void HoaDonMenu::xemDanhSach()
{
    if (repoHoaDon.readAll().empty())
    {
        cout << "[Error] Invoice list is empty!" << endl;
        return;
    }

    TablePrinter::printTable(widths, headers, repoHoaDon.exportToVector());
    vector<HoaDon> danhSach = repoHoaDon.readAll();
    double tongDoanhThu = 0;
    int s = danhSach.size();
    for (int i = 0; i < s; i++)
    {
        tongDoanhThu += danhSach[i].getSoTien();
    }
    cout << "Total revenue: " << (long long)tongDoanhThu << " VND" << endl;
    InputHelper::getInputOptionalString("Press any key to go back: ");
    SystemHelper::clearScreen();
}
// Chuc nang them hoa don: validate ma hop dong, ma khach hang, so tien va trang thai thanh toan
void HoaDonMenu::themHoaDon()
{
    HoaDon hoaDonMoi;
    string maHopDong;
    while (true)
    {
        maHopDong = InputHelper::getInputString("Enter contract ID: ");
        maHopDong = convertToUpper(maHopDong);
        try
        {
            if (!kiemTraHopDongTonTai(maHopDong))
            {
                cout << "[Error] Contract ID not found or invalid, try again!" << endl;
                continue;
            }
            else
            {
                hoaDonMoi.setMaHopDong(maHopDong);
                break;
            }
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[INVOICEMenu] Invalid contract id input during creation: " + string(e.what()));
        }
    }
    Contract *pcontract = repoContract.readByID(maHopDong);
    string maKhachHang = pcontract->get_customerID();
    hoaDonMoi.setMaKhachHang(maKhachHang);
    cout << "Customer representative: " << maKhachHang << endl;
    Date ngayLap = Date::getCurrentDate();
    if (ngayLap > pcontract->get_endDate())
    {
        cout << "The date of issuance cannot be later than the end date of contract, please try again!" << endl;
        return;
    }
    // Lay thong tin hop dong tu repoContract de tu dong trich xuat ma khach hang, dam bao rang buoc toan ven khoa ngoai
    else
        hoaDonMoi.setNgayLap(ngayLap);
    while (true)
    {
        double soTien = InputHelper::getInputDouble("Enter amount: ");
        try
        {
            hoaDonMoi.setSoTien(soTien);
            break;
        }
        catch (const exception &e)
        {
            cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
            LOG("[INVOICEMenu] Invalid amount input during creation: " + string(e.what()));
        }
    }

    Date hanTraTien;
    while (true)
    {
        hanTraTien = InputHelper::getInputDate("Enter due date (DD/MM/YYYY): ");
        if (hanTraTien < ngayLap || hanTraTien > pcontract->get_endDate())
        {
            cout << "The due date cannot be earlier than the date of issuance and later than end date of contract, please try again!" << endl;
            continue;
        }
        break;
    }
    hoaDonMoi.setHanTraTien(hanTraTien);
    string trangThai;
    Date ngayTraTien;
    while (true)
    {
        ngayTraTien = InputHelper::getInputOptionalDate("Enter pay date (DD/MM/YYYY): ");
        if (ngayTraTien.isEmpty())
        {
            trangThai = "Unpaid";
        }
        else
        {
            if (ngayTraTien < ngayLap)
            {
                cout << "The payment date cannot be earlier than the date of issuance, please try again!" << endl;
                continue;
            }
            else
            {
                trangThai = "Paid";
            }
        }
        break;
    }
    hoaDonMoi.setNgayTraTien(ngayTraTien);
    hoaDonMoi.setTrangThai(trangThai);

    string id = repoHoaDon.generateNextID();
    hoaDonMoi.setID(id);
    repoHoaDon.create(hoaDonMoi);
    cout << "Invoice created successfully!" << endl;
    repoHoaDon.writeToFile();
    InputHelper::getInputOptionalString("Press any key to go back: ");
    SystemHelper::clearScreen();
}
// Chuc nang tim kiem hoa don: tim theo ma HDN, ma HD, ma KH, ngay lap, han tra, ngay tra, trang thai
void HoaDonMenu::timKiem()
{
    cout << "=====Search invoice=====" << endl;
    cout << "1. Search by invoice ID" << endl;
    cout << "2. Search by contract ID" << endl;
    cout << "3. Search by customer ID" << endl;
    cout << "4. Search by issue date" << endl;
    cout << "5. Search by due date" << endl;
    cout << "6. Search by payment date" << endl;
    cout << "7. Search by payment status" << endl;
    cout << "Enter any other option to go back." << endl;
    int choice = InputHelper::getInputInt("Enter your choice: ");
    repoHoaDon.loadFromFile();
    const vector<HoaDon> &danhSach = repoHoaDon.readAll();
    int s = danhSach.size();
    switch (choice)
    {
    case 1:
    {
        string id = InputHelper::getInputString("Enter invoice ID (HDNXXX): ");
        id = convertToUpper(id);
        HoaDon *p = repoHoaDon.readByID(id);
        if (p == nullptr)
        {
            cout << "[Error] Invoice not found, ID: " << id << endl;
            cout << "Try again!" << endl;
            break;
        }
        else
            TablePrinter::printSingle(widths, headers, p->exportToVector());
        break;
    }
    case 2:
    {
        string maHopDong = InputHelper::getInputString("Enter contract ID (HDXXX): ");
        maHopDong = convertToUpper(maHopDong);
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
            cout << "[Error] No invoice found with contract ID: " << maHopDong << endl;
            cout << "Try again!" << endl;
            break;
        }
        else
            TablePrinter::printTable(widths, headers, data);
        break;
    }
    case 3:
    {
        string maKhachHang = InputHelper::getInputString("Enter customer ID (KHXXX): ");
        maKhachHang = convertToUpper(maKhachHang);
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
            cout << "[Error] No invoice found with customer ID: " << maKhachHang << endl;
            cout << "Try again!" << endl;
            break;
        }
        else
            TablePrinter::printTable(widths, headers, data);
        break;
    }
    case 4:
    {
        Date ngayLap = InputHelper::getInputDate("Enter issue date (DD/MM/YYYY): ");
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
            cout << "[Error] No invoice found with issue date: " << ngayLap.toString() << endl;
            cout << "Try again!" << endl;
            break;
        }
        else
            TablePrinter::printTable(widths, headers, data);
        break;
    }
    case 5:
    {
        Date hanTraTien = InputHelper::getInputDate("Enter due date (DD/MM/YYYY): ");
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
            cout << "[Error] No invoice found with due date: " << hanTraTien.toString() << endl;
            cout << "Try again!" << endl;
            break;
        }
        else
            TablePrinter::printTable(widths, headers, data);
        break;
    }
    case 6:
    {
        Date ngayTraTien = InputHelper::getInputOptionalDate("Enter payment date (DD/MM/YYYY): ");
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
                cout << "[Error] No invoice found with payment date: " << "NONE" << endl;
                cout << "Try again!" << endl;
                break;
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
                cout << "[Error] No invoice found with payment date: " << ngayTraTien.toString() << endl;
                cout << "Try again!" << endl;
                break;
            }
            else
                TablePrinter::printTable(widths, headers, data);
        }

        break;
    }

    case 7:
    {
        int choice = InputHelper::getInputInt("Enter payment status (1. Paid / 2. Unpaid): ");
        vector<vector<string>> data;
        if (choice == 1)
        {
            for (int i = 0; i < s; i++)
            {
                if (danhSach[i].getTrangThai() == "Paid")
                {
                    data.push_back(danhSach[i].exportToVector());
                }
            }
        }
        else if (choice == 2)
        {
            for (int i = 0; i < s; i++)
            {
                if (danhSach[i].getTrangThai() == "Unpaid")
                {
                    data.push_back(danhSach[i].exportToVector());
                }
            }
        }
        else
        {
            cout << "[Error] Invalid option, try again!" << endl;
            break;
        }
        if (!data.empty())
        {
            TablePrinter::printTable(widths, headers, data);
        }
        else
            cout << "[Error] No invoice found with that status!" << endl;
        break;
    }
    default:
    {
        SystemHelper::clearScreen();
        break;
    }
    }
    InputHelper::getInputOptionalString("Press any key to go back: ");
    SystemHelper::clearScreen();
}
// Cac ham so sanh phuc vu sap xep danh sach hoa don theo ngay thang
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
// Chuc nang sap xep hoa don: sap xep theo so tien, ngay lap, han tra, ngay tra va cap nhat file
void HoaDonMenu::sapXep()
{
    cout << "=====Sort invoice list=====" << endl;
    cout << "1. Sort by amount (Min -> Max)" << endl;
    cout << "2. Sort by amount (Max -> Min)" << endl;
    cout << "3. Sort by issue date (newest -> oldest)" << endl;
    cout << "4. Sort by issue date (oldest -> newest)" << endl;
    cout << "5. Sort by due date (newest -> oldest)" << endl;
    cout << "6. Sort by due date (oldest -> newest)" << endl;
    cout << "7. Sort by payment date (newest -> oldest)" << endl;
    cout << "8. Sort by payment date (oldest -> newest)" << endl;
    cout << "Press any key to go back: " << endl;
    int choice = InputHelper::getInputInt("Enter your choice: ");
    vector<HoaDon> danhSach = repoHoaDon.readAll();
    int s = danhSach.size();
    switch (choice)
    {
    case 1:
    {
        sort(danhSach.begin(), danhSach.end());
        ofstream tenFile("Data/hoadon.txt");
        cout << "Invoice list after sorting: " << endl;
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repoHoaDon.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Sort order saved to file successfully!" << endl;
        break;
    }
    case 2:
    {
        sort(danhSach.begin(), danhSach.end(), greater<HoaDon>());
        ofstream tenFile("Data/hoadon.txt");
        cout << "Invoice list after sorting: " << endl;
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repoHoaDon.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Sort order saved to file successfully!" << endl;
        break;
    }
    case 3:
    {
        sort(danhSach.begin(), danhSach.end(), soSanhTheoNgayLapGiamDan);
        ofstream tenFile("Data/hoadon.txt");
        cout << "Invoice list after sorting: " << endl;
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repoHoaDon.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Sort order saved to file successfully!" << endl;
        break;
    }
    case 4:
    {
        sort(danhSach.begin(), danhSach.end(), soSanhTheoNgayLapTangDan);
        ofstream tenFile("Data/hoadon.txt");
        cout << "Invoice list after sorting: " << endl;
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repoHoaDon.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Sort order saved to file successfully!" << endl;
        break;
    }
    case 5:
    {
        sort(danhSach.begin(), danhSach.end(), soSanhTheoHanTraTienGiamDan);
        ofstream tenFile("Data/hoadon.txt");
        cout << "Invoice list after sorting: " << endl;
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repoHoaDon.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Sort order saved to file successfully!" << endl;
        break;
    }
    case 6:
    {
        sort(danhSach.begin(), danhSach.end(), soSanhTheoHanTraTienTangDan);
        ofstream tenFile("Data/hoadon.txt");
        cout << "Invoice list after sorting: " << endl;
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repoHoaDon.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Sort order saved to file successfully!" << endl;
        break;
    }
    case 7:
    {
        sort(danhSach.begin(), danhSach.end(), soSanhTheoNgayTraTienGiamDan);
        ofstream tenFile("Data/hoadon.txt");
        cout << "Invoice list after sorting: " << endl;
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repoHoaDon.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Sort order saved to file successfully!" << endl;
        break;
    }
    case 8:
    {
        sort(danhSach.begin(), danhSach.end(), soSanhTheoNgayTraTienTangDan);
        ofstream tenFile("Data/hoadon.txt");
        cout << "Invoice list after sorting: " << endl;
        vector<vector<string>> data;
        for (int i = 0; i < s; i++)
        {
            data.push_back(danhSach[i].exportToVector());
            tenFile << danhSach[i].exportToString() << endl;
        }
        tenFile.close();
        repoHoaDon.loadFromFile();
        TablePrinter::printTable(widths, headers, data);
        cout << "Sort order saved to file successfully!" << endl;
        break;
    }
    default:
        SystemHelper::clearScreen();
        break;
    }
    InputHelper::getInputOptionalString("Press any key to go back: ");
    SystemHelper::clearScreen();
}
// Chuc nang cap nhat hoa don: tim theo ma HDN, hien thi thong tin hien tai va cap nhat tung truong
void HoaDonMenu::capNhat()
{
    string id = InputHelper::getInputString("Enter invoice ID: ");
    id = convertToUpper(id);
    HoaDon *p = repoHoaDon.readByID(id);

    if (p == nullptr)
    {
        cout << "[Error] Invalid invoice ID, try again!" << endl;
        InputHelper::getInputOptionalString("Press any key to go back: ");
        return;
    }
    else
    {
        Contract *pcontract = repoContract.readByID(p->getMaHopDong());
        cout << "=====Current invoice information=====" << endl;
        cout << "Invoice ID: " << id << endl;
        cout << "Contract ID: " << p->getMaHopDong() << endl;
        cout << "Customer ID: " << p->getMaKhachHang() << endl;
        cout << "Amount: " << p->getSoTien() << endl;
        cout << "Issue date: " << (p->getNgayLap()).toString() << endl;
        cout << "Due date: " << (p->getHanTraTien()).toString() << endl;
        cout << "Payment date: ";
        Date strNgayTra = p->getNgayTraTien();
        if (strNgayTra.isEmpty())
        {
            cout << "NONE" << endl;
        }
        else
        {
            cout << strNgayTra.toString() << endl;
            ;
        }
        cout << "Status: " << p->getTrangThai() << endl;

        while (true)
        {
            double soTien = InputHelper::getInputOptionalDouble("Enter amount: ");
            if (soTien == -1.0)
                break;
            try
            {
                p->setSoTien(soTien);
                cout << "Amount updated successfully" << endl;
                break;
            }
            catch (const exception &e)
            {
                cout << "[Validation Error] " << e.what() << "\nPlease try again.\n";
                LOG("[INVOICEMenu] Invalid amount input during update: " + string(e.what()));
            }
        }
        while (true)
        {
            Date hanTraTien = InputHelper::getInputOptionalDate("Enter due date: ");
            if (hanTraTien.isEmpty())
                break;
            else
            {
                if (hanTraTien < p->getNgayLap() || (pcontract != nullptr && hanTraTien > pcontract->get_endDate()))
                {
                    cout << "The due date cannot be earlier than the date of issuance, please try again!" << endl;
                    continue;
                }
                else
                {
                    p->setHanTraTien(hanTraTien);
                    cout << "Due date updated successfully" << endl;
                    break;
                }
            }
        }
        if (p->getTrangThai() != "Paid")
        {
            while (true)
            {
                Date ngayTraTien = InputHelper::getInputOptionalDate("Enter payment date: ");
                if (ngayTraTien.isEmpty())
                    break;
                else
                {
                    if (ngayTraTien < p->getNgayLap())
                    {
                        cout << "The payment date cannot be earlier than the date of issuance, please try again!" << endl;
                        continue;
                    }
                    else
                    {
                        p->setNgayTraTien(ngayTraTien);
                        cout << "Pay date updated successfully" << endl;
                        p->setTrangThai("Paid");
                        cout << "Payment status updated successfully!" << endl;
                        if (ngayTraTien > p->getHanTraTien())
                        {
                            cout << "[Notice] Late payment: Payment was made after the due date!" << endl;
                        }
                        break;
                    }
                }
            }
        }
        else
        {
            cout << "[Notice] Invoice is already Paid, payment date and status cannot be modified!" << endl;
        }
    }
    repoHoaDon.writeToFile();
    cout << "Invoice updated successfully" << endl;
    InputHelper::getInputOptionalString("Press any key to go back: ");
    SystemHelper::clearScreen();
}
// Chuc nang xoa hoa don: tim theo ma HDN, xac nhan Y/N truoc khi xoa va luu lai file
void HoaDonMenu::xoaHoaDon()
{   
    repoHoaDon.loadFromFile();
    string id = InputHelper::getInputString("Enter invoice ID: ");
    id = convertToUpper(id);
    HoaDon *p = repoHoaDon.readByID(id);
    if (p == nullptr)
    {
        cout << "[Error] Invalid invoice ID, try again!" << endl;
        InputHelper::getInputOptionalString("Press any key to go back: ");
        return;
    }
    else
    {
        cout << p->exportToString() << endl;
        string xacNhan = InputHelper::getInputString("Are you sure you want to delete this invoice? (y/n): ");
        if (xacNhan == "Y" || xacNhan == "y" || xacNhan == "YES" || xacNhan == "yes" || xacNhan == "Yes")
        {
            repoHoaDon.remove(id);
            repoHoaDon.writeToFile();
            cout << "Invoice deleted successfully!" << endl;
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
// Xu ly lua chon chuc nang tu menu quan ly hoa don
void HoaDonMenu::handleChoice(int choice)
{
    switch (choice)
    {
    case 2:
        xemDanhSach();
        break;
    case 1:
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
        cout << "[Error] Invalid option, try again!" << endl;
        break;
    }
}
// Hien thi giao dien menu quan ly hoa don
void HoaDonMenu::printMenu()
{
    SystemHelper::clearScreen();
    cout << "=====Invoice Management=====" << endl;
    cout << "1. Add new invoice" << endl;
    cout << "2. View all invoices" << endl;
    cout << "3. Search invoice" << endl;
    cout << "4. Sort invoice list" << endl;
    cout << "5. Update invoice" << endl;
    cout << "6. Delete invoice" << endl;
    // phan xoa hoa don ko duoc thuc hien do quy dinh cua phap luat
}
