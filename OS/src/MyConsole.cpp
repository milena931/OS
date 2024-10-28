//
// Created by os on 9/4/23.
//
#include "../h/MyConsole.hpp"


MyConsole& MyConsole::getObj() {  //da bi klasa bila singleton
    static MyConsole obj;
    return obj;
}

void MyConsole::myConsoleHandler() {
    int interrupt_request = plic_claim();

    while(*((char*)CONSOLE_STATUS)  & CONSOLE_RX_STATUS_BIT){
        char c = *((char*)CONSOLE_RX_DATA);
        MyConsole::getObj().inputBuffer->buff_putc(c);
    }

    plic_complete(interrupt_request);
}

void MyConsole::output(void* arg) {
    do{
        while(*((char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT){
            char c = MyConsole::getObj().outputBuffer->buff_getc();
            *((char*) CONSOLE_TX_DATA) = c;
        }
    }while(true);
}

