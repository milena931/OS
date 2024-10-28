//
// Created by os on 9/4/23.
//

#ifndef PROJECT_BASE_V1_1_MYCONSOLE_HPP
#define PROJECT_BASE_V1_1_MYCONSOLE_HPP


#include "../h/MyBuffer.hpp"

class MyBuffer;

class MyConsole{   //klasa koja slizi kap apstrakcija konzole
public:
    void myConsoleHandler();      //hendler za prekid od ove konzole
    static void output(void* arg);
    static MyConsole& getObj();   //metoda koja kao kod alokatora i skedzulera sluzi da obezbedi da klasa bude singleton



    MyBuffer* inputBuffer;  //getc
    MyBuffer* outputBuffer;  //putc
private:
    friend class Riscv;

};


#endif //PROJECT_BASE_V1_1_MYCONSOLE_HPP
