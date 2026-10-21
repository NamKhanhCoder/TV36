// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Pham Quoc Trung (B24DCVT361)
// Use Case 3: Quan ly The loai kenh (Channel Category)
// ===============================================================
#include "ChannelCategory.h"
#include "Utils/StringHelper.h"
#include "Utils/Validator.h"
#include <stdexcept>

ChannelCategory::ChannelCategory(const string &id, const string &name, const string &description) : Entity(id)
{
    setName(name);
    setDescription(description);
}

void ChannelCategory::setName(const string &name)
{
    if (name.empty())
        throw invalid_argument("[ChannelCategory] Category name can not be empty!");
    if (name.find_first_of("|\r\n") != string::npos)
        throw invalid_argument("[ChannelCategory] Category name cannot contain '|' or newline characters!");
    this->name = name;
}

void ChannelCategory::setDescription(const string &description)
{
    if (description.find_first_of("|\r\n") != string::npos)
        throw invalid_argument("[ChannelCategory] Category description cannot contain '|' or newline characters!");
    this->description = description;
}

string ChannelCategory::getName() const
{
    return name;
}

string ChannelCategory::getDescription() const
{
    return description;
}

void ChannelCategory::parseFromString(const string &data)
{
    vector<string> words = splitString(data);

    if (words.size() != 3)
    {
        throw invalid_argument("[ChannelCategory] Invalid field count in line: " + data);
    }

    if (!Validator::isValidID(words[0], "TL", 5))
    {
        throw invalid_argument("[ChannelCategory] Invalid Category ID: " + words[0]);
    }

    id = words[0];
    setName(words[1]);

    string desc = words[2];
    if (!desc.empty() && desc.back() == '\r')
        desc.pop_back();
    setDescription(desc);
}

string ChannelCategory::exportToString() const
{
    return id + "|" + name + "|" + description;
}

vector<string> ChannelCategory::exportToVector() const
{
    vector<string> result = {id, name, description};
    return result;
}

bool ChannelCategory::operator<(const ChannelCategory &other) const
{
    return this->name < other.name;
}