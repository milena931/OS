//
// Created by os on 8/4/23.
//

#ifndef PROJECT_BASE_V1_1__SEM_HPP
#define PROJECT_BASE_V1_1__SEM_HPP

#include "../h/syscall_c.hpp"
#include "../h/TCB.hpp"
#include "../h/myPrinting.hpp"


class TCB;

struct SemQueue{
    TCB* nit;
    SemQueue* next;
    SemQueue* prev;
};

class _sem{
public:
     static int open(_sem** handle, unsigned init);

     static int close(_sem* handle);

     static int signal(_sem* id);

     static int wait(_sem* id);

private:
    friend class Riscv;
    int init = 0;

    SemQueue* queueHead;
    SemQueue* queueTail;

    TCB* queuePop();
    void queuePush(TCB* nit);
};


#endif //PROJECT_BASE_V1_1__SEM_HPP
