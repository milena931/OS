//
// Created by os on 8/4/23.
//

#include "../h/Riscv.hpp"
#include "../h/myPrinting.hpp"

bool Riscv::user_mode = false;
List* Riscv::threads_asleep_head = nullptr;
List* Riscv::threads_asleep_tail = nullptr;


void Riscv::ecallTrapHandler() {
    volatile uint64 scause = r_scause();  //pamtimo scause registar da bismo kasnije mogli da restauriramo njegovu vrednost
    volatile uint64 arg1;            //argumente skidamo odmah iz a1, a2 i a3 registra da bismo ocuvali potrebne vrednosti
    __asm__ volatile("mv %[arg1], a1" : [arg1]"=r"(arg1));
    volatile uint64 arg2;
    __asm__ volatile("mv %[arg2], a2" : [arg2]"=r"(arg2));
    volatile uint64 arg3;
    __asm__ volatile("mv %[arg3], a3" : [arg3]"=r"(arg3));
    volatile uint64 arg4;
    __asm__ volatile("mv %[arg4], a4" : [arg4]"=r"(arg4));
    if(scause == 0x08 || scause == 0x09){    //sistemski pozivi koji se obradjuju u prekidnoj rutini - ovde se ulazi sa ecall iz korisnickog ili sistemskog rezima
        volatile uint64 sepc = r_sepc();
        sepc += 0x04;  //sepc uvecan za 4 mora da se upise u sepc na kraju rutine
        volatile uint64 sstatus = r_sstatus();  //sstatus se pamti da bi se kao scause restarurirao na kraju

        uint64 sis;
        __asm__ volatile("mv %[rx], a0": [rx]"=r" (sis) );   //u registru a0 se nalazi kod sistemskog poziva


        if(sis == 0x01){   //mem_alloc
            void* pov = MemoryAllocator::getObj().alloc(arg1);  //poziv funkicje i cuvanje povratne vrednosti da bi se ona smestila u a0 regustar
            __asm__ volatile("mv a0, %[pov]" : : [pov]"r"(pov));

        }else if(sis == 0x02){ //mem_free
            int r = MemoryAllocator::getObj().free((void*)arg1);
            __asm__ volatile("mv a0, %[r]" : : [r]"r"(r));


        }else if(sis == 0x11){//thread_create
            void* stek = (void*)arg4;
            void* argumenti = (void*) arg3;
            TCB::Body funkcija = (TCB::Body) arg2;
            TCB** rucka = (TCB**) arg1;

            TCB* nit = TCB::createThread(rucka, funkcija, argumenti, stek);

            int ret;
            if(nit == nullptr) ret = -1;
            else ret = 0;
            __asm__ volatile("mv a0, %[ret]" : : [ret]"r"(ret));


        }else if(sis == 0x12){  //exit
            TCB::runningThread->setFinished(true);
            TCB::timeSliceCounter = 0;
            int ret;
            if(TCB::runningThread->isFinished()) {
                ret = 0;
            }else ret = -1;
            __asm__ volatile("mv a0, %[ret]" : : [ret]"r"(ret));
            TCB::dispatch();


        }else if(sis == 0x13){  //dispatch
            TCB::dispatch();


        }else if(sis == 0x14){  //join
            TCB* rucka = (TCB*)arg1;
            while(!rucka->isFinished()) {
                TCB::dispatch();
            }


        }else if(sis == 0x21) { //sem_open
            _sem **handle = (_sem **) arg1;
            unsigned init = (unsigned) arg2;

            int ret = _sem::open(handle, init);
            __asm__ volatile("mv a0, %[ret]":: [ret]"r"(ret));


        }else if(sis == 0x22){ //sem_close
            _sem* sem = (_sem*) arg1;
            int ret = _sem::close(sem);
            __asm__ volatile("mv a0, %[ret]" :: [ret]"r"(ret));


        }else if(sis == 0x23){ //sem_wait
            _sem* sem = (_sem*) arg1;
            int ret = _sem::wait(sem);
            __asm__ volatile("mv a0, %[ret]" :: [ret]"r"(ret));


        }else if(sis == 0x24){  //sem_signal
            _sem* sem = (_sem*) arg1;
            int ret = _sem::signal(sem);
            __asm__ volatile("mv a0, %[ret]" :: [ret]"r"(ret));


        }else if(sis == 0x31){  //time_sleep
            time_t time = (time_t) arg1;
            if(time){
                TCB::runningThread->asleep = true;
                addSleeping(TCB::runningThread, time);
            }
            TCB::dispatch();


        }else if(sis == 0x41){  //getc
            //char c = __getc();
            while(MyConsole::getObj().inputBuffer->isEmpty()){
                TCB::timeSliceCounter = 0;
                TCB::dispatch();
            }
            char c = MyConsole::getObj().inputBuffer->buff_getc();
            __asm__ volatile("mv a0, %[c]" :: [c]"r"(c));


        }else if(sis == 0x42){  //putc
            volatile char c = arg1;
            //__putc(c);
            while(MyConsole::getObj().outputBuffer->isFull()){
                TCB::timeSliceCounter = 0;
                TCB::dispatch();
            }
            MyConsole::getObj().outputBuffer->buff_putc(c);


        }else{
            TCB::timeSliceCounter = 0;
            TCB::dispatch();

        }
        w_sepc(sepc);  //mora na kraju da se upise sepc uvecan za 4 da se program ne bi vrteo na instrukciji koja je izazvala prekid
        w_sstatus(sstatus);
        mc_sip(SIP_SSIE);

    }else{  //neki exception
        //ovde scause i sepc treba ispisati

        kernelPrintString("\n***Exception***\nException scause:  ");
        kernelPrintInt(scause);
        kernelPrintString("\nException sepc:  ");
        kernelPrintInt(r_sepc());
        kernelPrintString("\n");

        while(true){
            MyConsole::output(nullptr);
        }

    }
}


