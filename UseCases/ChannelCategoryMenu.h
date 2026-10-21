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

    // CRUD
    void createChannelCategory();
    void readChannelCategory();
    void sortCategoriesByName();
    void updateChannelCategory();
    void deleteChannelCategory();

protected:
    void printMenu() override;
    void handleChoice(int choice) override;

public:
    ChannelCategoryMenu();
};