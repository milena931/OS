//
// Created by os on 8/4/23.
//

#include "../h/_sem.hpp"
#include "../test/printing.hpp"



int _sem::open(_sem **handle, unsigned init) {
    _sem* semafor = (_sem*) MemoryAllocator::getObj().alloc((sizeof(_sem) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
    *handle = semafor;
    semafor -> init = init;
    semafor -> queueHead = nullptr;
    semafor -> queueTail = nullptr;

    //int ret;
    //if(semafor == nullptr) ret = -1;
   // ret = 0;

    return 0;
}

int _sem::close(_sem *handle) {
    if(handle == nullptr) return -1;
    for(SemQueue* temp = handle->queueHead; temp!=nullptr; temp = temp->next){
        temp->nit->blocked = false;
        Scheduler::getObj().put(temp->nit);
    }
    int ret = MemoryAllocator::getObj().free(handle);

    return ret;
}

int _sem::signal(_sem *id) {
    if(id == nullptr) return -1;
    else{
        id->init++;
        if(id->init <= 0){
            TCB* nit = id->queuePop();
            nit->blocked = false;
            Scheduler::put(nit);
        }
    }

    return 0;
}

int _sem::wait(_sem *id) {
    if(id == nullptr) return -1;
    else{
        id->init--;
        if(id->init < 0){
            TCB::runningThread->blocked = true;
            id->queuePush(TCB::runningThread);
            //TCB::dispatch();
            thread_dispatch();
        }
    }
    return 0;
}

void _sem::queuePush(TCB *nit) {
    SemQueue* novi = (SemQueue*) MemoryAllocator::getObj().alloc((sizeof(SemQueue) + MEM_BLOCK_SIZE - 1)/ MEM_BLOCK_SIZE);
    novi->nit = nit;
    novi->next = nullptr;
    novi->prev = nullptr;
    if(!queueHead){
        queueHead = novi;
        queueTail = novi;
    }else{
        novi->prev = queueTail;
        queueTail->next = novi;
        queueTail = queueTail->next;
    }
    /*
    if(novi->nit->getId() > 3){
        printString("Blokirala se nit ID-");
        printInt(novi->nit->getId());
        printString("\n");
    }
     */
}

TCB* _sem::queuePop() {
    if(!queueHead) return nullptr;
    TCB* ret = queueHead->nit;
    SemQueue* free = queueHead;
    queueHead = queueHead->next;
    MemoryAllocator::getObj().free(free);

    return ret;
}