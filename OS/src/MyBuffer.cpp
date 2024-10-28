//
// Created by os on 9/4/23.
//

#include "../h/MyBuffer.hpp"


char MyBuffer::buff_getc() {
    _sem::wait(item_available);
    char c = buff[head];
    head =(head + 1) % BUFFER_SIZE;
    numOfChars--;
    _sem::signal(space_available);
    return c;
}

void MyBuffer::buff_putc(char c) {
    _sem::wait(space_available);
    if(numOfChars < BUFFER_SIZE){
        buff[tail] = c;
        tail = (tail + 1) % BUFFER_SIZE;
        numOfChars++;
        _sem::signal(item_available);
    }
}

MyBuffer::MyBuffer(){
    numOfChars = head = tail = 0;
    sem_open(&item_available, 0);
    sem_open(&space_available, BUFFER_SIZE);
}

bool MyBuffer::isEmpty() {
    if(numOfChars) return false;
    return true;
}

bool MyBuffer::isFull() {
    if(numOfChars == BUFFER_SIZE) return true;
    else return false;
}