// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Pham Quoc Trung (B24DCVT361)
// Use Case 3: Quan ly The loai kenh (Channel Category Menu)
// ===============================================================
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

    // Cac chuc nang CRUD co kiem soat toan ven du lieu
    void createChannelCategory();
    void readChannelCategory();
    void sortCategoriesByName();
    void updateChannelCategory();
    void deleteChannelCategory();

protected:
    // Hien thi menu va xu ly lua chon dieu huong
    void printMenu() override;
    void handleChoice(int choice) override;

public:
    // Khoi tao menu the loai va nap du lieu tu file
    ChannelCategoryMenu();
};
