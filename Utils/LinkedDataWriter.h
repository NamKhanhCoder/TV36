// Nguyen Hong Anh: luu lich/VOD qua file tam, chi thay file goc khi ghi xong.
#pragma once
#include "Repositories/Repository.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

template<class T>
void writeLinkedData(Repository<T>& repo, const std::string& filename)
{
    namespace fs = std::filesystem;
    fs::path target(filename);
    fs::path temporary = target;
    temporary += ".pending-" + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());
    try
    {
        std::ofstream file(temporary, std::ios::binary | std::ios::trunc);
        if (!file) throw std::runtime_error("Cannot open temporary data file.");
        for (const auto& item : repo.readAll()) file << item.exportToString() << '\n';
        file.flush();
        if (!file) throw std::runtime_error("Failed to write data file.");
        file.close();
        if (file.fail()) throw std::runtime_error("Failed to close data file.");
#ifdef _WIN32
        if (!MoveFileExW(temporary.c_str(), target.c_str(),
                         MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
            throw std::runtime_error("Cannot replace data file (Windows error " +
                                     std::to_string(GetLastError()) + ").");
#else
        fs::rename(temporary, target);
#endif
    }
    catch (...)
    {
        std::error_code ignored;
        fs::remove(temporary, ignored);
        throw;
    }
}
