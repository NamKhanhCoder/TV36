#pragma once
#include "Entity.h"
#include <string>


class ChannelCategory : public Entity
{
private:
    string name;
    string description;

public:
    ChannelCategory() : Entity() {};
    ChannelCategory(string id, string name, string description);

    void setName(string &name);
    void setDescription(string &description);
    
    string getName() const;
    string getDescription() const;

    void parseFromString(const string &data) override;

    string exportToString() const override;
    vector<string> exportToVector() const override;
};