//
// Created by os on 8/4/23.
//

#include "../h/Scheduler.hpp"

Queue* Scheduler::schedulerQueue = nullptr;
Queue* Scheduler::schedulerQueueEnd = nullptr;


TCB* Scheduler::get() {    //metoda koja vadi nit iz skedzulera sa pocetka reda (prva ubacena nit)
    if(schedulerQueue != nullptr){
        Queue* free = schedulerQueue;
        TCB* ret = schedulerQueue->thread;
        schedulerQueue = schedulerQueue -> next;
        if(schedulerQueue == nullptr) schedulerQueueEnd = schedulerQueue;
        MemoryAllocator::getObj().free((void*)free);
        return ret;
    }else return nullptr;
}

void Scheduler::put(TCB *thread) {  //metoda koja stavlja nit u skedzuler na kraj reda
    Queue* temp = (Queue*) MemoryAllocator::getObj().alloc((sizeof(Queue) + MEM_BLOCK_SIZE -1) / MEM_BLOCK_SIZE);
    temp->thread = thread;
    temp->next= nullptr;
    if(schedulerQueueEnd == nullptr){
        schedulerQueue = schedulerQueueEnd = temp;
    }else{
        schedulerQueueEnd->next = temp;
        schedulerQueueEnd = schedulerQueueEnd->next;
    }
}

Scheduler& Scheduler::getObj() {  //da bi klasa bila singleton treba joj metoda koja ce da povede racuna da postoji samo jedan objekat klase
                                  //kao kod MemoryAllocatora
    static Scheduler obj;
    return obj;
}