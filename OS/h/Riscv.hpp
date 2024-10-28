//
// Created by os on 8/8/23.
//

#ifndef PROJECT_BASE_V1_1_RISCV_HPP
#define PROJECT_BASE_V1_1_RISCV_HPP

#include "../lib/hw.h"
#include "../h/MemoryAllocator.hpp"
#include "../h/TCB.hpp"
#include "../h/_sem.hpp"
#include "../h/MyConsole.hpp"


struct List{
    TCB* thread;
    List* next;
    time_t sleepingTime;
};

class Riscv{
public:

    static void popSppSpie();  //vadi spp i spie bit iz statusnog registra
    static void setUserMode(bool mode);

    //citanje i upis u registar koji opisuje razlog ulaska u sistemski rezim - scause
    static uint64 r_scause();
    static void w_scause(uint64 scause);
    //citanje i upis registra sepc - sacuvana vrednost registra pc u korisnickom rezimu
    static uint64 r_sepc();
    static void w_sepc(uint64 sepc);
    //citanje i upis u registar stvec koji predstavlja adresu prekidna rutine poravnatu na 4 bajta
    static uint64 r_stvec();
    static void w_stvec(uint64 stvec);
    //citanje i upis u statusni registar - sstatus
    static uint64 r_sstatus();
    static void w_sstatus(uint64 sstatus);


    enum MaskSip{
        SIP_SSIE= (1<<1),
        SIP_STIE = (1<<5),
        SIP_SEIE = (1<<9)
    };

    enum MasksSstatus{    //maske za SSTATUS registar (
        SSTATUS_SIE = (1<<1),
        SSTATUS_SPIE = (1<<5),
        SSTATUS_SPP = (1<<8)
    };

    static void mc_sip(uint64 mask);

    static void ms_sip(uint64 mask);

    static void mc_sstatus(uint64 mask);

    static void ms_sstatus(uint64 mask);

    static void supervisorTrap();

private:
    static bool user_mode;
    static List* threads_asleep_head;
    static List* threads_asleep_tail;

    //metode za rad sa listom uspavanih niti
    static void addSleeping(TCB* thread, time_t time);
    static void removeSleepingThreads();
    static void decTime();

    static void interruptHandle();  //funkcija koja ce da obradjuje razlicite sistemske pozive; pozivace se u prekidnoj rutini - supervisorTrap

    static void timerTrapHandler();
    static void externalTrapHandler();
    static void ecallTrapHandler();
};


inline uint64 Riscv::r_scause(){
    uint64 volatile scause;
    __asm__ volatile ("csrr %[scause], scause" : [scause] "=r"(scause));
    return scause;
}

inline void Riscv::w_scause(uint64 scause){
    __asm__ volatile ("csrw scause, %[scause]" : : [scause] "r"(scause));
}


inline uint64 Riscv::r_sepc() {
    uint64 volatile sepc;
    __asm__ volatile("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
    return sepc;
}

inline void Riscv::w_sepc(uint64 sepc) {
    __asm__ volatile("csrw sepc, %[sepc]" : : [sepc] "r"(sepc));
}

inline uint64 Riscv::r_stvec() {
    uint64 volatile stvec;
    __asm__ volatile("csrr %[stvec], stvec" : [stvec] "=r"(stvec));
    return stvec;
}

inline void Riscv::w_stvec(uint64 stvec) {
    __asm__ volatile("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
}

inline uint64 Riscv::r_sstatus() {
    uint64 volatile sstatus;
    __asm__ volatile("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void Riscv::w_sstatus(uint64 sstatus) {
    __asm__ volatile("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

inline void Riscv::mc_sip(uint64 mask) {
    __asm__ volatile("csrc sip, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::ms_sip(uint64 mask){
    __asm__ volatile("csrs sip, %[mask]" : : [mask]"r"(mask));
}

inline void Riscv::mc_sstatus(uint64 mask){
    __asm__ volatile("csrc sstatus, %[mask]" : : [mask]"r"(mask));
}

inline void Riscv::ms_sstatus(uint64 mask){
    __asm__ volatile("csrs sstatus, %[mask]" : : [mask]"r"(mask));
}


#endif //PROJECT_BASE_V1_1_RISCV_HPP
