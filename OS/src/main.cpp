//
// Created by os on 8/8/23.
//

#include "../lib/hw.h"
#include "../h/syscall_c.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/Riscv.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/myPrinting.hpp"

void userMain();

void myTest();

void myTestWrapper(void * arg){
    myTest();
}

void UserMainWrapper(void * arg){
    userMain();
}

void fja(void* arg){
    while(true){
        thread_dispatch();
    }
}

void main(){
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap + 1);
/*
    MyConsole::getObj().getcBuffer = (MyBuffer*) MemoryAllocator::getObj().alloc((sizeof(MyBuffer) + MEM_BLOCK_SIZE -1) / MEM_BLOCK_SIZE);
    MyConsole::getObj().putcBuffer = (MyBuffer*) MemoryAllocator::getObj().alloc((sizeof(MyBuffer) + MEM_BLOCK_SIZE -1) / MEM_BLOCK_SIZE);
*/
    MyConsole::getObj().inputBuffer = new MyBuffer();
    MyConsole::getObj().outputBuffer = new MyBuffer();

    TCB* mainThread;
    thread_create(&mainThread, nullptr, nullptr);
    TCB::runningThread = mainThread;
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);  //dozvoljavam prekide
    Riscv::setUserMode(true);

    TCB* idle;
    thread_create(&idle, fja, nullptr);

    TCB* outputThread;
    thread_create(&outputThread, MyConsole::output, nullptr);

    //Riscv::ms_sstatus(Riscv::SSTATUS_SIE);  //dozvoljavam prekide
    //Riscv::setUserMode(true);

    TCB* userMainThread;
    thread_create(&userMainThread, UserMainWrapper, nullptr);

    //TCB* myTestThread;
    //thread_create(&myTestThread, myTestWrapper, nullptr);

    //thread_join(userMainThread);

    //thread_join(outputThread);

    while(MyConsole::getObj().outputBuffer->isEmpty() == false || userMainThread->isFinished() == false){
        thread_dispatch();
    }

/*
    while(MyConsole::getObj().outputBuffer->isEmpty() == false || myTestThread->isFinished() == false){
        thread_dispatch();
    }
*/


    mem_free(outputThread);
    mem_free(MyConsole::getObj().inputBuffer);
    mem_free(MyConsole::getObj().outputBuffer);
}