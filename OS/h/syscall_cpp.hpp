#ifndef _syscall_cpp
#define _syscall_cpp

#include "../h/syscall_c.hpp"

void* operator new(size_t size);
void operator delete(void* size) noexcept;


class Thread{
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread();

    int start();

    void join();

    static void dispatch();
    static int sleep(time_t time);

protected:
    Thread();
    virtual void run(){}

private:
    thread_t myHandle;
    void (*body)(void*);
    void* arg;

    static void Thread_wrapper(void* arg);

};

class Semaphore{
public:
    Semaphore(unsigned init = 1);
    virtual ~Semaphore();

    int wait();
    int signal();
private:
    sem_t myHandle;
};


class Console{
public:
    static char getc();
    static void putc(char c);
};


class PeriodicThread : public Thread{
public:
    void terminate();

protected:

    PeriodicThread(time_t p):Thread(), period(p){ running = true;}
    virtual void periodicActivation() {}

    void run() override{
        while(running){
            periodicActivation();
            time_sleep(period);
        }
    }

private:
    time_t period;
    bool running;
};


#endif