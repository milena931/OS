//
// Created by os on 8/4/23.
//

#include "../h/TCB.hpp"

TCB* TCB::runningThread = nullptr;
uint64 TCB::timeSliceCounter = 0;
uint64 TCB::numberOfUnfinishedThreads = 0;
int TCB::globalId = 0;


TCB *TCB::createThread(TCB** handle, TCB::Body bd, void* arg, void* stack) {   //funkcija koja pravi nit ne startuje je
    TCB* nit = (TCB*) MemoryAllocator::getObj().alloc((uint64)(sizeof(TCB) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
    nit->body = bd;
    nit->arguments = arg;
    nit->stack = (nit->body != nullptr ? (char*)stack : nullptr);
    if(nit->body == nullptr) MemoryAllocator::getObj().free(stack);
    nit->context.ra = (uint64) &thread_wrapper;
    nit->context.sp = (uint64) (nit->stack!=nullptr ? (uint64) &nit->stack[DEFAULT_STACK_SIZE] : 0);
    nit->finished = false;
    nit->blocked = false;
    nit->timeSlice = DEFAULT_TIME_SLICE;
    nit->started = false;
    nit->asleep = false;
    nit->id = ++globalId;

    if(nit->body != nullptr){
        nit->started = true;
        Scheduler::getObj().put(nit);
        TCB::numberOfUnfinishedThreads++;
    }
    *handle = nit;

    //kernelPrintString("Kreirana nit ");
    //kernelPrintInt(nit->getId());
    //kernelPrintString("\n");

    return nit;
}


void TCB::yeald() {
    __asm__ volatile("li a0, 0x13");  //dispatch
    __asm__ volatile("ecall");
}



void TCB::dispatch() {
    TCB* oldThread = runningThread;
    if(!oldThread -> isFinished() && !oldThread->blocked && !oldThread->asleep){
        Scheduler::getObj().put(oldThread);
    }
    runningThread = Scheduler::getObj().get();
    contextSwitch(&oldThread->context, &runningThread->context);
}


void TCB::thread_wrapper() {  //omotac u kom se pokrece t-ja body sa argumentima arg
    Riscv::popSppSpie();
    runningThread->body(runningThread->arguments);
    //runningThread->setFinished(true);
    thread_exit();
    //TCB::yeald();
    //thread_dispatch();
}