#include "ChannelCategory.h"
#include "Utils/StringHelper.h"

ChannelCategory::ChannelCategory(string id, string name, string description) : Entity(id)
{
    this->name = name;
    this->description = description;
}

void ChannelCategory::setName(string &name)
{
    this->name = name;
}

void ChannelCategory::setDescription(string &description)
{
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
    vector<string> words;
    words=splitString(data);

    if (words.size() >= 3) 
    {
        id              = words[0];
        name            = words[1];
        description     = words[2];
    }
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