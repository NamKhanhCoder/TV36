#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/Channel.h"

class ChannelMenu : public BaseMenu
{
private:
    Repository<Channel> repo;
    bool dataLoaded;
    void inputFields(Channel& item, bool editing);
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
