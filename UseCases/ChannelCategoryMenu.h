#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/ChannelCategory.h"

class ChannelCategoryMenu : public BaseMenu
{

private:
    Repository<ChannelCategory> repo;
    void createChannelCategory();
    void readChannelCategory();
    void updateChannelCategory();
    void deleteChannelCategory();
protected:
    void printMenu();
    void handleChoice(int choice);
public:
    ChannelCategoryMenu();
};