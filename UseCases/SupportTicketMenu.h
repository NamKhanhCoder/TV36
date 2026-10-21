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

/**
 * @brief Lop menu dieu khien giao dien console cho chuc nang quan ly Phieu ho tro ky thuat (SupportTicket).
 */
class SupportTicketMenu : public BaseMenu
{
private:
    Repository<SupportTicket> repo;
    Repository<Contract> repoContract;

    /**
     * @brief Chuc nang tiep nhan va tao moi mot phieu ho tro.
     */
    void createTicket();

    /**
     * @brief Chuc nang xem danh sach va tim kiem phieu ho tro theo ma.
     */
    void readTicket();

    /**
     * @brief Chuc nang sap xep phieu ho tro theo Contract, Priority va Status.
     */
    void sortTicket();

    /**
     * @brief Chuc nang cap nhat phieu ho tro (khoa sua neu phieu DA_DONG, tu dong dong phieu khi DA_DONG).
     */
    void updateTicket();

    /**
     * @brief Chuc nang xoa phieu ho tro (chi cho phep xoa khi o trang thai TIEP_NHAN).
     */
    void deleteTicket();

    TicketPriority inputTicketPriority(bool isOptional = false, TicketPriority current = TicketPriority::BINH_THUONG);
    TicketStatus inputTicketStatus(bool isOptional = false, TicketStatus current = TicketStatus::TIEP_NHAN);

protected:
    void printMenu() override;
    void handleChoice(int choice) override;

public:
    /**
     * @brief Constructor nap cac repository phu vu nghiep vu phieu ho tro.
     */
    SupportTicketMenu();
};
