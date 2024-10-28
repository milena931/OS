#include "../lib/hw.h"
#include "../lib/console.h"
//#include "../h/TCB.hpp"
//#include "../h/_sem.hpp"

void* mem_alloc(size_t size);  //ovde je size broj bajtova koji se najmanje alocira

int mem_free(void* mem);

class TCB;
typedef TCB* thread_t;

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg);

int thread_exit();

void thread_dispatch();

void thread_join(thread_t handle);

class _sem;
typedef _sem* sem_t;

int sem_open(sem_t* handle, unsigned init);

int sem_close(sem_t handle);

int sem_wait(sem_t id);

int sem_signal(sem_t id);

typedef unsigned long time_t;
int time_sleep(time_t time);

char getc();

void putc(char c);
