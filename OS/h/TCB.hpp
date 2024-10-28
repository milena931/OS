//
// Created by os on 8/4/23.
//

#ifndef PROJECT_BASE_V1_1_TCB_HPP
#define PROJECT_BASE_V1_1_TCB_HPP


#include "../lib/hw.h"
#include "../h/Scheduler.hpp"
#include "../h/Riscv.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/syscall_c.hpp"



class TCB{

public:
    using Body = void(*)(void*);

    static TCB* runningThread;  //trenutna running nit
    static TCB* createThread(TCB** handle,Body body, void* arg, void* stack);
    static void yeald();


    bool isFinished() const { return finished; }
    void setFinished(bool f){
        numberOfUnfinishedThreads--;
        this->finished = f;
    }
    uint64 getTimeSlice() const { return timeSlice; }
    int getId() const { return id; }


    ~TCB(){
        MemoryAllocator::getObj().free(stack);
    }

private:
/*
    TCB(Body body, void* arg):
        body(body),
        arguments(arg),
        stack(body != nullptr ? (uint64*)MemoryAllocator::getObj().alloc(DEFAULT_STACK_SIZE) : nullptr),
        timeSlice(DEFAULT_TIME_SLICE),
        context({
            (uint64) &thread_wrapper,
            (uint64) (stack != nullptr ? (uint64) stack[DEFAULT_STACK_SIZE] : 0)
        }),
        finished(false), blocked(false), closed(false), started(false){

        if(body != nullptr) { Scheduler::getObj().put(this); }

    }
*/

    friend class Riscv;
    friend class _sem;


    struct ThreadContext{
        uint64 ra;  //return address
        uint64 sp;  //stack pointer
    };

    Body body;
    void* arguments;
    char* stack = nullptr;
    uint64 timeSlice ;
    ThreadContext context;
    int id;

    bool finished;
    bool blocked;
    bool asleep;
    bool started;

    static void contextSwitch(ThreadContext* oldContext, ThreadContext* newContext);  //definicija ove funkcije je u asembleru
    static void dispatch();
    static void thread_wrapper();  //ova funkcija ce da poziva funkciju body
    static uint64 timeSliceCounter;
    static uint64 numberOfUnfinishedThreads;
    static int globalId;
};


#endif //PROJECT_BASE_V1_1_TCB_HPP
