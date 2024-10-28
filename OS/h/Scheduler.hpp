//
// Created by os on 8/4/23.
//

#ifndef PROJECT_BASE_V1_1_SCHEDULER_HPP
#define PROJECT_BASE_V1_1_SCHEDULER_HPP

#include "../h/MemoryAllocator.hpp"
#include "TCB.hpp"

class TCB;

struct Queue{  //struktura koja opisuje red
    TCB* thread;
    Queue* next;
};


class Scheduler{
public:
    static void put(TCB* thread);
    static TCB*  get();
    static Scheduler& getObj();  //ovo nam treba kao kod alokatora jer klasa treba da bude singleton
private:
    static Queue* schedulerQueue;
    static Queue* schedulerQueueEnd;

};


#endif //PROJECT_BASE_V1_1_SCHEDULER_HPP
