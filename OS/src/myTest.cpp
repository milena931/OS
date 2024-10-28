//
// Created by os on 9/24/23.
//

#include "../h/syscall_c.hpp"
#include "../test/printing.hpp"
#include "../h/TCB.hpp"

static sem_t semaphore;
static sem_t wait;
static int N = 50;

void workerBody(void* arg){
    for(int i=0; i<3; i++){
        sem_wait(semaphore);
        printString("Usao u kriticnu sekciju : nit ");
        printInt(TCB::runningThread->getId());
        putc('\n');


/*
        for(int j= 0; j < 10000 * TCB::runningThread->getId(); j++){
            //busy wait
        }
*/

        //ili sleep u zavisnosti od toga sta radi i kako menja kontekst
        //time_sleep((TCB::runningThread->getId() + i) % 5);
        time_sleep(i * 10 + TCB::runningThread->getId() % 5);

        sem_signal(semaphore);
        printString("Izasao iz kriticne sekcije : nit ");
        printInt(TCB::runningThread->getId());
        putc('\n');
        thread_dispatch();
    }
    sem_signal(wait);
}

void myTest(){
    printString("\n\n***Pocetak mog testa***\n\n");

    sem_open(&semaphore, N/10);
    sem_open(&wait, 0);
    TCB* threads[N];

    for(int i=0; i<N; i++){
        thread_create(&threads[i], workerBody, nullptr);
    }


    for(int i=0; i < N; i++){  //ceka da se sve niti zavrse
        sem_wait(wait);
    }



    printString("\n***Kraj mog testa***\n\n");

    sem_close(wait);
    sem_close(semaphore);

}