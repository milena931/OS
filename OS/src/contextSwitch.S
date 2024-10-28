.global _ZN3TCB13contextSwitchEPNS_13ThreadContextES1_
.type _ZN3TCB13contextSwitchEPNS_13ThreadContextES1_, @function
_ZN3TCB13contextSwitchEPNS_13ThreadContextES1_:
    sd ra, 0 * 8(a0)
    sd sp, 1 * 8(a0)  #a0 je old

    ld ra, 0 * 8(a1)
    ld sp, 1 * 8(a1)  #a1 je novi (zbog redosleda argumenata)

    ret
