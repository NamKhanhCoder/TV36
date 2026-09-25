#pragma once
#include <vector>
#include <string>
#include <fstream>
#include "Utils/Debugger.h"

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
    void loadIDCount()
    {
        ifstream file(counter_path);
        int counter = 0;
        if (file.is_open())
        {
            file >> counter;
            file.close();
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
        ifstream file(file_path);

        if (file.is_open() == false) // cant open case
        {
            LOG("[Repository] Failed to open " + file_path);
            return;
        }

        items.clear();
        string line;

        while (getline(file, line)) // read line by line
        {
            if (line.empty() == true)
            {
                LOG("[Repository] White line detected, skipping");
                continue;
            }
            T item;
            item.parseFromString(line);
            items.push_back(item);
        }
        file.close();
        LOG("[Repository] Successfully loaded from   file: " + file_path);
    }
    void writeToFile() // convert and save to file
    {
        LOG("[Repository] Attempt saving to " + file_path);
        ofstream file(file_path);

        if (file.is_open() == false) // cant open case
        {
            LOG("[Repository] Failed to open " + file_path);
            return;
        }

        for (const T &item : items)
        {
            file << item.exportToString() << endl;
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
    const vector<T> &readAll() const
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