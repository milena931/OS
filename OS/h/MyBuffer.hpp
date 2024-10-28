//
// Created by os on 9/4/23.
//

#ifndef PROJECT_BASE_V1_1_MYBUFFER_HPP
#define PROJECT_BASE_V1_1_MYBUFFER_HPP

#include "../lib/hw.h"
#include "../h/_sem.hpp"
#include "../h/syscall_c.hpp"

class MyBuffer{
public:
    MyBuffer();
    char buff_getc();
    void buff_putc(char c);

    bool isEmpty();
    bool isFull();

private:
    static const uint64 BUFFER_SIZE = 1024;

    char buff[BUFFER_SIZE];
    uint64 head, tail, numOfChars;
    _sem* item_available;
    _sem* space_available;
};


#endif //PROJECT_BASE_V1_1_MYBUFFER_HPP
