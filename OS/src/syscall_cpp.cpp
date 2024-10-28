//
// Created by os on 8/3/23.
//

#include "../h/syscall_cpp.hpp"
#include "../h/syscall_c.hpp"
#include "../h/Riscv.hpp"


void* operator new(size_t size){
    return mem_alloc(size);
}

void operator delete(void* size) noexcept{
    mem_free(size);
}

Thread::Thread(void (*body)(void *), void* arg) {  //konstruktor samo postavi polja u objektu - ne startuje nit!
    this->body = body;
    this->arg = arg;
    //thread_create(&myHandle, body, arg);
}

Thread::~Thread() {
    delete myHandle;
}

int Thread::start() {   //startuje nit
    if(body == nullptr){
        thread_create(&myHandle, &Thread::Thread_wrapper, this);
    }else{
        thread_create(&myHandle, body, (void*)arg);
    }
    //thread_start(myHandle);
    //Scheduler::getObj().put(myHandle);
    return 0;
}

void Thread::dispatch() {
    thread_dispatch();
}

void Thread::join(){
    thread_join(myHandle);
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

Thread::Thread() {
    this->body = nullptr;
    this->arg = nullptr;
    //thread_create(&myHandle, Thread::Thread_wrapper, this);
}

void Thread::Thread_wrapper(void *arg) {
    Thread* nit = (Thread*) arg;
    nit->run();
    //thread_exit();
    //thread_dispatch();
}

Semaphore::Semaphore(unsigned init) {
    sem_open(&myHandle, (unsigned)init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}

void PeriodicThread::terminate() {
    running = false;
}

