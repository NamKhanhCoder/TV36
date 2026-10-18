#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/ChannelCategory.h"
#include "Entities/Channel.h"
#include <string>
#include <vector>

using namespace std;

class ChannelCategoryMenu : public BaseMenu
{
private:
    Repository<ChannelCategory> repo;
    Repository<Channel> repoChannel;

    void printCategoryTable(const vector<vector<string>> &data);

    bool isNameExisted(const string &name, const string &excludeID = "");
    bool hasChannelsInCategory(const string &categoryID);

    // CRUD
    void createChannelCategory();
    void readChannelCategory();
    void updateChannelCategory();
    void deleteChannelCategory();

    void updateName(ChannelCategory *category);
    void updateDescription(ChannelCategory *category);

protected:
    void printMenu() override;
    void handleChoice(int choice) override;

public:
    ChannelCategoryMenu();
};