void Riscv::timerTrapHandler() {   //hendler za prekid od tajmera
    decTime();
    if(threads_asleep_head != nullptr && threads_asleep_head->sleepingTime == 0){
        removeSleepingThreads();  //vadimo niti koje vise ne treba da budu uspavane nakon isteka jos jedne periode vremena
    }

    mc_sip(SIP_SSIE);  //gasi sistem software interrupt bit
    TCB::timeSliceCounter++;
    if(TCB::timeSliceCounter >= TCB::runningThread->getTimeSlice()){
        volatile uint64 sepc = r_sepc();
        volatile uint64 sstatus = r_sstatus();
        TCB::timeSliceCounter = 0;
        TCB::dispatch();
        w_sepc(sepc);
        w_sstatus(sstatus);
    }
    //mc_sip(SIP_SSIE);  //gasi sistem software interrupt bit


}

void Riscv::externalTrapHandler() {   //hendler za spoljasnji prekid verovetno od konzole
    //console_handler();  //ovo treba moja konzola posle da bude
    MyConsole::getObj().myConsoleHandler();   //moj hendler za konzolu
}


void Riscv::popSppSpie() {
    __asm__ volatile("csrw sepc, ra");  //da se ra u ovom ifu ne bi promenilo
    if(user_mode){   //da bi se osiguralo da se korisnicki kod izvsava u korisnickom rezimu
        mc_sstatus(Riscv::SSTATUS_SPP);
    }else{
        ms_sstatus(Riscv::SSTATUS_SPP);
    }
    //__asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::setUserMode(bool mode) {
    user_mode = mode;
}

void Riscv::addSleeping(TCB *thread, time_t time) {  //metoda koja dodaje uspavanu nit u ulancanu listu uspavanih niti na kraj liste
    List* novi = (List*) MemoryAllocator::getObj().alloc((uint64)(sizeof(List) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
    novi->thread = thread;
    novi->sleepingTime = time;
    novi->next = nullptr;
    if(threads_asleep_head == nullptr){   //prva uspavana nit
        threads_asleep_head = novi;  //head je prvi element
        threads_asleep_tail = novi;  //tail je poslednji element
    }else{
        List* prev = nullptr;
        List* curr = threads_asleep_head;

        //prolazimo kroz listu uspavanih niti i trazimo na koje mesto treba nit da se ubaci da
        //bi lista bila uredjena rastuce po vremenu spavanja niti
        //za to vreme se vreme spavanja niti koja se ubacuje smanjuje za vreme koje spava trenutna nit koja se preskace
        while(curr != threads_asleep_tail && novi->sleepingTime >= curr->sleepingTime ){
            prev = curr;
            novi->sleepingTime -= curr->sleepingTime;
            curr = curr->next;
        }
        if(curr != threads_asleep_tail){  //ako se dodaje u sred liste
            if(prev == nullptr){  //u ovom slucaju je to prvi element koji se dodaje
                novi->next = curr;
                threads_asleep_head = novi;
            }else{
                prev->next = novi;  //u ovom slucaju se element dodaje negde u sredini liste
                novi->next = curr;
            }
            curr->sleepingTime -= novi->sleepingTime;
        }else{
            if(novi->sleepingTime >= threads_asleep_tail->sleepingTime){  //sko se dodaje na kraj liste
                novi->sleepingTime -= threads_asleep_tail->sleepingTime;
                threads_asleep_tail->next = novi;
                threads_asleep_tail = threads_asleep_tail->next;
            }else{
                if(prev == nullptr){
                    novi->next = curr;
                    threads_asleep_head = novi;
                }else{
                    prev->next = novi;
                    novi->next = threads_asleep_tail;
                }
                curr->sleepingTime -= novi->sleepingTime;
                //threads_asleep_tail->sleepingTime -= novi->sleepingTime;
            }
        }

    }
}

void Riscv::removeSleepingThreads() {    //metoda koja iz liste uspavanih niti sklanja niti koje vise ne treba da budu uspavane
    while(threads_asleep_head != nullptr && threads_asleep_head->sleepingTime == 0){
        List* free = threads_asleep_head;
        threads_asleep_head = threads_asleep_head->next;
        if(!threads_asleep_head) threads_asleep_tail = nullptr;
        free->thread->asleep = false;
        Scheduler::getObj().put(free->thread);
        MemoryAllocator::getObj().free(free);
    }
}

void Riscv::decTime() {
    if(threads_asleep_head){
        threads_asleep_head->sleepingTime --;
    }
}