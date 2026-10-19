#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/Channel.h"
#include "Entities/ChannelCategory.h"
#include "Entities/Package.h"
#include "Entities/Schedule.h"

class ChannelMenu : public BaseMenu
{
private:
    Repository<Channel> repo;
    Repository<ChannelCategory> repoCategory;
    Repository<Package> repoPackage;
    Repository<Schedule> repoSchedule;
    bool dataLoaded;
    bool inputFields(Channel& item, bool editing);
    void loadChannels();
    bool isNumberUsed(int number, const string& excludeID);
    void sortChannels();
    void createChannel();
    void readChannel();
    void updateChannel();
    void deleteChannel();
    void saveChanges();

protected:
    void printMenu() override;
    void handleChoice(int choice) override;

public:
    ChannelMenu();
};
