#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/SupportTicket.h"
#include "Entities/Contract.h"

class SupportTicketMenu : public BaseMenu
{
private:
    Repository<SupportTicket> repo;
    Repository<Contract> repoContract;

    void createTicket();
    void readTicket();
    void updateTicket();
    void deleteTicket();

    TicketPriority inputTicketPriority(bool isOptional = false, TicketPriority current = TicketPriority::BINH_THUONG);

protected:
    void printMenu() override;
    void handleChoice(int choice) override;

public:
    SupportTicketMenu();
};
