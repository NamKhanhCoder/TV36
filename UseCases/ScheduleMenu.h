// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Hong Anh (NotaSpookyGhost)
// Use Case 5: Quan ly lich phat song (Schedule Menu)
// ===============================================================
#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/Schedule.h"
#include "Entities/Channel.h"
#include "Entities/NoiDungVOD.h"

// Menu CRUD lich, kiem tra kenh ton tai va tranh trung gio tren cung kenh.
class ScheduleMenu : public BaseMenu
{
private:
    Repository<Schedule> repo;
    Repository<Channel> repoChannel;
    Repository<NoiDungVOD> repoVod;
    bool inputFields(Schedule& item, bool editing);
    void loadSchedules();
    string findConflict(const Schedule& item);
    void sortSchedules();
    void createSchedule();
    void readSchedule();
    void updateSchedule();
    void deleteSchedule();

protected:
    // Hien thi cac lua chon CRUD va sap xep.
    void printMenu() override;
    // Nap du lieu moi, thuc hien lua chon va bat ngoai le.
    void handleChoice(int choice) override;

public:
    // Khoi tao cac Repository va nap danh sach lich.
    ScheduleMenu();
};
