// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193)
// Framework cot loi: Repository<T> (Khuon mau quan ly du lieu & file)
// ===============================================================
#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include "Utils/Debugger.h"
#include <stdexcept>
#include "Utils/InputHelper.h"
using namespace std;

template <class T>
class Repository
{
private:
    string padding(int number, int width)
    {
        string res = to_string(number);
        if (res.length() < width)
        {
            res = string(width - res.length(), '0') + res;
        }
        return res;
    }
    void ensureFileExists(const string &path, const string &default_content = "")
    {
        std::error_code ec;
        if (std::filesystem::is_directory(path, ec))
        {
            LOG("[Repository] Cannot create file: path is a directory: " + path);
            throw std::runtime_error("[Repository] Path is a directory: " + path);
        }

        ifstream check(path);
        if (!check.is_open())
        {
            try
            {
                std::filesystem::path p(path);
                if (p.has_parent_path() && !std::filesystem::exists(p.parent_path(), ec))
                {
                    std::filesystem::create_directories(p.parent_path(), ec);
                }
            }
            catch (...)
            {
            }

            ofstream create_file(path);
            if (create_file.is_open())
            {
                if (!default_content.empty())
                {
                    create_file << default_content;
                }
                create_file.close();
                LOG("[Repository] Created file: " + path);
            }
            else
            {
                LOG("[Repository] Failed to create file: " + path);
                throw std::runtime_error("[Repository] Failed to create file: " + path);
            }
        }
    }
    void loadIDCount()
    {
        std::error_code ec;
        if (std::filesystem::is_directory(counter_path, ec))
        {
            LOG("[Repository] Counter path is a directory: " + counter_path);
            throw std::runtime_error("[Repository] Counter path is a directory: " + counter_path);
        }

        ifstream file(counter_path);
        int counter = 0;
        if (file.is_open())
        {
            file >> counter;
            file.close();
        }
        else
        {
            // Neu file counter chua ton tai, tu tao file voi gia tri 0
            ensureFileExists(counter_path, "0\n");
            counter = 0;
        }
        this->id_count = counter;
        // in case the file is not openned, ID will be 0
        LOG("[Repository] Read counter = " + to_string(counter) + " from " + counter_path);
    }
protected:
    vector<T> items;
    string file_path;
    string counter_path;

