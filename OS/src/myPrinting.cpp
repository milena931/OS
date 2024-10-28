#include "../h/myPrinting.hpp"

void kernelPrintString(char const  *string) {
    uint64 sstatus = Riscv::r_sstatus();
    Riscv::mc_sstatus(Riscv::SSTATUS_SIE);
    while (*string != '\0')
    {
        putc(*string);
        string++;
    }
    Riscv::ms_sstatus(sstatus & Riscv::SSTATUS_SIE ? Riscv::SSTATUS_SIE : 0 );
}

void kernelPrintInt(int br) {
    uint64 sstatus = Riscv::r_sstatus();
    Riscv::mc_sstatus(Riscv::SSTATUS_SIE);
    char buf[16];
    int i;
    uint x;
    char digits[] = "0123456789ABCDEF";

    x = br;

    i = 0;
    do{
        buf[i++] = digits[x % 10];
    }while((x /= 10) != 0);

    while(--i >= 0){ putc(buf[i]);}

    Riscv::ms_sstatus(sstatus & Riscv::SSTATUS_SIE ? Riscv::SSTATUS_SIE : 0 );
}

void myPrintString(char const * string){
    while (*string != '\0')
    {
        putc(*string);
        string++;
    }
}

void myPrintInt(int br){
    char buf[16];
    int i;
    uint x;
    char digits[] = "0123456789ABCDEF";

    x = br;

    i = 0;
    do{
        buf[i++] = digits[x % 10];
    }while((x /= 10) != 0);

    while(--i >= 0){ putc(buf[i]);}
}