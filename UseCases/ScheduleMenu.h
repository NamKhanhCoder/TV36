#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/Schedule.h"

class ScheduleMenu : public BaseMenu
{
private:
    Repository<Schedule> repo;
    bool dataLoaded;
    void inputFields(Schedule& item, bool editing);
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
