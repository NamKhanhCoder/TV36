#pragma once
#include "UseCases/BaseMenu.h"
#include "Repositories/Repository.h"
#include "Entities/SupportTicket.h"

class SupportTicketMenu : public BaseMenu
{
private:
    Repository<SupportTicket> repo;
    void createTicket();
    void readTicket();
    void updateTicket();
    void deleteTicket();

    TicketPriority inputTicketPriority(bool isOptional = false, TicketPriority current = TicketPriority::BINH_THUONG);
    TicketStatus inputTicketStatus(bool isOptional = false, TicketStatus current = TicketStatus::TIEP_NHAN);

protected:
    void printMenu() override;
    void handleChoice(int choice) override;

public:
    SupportTicketMenu();
};

