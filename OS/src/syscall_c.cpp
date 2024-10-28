
#include "../h/syscall_c.hpp"

void* mem_alloc(size_t size){
    size_t finalSize;
    finalSize = (size + MEM_BLOCK_SIZE -1)/MEM_BLOCK_SIZE;  //pretvaranje velicine bajtova u blokove

    //postavljanje argumenata
    __asm__ volatile ("mv a1, %[sz]" : : [sz] "r" (finalSize));
    __asm__ volatile ("li a0, 0x01");
    __asm__ volatile ("ecall");  //ulazak u prekidnu rutinu

    //uint64* retVal;
    uint64 retVal;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r"(retVal));

    return (void*) retVal;

}

int mem_free(void* mem){
    void* newMem = mem;

    __asm__ volatile ("mv a1, %[ar]" :: [ar] "r" (newMem));
    __asm__ volatile ("li a0, 0x02");
    __asm__ volatile ("ecall");  //ulazak u prekidnu rutinu

    uint64 retVal;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r"(retVal));

    return retVal;

}

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg){
    thread_t* rucka = handle;
    *rucka = nullptr;
    void(*rutina)(void*) = start_routine;
    void* argumenti = arg;
    void* stek = mem_alloc(DEFAULT_STACK_SIZE);
    __asm__ volatile("mv a4, %[stek]" :: [stek]"r"(stek));
    __asm__ volatile("mv a3, %[argumenti]" :: [argumenti]"r"(argumenti));
    __asm__ volatile ("mv a2, %[rutina]" :: [rutina]"r"(rutina));
    __asm__ volatile ("mv a1, %[rucka]" :: [rucka]"r"(rucka));
    __asm__ volatile ("li a0, 0x11");
    __asm__ volatile ("ecall");  //ulazak u prekidnu rutinu

    int ret;
    __asm__ volatile("mv %[ret], a0" : [ret]"=r"(ret));
    return ret;
}

int thread_exit(){
    __asm__ volatile("li a0, 0x12");
    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %[ret], a0" : [ret]"=r"(ret));
    return ret;
}

void thread_dispatch(){
    __asm__ volatile("li a0, 0x13");
    __asm__ volatile("ecall");
}

void thread_join(thread_t handle){
    thread_t rucka= handle;
    __asm__ volatile("mv a1, %[rucka]" :: [rucka]"r"(rucka));
    __asm__ volatile("li a0, 0x14");
    __asm__ volatile("ecall");
}

int sem_open(sem_t* handle, unsigned init ){
    sem_t* rucka = handle;
    unsigned arg2 = init;
    *rucka = nullptr;
    __asm__ volatile("mv a2, %[arg2]" : : [arg2]"r"(arg2));
    __asm__ volatile(" mv a1, %[rucka]" : : [rucka]"r"(rucka));
    __asm__ volatile("li a0, 0x21");
    __asm__ volatile ("ecall");

    int ret;
    __asm__ volatile("mv %[ret], a0" : [ret]"=r"(ret));
    return ret;
}

int sem_close(sem_t handle){
    sem_t rucka = handle;
    __asm__ volatile(" mv a1, %[rucka]" : : [rucka]"r"(rucka));
    __asm__ volatile("li a0, 0x22");
    __asm__ volatile ("ecall");

    int ret;
    __asm__ volatile("mv %[ret], a0" : [ret]"=r"(ret));
    return ret;
}

int sem_wait(sem_t handle){
    sem_t rucka = handle;
    __asm__ volatile(" mv a1, %[rucka]" : : [rucka]"r"(rucka));
    __asm__ volatile("li a0, 0x23");
    __asm__ volatile ("ecall");

    int ret;
    __asm__ volatile("mv %[ret], a0" : [ret]"=r"(ret));
    return ret;
}

int sem_signal(sem_t handle){
    sem_t rucka = handle;
    __asm__ volatile(" mv a1, %[rucka]" : : [rucka]"r"(rucka));
    __asm__ volatile("li a0, 0x24");
    __asm__ volatile ("ecall");

    int ret;
    __asm__ volatile("mv %[ret], a0" : [ret]"=r"(ret));
    return ret;
}

int time_sleep(time_t time){
    time_t vreme = time;
    __asm__ volatile("mv a1, %[vreme]" :: [vreme]"r"(vreme));
    __asm__ volatile("li a0, 0x31");
    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %[ret], a0" : [ret]"=r"(ret));
    return ret;
}

char getc(){
    __asm__ volatile("li a0, 0x41");
    __asm__ volatile("ecall");

    char c;
    __asm__ volatile("mv %[c], a0" : [c]"=r"(c));
    return c;
}

void putc(char c){
    char arg = c;
    __asm__ volatile("mv a1, %[arg]" :: [arg]"r"(arg));
    __asm__ volatile("li a0, 0x42");
    __asm__ volatile("ecall");
}