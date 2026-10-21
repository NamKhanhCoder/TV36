// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Nguyen Hong Anh (NotaSpookyGhost)
// Use Case 4: Quan ly kenh truyen hinh (Channel Menu)
// ===============================================================
#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/Channel.h"
#include "Entities/ChannelCategory.h"
#include "Entities/Package.h"
#include "Entities/Schedule.h"

// Menu CRUD kenh, dung Repository chung va kiem tra rang buoc the loai/goi/lich.
class ChannelMenu : public BaseMenu
{
private:
    Repository<Channel> repo;
    Repository<ChannelCategory> repoCategory;
    Repository<Package> repoPackage;
    Repository<Schedule> repoSchedule;
    bool inputFields(Channel& item, bool editing);
    void loadChannels();
    bool isNumberUsed(int number, const string& excludeID);
    void sortChannels();
    void createChannel();
    void readChannel();
    void updateChannel();
    void deleteChannel();

protected:
    // Hien thi cac lua chon CRUD va sap xep.
    void printMenu() override;
    // Nap du lieu moi, thuc hien lua chon va bat ngoai le.
    void handleChoice(int choice) override;

public:
    // Khoi tao cac Repository va nap danh sach kenh.
    ChannelMenu();
};
