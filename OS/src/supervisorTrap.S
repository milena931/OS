.extern _ZN5Riscv16ecallTrapHandlerEv
.extern _ZN5Riscv16timerTrapHandlerEv
.extern _ZN5Riscv19externalTrapHandlerEv
.align 4
.global _ZN5Riscv14supervisorTrapEv
.type  _ZN5Riscv14supervisorTrapEv, @function
_ZN5Riscv14supervisorTrapEv:
j Ecall
j Timer
.skip 28
j External
Ecall:
    addi sp, sp, -256    //stavljanje svih registara na stek na pocetku prekidne rutine
    .irp index, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    sd x\index, \index * 8(sp)
    .endr

    //poziv funkije iz klase Riscv za obradu ecall i exception prekida
    call _ZN5Riscv16ecallTrapHandlerEv

    .irp index, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256    //sklanjanje svih registara sa steka na krajuz prekidne rutine

    sret


Timer:
    addi sp, sp, -256    //stavljanje svih registara na stek na pocetku prekidne rutine
    .irp index, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    sd x\index, \index * 8(sp)
    .endr

    //poziv funkije iz klase Riscv za obradu  prekida od tajmera
    call _ZN5Riscv16timerTrapHandlerEv

    .irp index, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256    //sklanjanje svih registara sa steka na krajuz prekidne rutine

    sret


External:
    addi sp, sp, -256    //stavljanje svih registara na stek na pocetku prekidne rutine
    .irp index, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    sd x\index, \index * 8(sp)
    .endr

    //poziv funkije iz klase Riscv za obradu spoljasnjeg prekida - vrv konzola
    call _ZN5Riscv19externalTrapHandlerEv

    .irp index, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256    //sklanjanje svih registara sa steka na krajuz prekidne rutine

    sret