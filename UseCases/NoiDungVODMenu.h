#pragma once
#include "Utils/SystemHelper.h"
#include "Repositories/Repository.h"
#include "Entities/NoiDungVOD.h"
#include "UseCases/BaseMenu.h"
#include <vector>
#include <string>
using namespace std;
class NoiDungVODMenu : public BaseMenu{
    private:
            Repository<NoiDungVOD> repo;
    public:
            NoiDungVODMenu();
            void xemDanhSach();
            void themNoiDung();
            void timKiem();
            void sapXep();
            void capNhat();
            void xoaNoiDung();
            void printMenu() override;
            void handleChoice(int choice) override;
};
