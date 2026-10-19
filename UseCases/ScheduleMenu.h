#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/Schedule.h"
#include "Entities/Channel.h"

class ScheduleMenu : public BaseMenu
{
private:
    Repository<Schedule> repo;
    Repository<Channel> repoChannel;
    bool dataLoaded;
    bool inputFields(Schedule& item, bool editing);
    void loadSchedules();
    string findConflict(const Schedule& item);
    void sortSchedules();
    void createSchedule();
    void readSchedule();
    void updateSchedule();
    void deleteSchedule();
    void saveChanges();

protected:
    void printMenu() override;
    void handleChoice(int choice) override;

public:
    ScheduleMenu();
};
