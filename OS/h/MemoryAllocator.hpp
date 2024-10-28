//
// Created by os on 8/3/23.
//

#ifndef _MemoryAllocator_
#define _MemoryAllocator_

#include "../lib/hw.h"
#include "../lib/console.h"

struct FreeMem{     //struktura koja opisuje jednostruku ulancanu listu slobodnih segmenata
    size_t size;  //u blokovima

    FreeMem* next;
    FreeMem* prev;

};



class MemoryAllocator{
public:
    void* alloc(size_t size);
    int free(void* addr);
    static MemoryAllocator& getObj();

private:
    FreeMem* freeMemHead = nullptr;
    bool head = false; //za proveru da li je ikada posatvljen head za slobodan prostor

    int tryToJoin(FreeMem* node);



};



#endif