    string id_prefix;
    int id_count;

public:
    string generateNextID()
    {
        this->id_count++;
        ofstream file(counter_path, ios::trunc);
        if (file.is_open())
        {
            file << this->id_count;
            file.close();
        }
        return id_prefix + padding(id_count, 3); // KH001, KH002...
    }
    // 1. Constructor
    Repository(const string &path, const string &counter_path, const string &id_prefix) // initialize with directory
    {
        LOG("[Repository] Repository initialized");
        this->file_path = path;
        this->counter_path = counter_path;
        this->id_prefix = id_prefix;
        this->loadIDCount();
    }
    virtual ~Repository() {}
    void loadFromFile() // overwrite the entire vector with data from file
    {
        LOG("[Repository] Attempt loading from " + file_path);
        std::error_code ec;

        // 1. Chặn trường hợp đường dẫn là thư mục (F15)
        if (std::filesystem::is_directory(file_path, ec))
        {
            LOG("[Repository] Path is a directory, not a file: " + file_path);
            throw std::runtime_error("[Repository] Path is a directory, cannot load: " + file_path);
        }

        // 2. Chỉ tự tạo file mới khi file thực sự chưa tồn tại
        if (!std::filesystem::exists(file_path, ec))
        {
            LOG("[Repository] File not found: " + file_path + ", creating new empty file...");
            ensureFileExists(file_path);
            items.clear();
            return;
        }

        // 3. Nếu đường dẫn tồn tại nhưng không phải file thông thường (regular file)
        if (!std::filesystem::is_regular_file(file_path, ec))
        {
            LOG("[Repository] Path is not a regular file: " + file_path);
            throw std::runtime_error("[Repository] Path is not a regular file: " + file_path);
        }

        ifstream file(file_path);

        if (file.is_open() == false) // Lỗi quyền truy cập hoặc lỗi I/O
        {
            LOG("[Repository] Failed to open file for reading: " + file_path);
            throw std::runtime_error("[Repository] Cannot access file for reading: " + file_path);
        }

        items.clear();
        string line;

        while (getline(file, line)) // read line by line
        {
            if (line.empty()) continue;

            T item;
            try
            {
                item.parseFromString(line);
            }
            catch(const std::exception& err)
            {
                cout << "[Warning] Data corrupted: " << err.what() << endl;
                InputHelper::getInputOptionalString("Press Enter to continue...");
                continue;
            }
            
            items.push_back(item);
        }
        file.close();
        LOG("[Repository] Successfully loaded from file: " + file_path);
    }
    void writeToFile() // convert and save to file
    {
        LOG("[Repository] Attempt saving to " + file_path);
        std::error_code ec;
        if (std::filesystem::is_directory(file_path, ec))
        {
            LOG("[Repository] Path is a directory, not a file: " + file_path);
            throw std::runtime_error("[Repository] Path is a directory, cannot write: " + file_path);
        }

        ensureFileExists(file_path);
        ofstream file(file_path);

        if (file.is_open() == false) // cant open case
        {
            LOG("[Repository] Failed to open file for writing: " + file_path);
            throw std::runtime_error("[Repository] Cannot access file for writing: " + file_path);
        }

        for (const T &item : items)
        {
            file << item.exportToString() << endl;
        }
        if (file.fail())
        {
            file.close();
            LOG("[Repository] Stream failure while writing to file: " + file_path);
            throw std::runtime_error("[Repository] Stream failure while writing to file: " + file_path);
        }
        file.close();
        LOG("[Repository] Successfully saved to file: " + file_path);
    }
    vector<vector<string>> exportToVector() const
    {
        vector<vector<string>> result;
        for (const T &item : items)
            result.push_back(item.exportToVector());
        return result;
    }
    // create
    bool create(const T &item)
    {
        LOG("[Repository] Attempt creating item");
        for (int i = 0; i < items.size(); i++)
        {
            if (item.getID() == items[i].getID())
            {
                LOG("[Repository] Failed, duplication detected with ID" + item.getID());
                return false;
            }
        }

        items.push_back(item);
        LOG("[Repository] Succeed creating item");
        return true;
    }
    // read
    T *readByID(const string &target_ID) // return the pointer of the position of the T
    {
        LOG("[Repository] Attempt finding item with ID " + target_ID);
        for (int i = 0; i < items.size(); i++)
        {
            if (items[i].getID() == target_ID)
            {
                LOG("[Repository] Item found, returning pointer");
                return &items[i];
            }
        }
        LOG("[Repository] Item not found, returning nullpointer");
        return nullptr;
    }
    const vector<T> &readAll()
    {
        LOG("[Repository] Succeed return items");
        return items;
    }
    // update
    bool update(const T &item)
    {
        LOG("[Repository] Attempt updating item with ID " + item.getID());
        for (int i = 0; i < items.size(); i++)
        {
            if (items[i].getID() == item.getID())
            {
                items[i] = item;
                LOG("[Repository] Successfuly updated item with ID " + item.getID());
                return true;
            }
        }
        LOG("[Repository] Failed to update, ID not found: " + item.getID());
        return false;
    }
    // delete
    bool remove(const string &target_ID)
    {
        LOG("[Repository] Attempt removing item with ID " + target_ID);
        for (auto it = items.begin(); it != items.end(); it++)
        {
            if (it->getID() == target_ID)
            {

                items.erase(it);
                LOG("[Repository] Successfully removed item with ID " + target_ID);
                return true;
            }
        }
        LOG("[Repository] Failed to remove, ID not found: " + target_ID);
        return false;
    }
};
