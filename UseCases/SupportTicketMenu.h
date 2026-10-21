// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Duong Bao Long (B24DCVT231)
// Use Case 2: Quan ly Phieu ho tro ki thuat Support Ticket (Menu dieu khien)
// ===============================================================
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
    void sortTicket();
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
