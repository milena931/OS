//
// Created by os on 8/3/23.
//


#include "../h/MemoryAllocator.hpp"

void* MemoryAllocator::alloc(size_t size) {  //trazi slobodan segment po first fit algoritmu
    if (head == false) {
        head = true;
        freeMemHead = (FreeMem *) (HEAP_START_ADDR);
        freeMemHead->next = freeMemHead->prev = nullptr;
        freeMemHead->size = ((uint64) ((char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR)-sizeof(FreeMem))/MEM_BLOCK_SIZE; //prvobitna velicina slobodnog segmenta je ceo heap ali posto je u blokovima deli se sa MEM_BLOCK_SIZE
                                                                                                                           //mora se oduzeti i sizeof(FreeMem) jer je zauzet prostor za jednu strukturu nakon njenog alociranja
    }

    void *ret_value = nullptr;
    for (FreeMem *current = freeMemHead; current != nullptr; current = current->next) {
        if(current->size < size) continue; //trazi se odgovarajuci segment
        //pronadjen segment i sad imamo 2 slucaja: kada se zauzima ceo segment i kada ostaje slobodan segment da visi (tako se izbegava interna fragmentacija)
        if(current->size - size <= sizeof(FreeMem)){ //zauzima se ceo segment
            ret_value = current;
            if(current->prev) current->prev->next = current->next;
            else freeMemHead = current->next;
            if(current->next) current->next->prev = current->prev;
            break;
        }else{ //ne zauzima se ceo segment
            ret_value = current;
            FreeMem* fragment = (FreeMem*)((uint64*)current + size*MEM_BLOCK_SIZE);  //alocira se novi fragment koji pocinje od adrese pomerene za size blokova od trenutne
            if(current->prev) current->prev->next = fragment;
            else freeMemHead = fragment;
            if(current->next) current->next->prev = fragment;
            fragment->prev = current->prev;
            fragment->next = current->next;
            fragment->size = current->size - size;  //velicina slobodnog fragmenta se smanjila
            current->size = size;
            break;
        }
    }
    //nakon ove for petlje pronadjen je odgovarajuci fragment koji se vraca kao povratna vrednost (ako segment nije nadjen vraca se nullptr)
    return (char*) ret_value + sizeof(FreeMem);  //mora da se doda i prostor koji je struktura zauzimala
}

int MemoryAllocator::free(void* addr) {
    //prvo treba proveriti da li se adresa nalazi u heap-u
    if((char*)addr < (char*)HEAP_START_ADDR || (char*)addr > (char*)HEAP_END_ADDR) return -1;
    FreeMem* current = nullptr;
    if(!freeMemHead || (char*)addr<(char*)freeMemHead){
        current = nullptr;
    }else{
        for(current = freeMemHead; current->next!=nullptr && (char*)addr > (char*) current->next; current = current->next); //trazimo posle kog cvora treba da umetnemo novi slobodan segment
    }
    FreeMem* novi = (FreeMem*)((char*)addr - sizeof(FreeMem));
    novi->prev=current;
    if(current) novi->prev = current->next;
    else novi->next = freeMemHead;
    if(novi->next) novi->next->prev = novi;
    if(current) current->next = novi;
    else freeMemHead = novi;

    //nakon oslobadjanja treba pokusati spajanje dodatog slobodnog segmenta sa sledecim ili porethodnim ako je to moguce
    if(novi != nullptr) tryToJoin(novi);  //spajanje sa sledecim
    if(current != nullptr) tryToJoin(current); //spajanje sa prethodnim

    return 0;  //uspesno izvrseno
}


int MemoryAllocator::tryToJoin(FreeMem *node) {   //ova funkcija u sustini spaja tekuci cvor node sa sledecim ako je to moguce i potrebno
    if(node == nullptr) return -1;   //greska

    if(node->next && (uint64*)node + node->size * MEM_BLOCK_SIZE == (uint64*) node->next){  //spajanje
        node->size += node->next->size;
        node->next = node->next->next;
        if(node->next) node->next->prev = node;
        return 0;
    }

    return -1;      //ako se doslo do ovog dela koda znaci da se spajanje nije dogodilo pa se vraca negativna vrednost
}

MemoryAllocator& MemoryAllocator::getObj() {
    static MemoryAllocator obj;
    return obj;
}