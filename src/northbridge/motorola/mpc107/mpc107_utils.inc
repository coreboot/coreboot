/* $Id$ */
/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

    .globl __pci_config_read_32
__pci_config_read_32:
    lis     r4, 0xfec0
    stwbrx   r3, r0, r4
    sync
    lis     r4, 0xfee0
    lwbrx   r3, 0, r4
    blr

    .globl __pci_config_read_16
__pci_config_read_16:
    lis     r4, 0xfec0
    andi.    r5, r3, 2
    stwbrx  r3, r0, r4
    sync
    oris     r4, r5, 0xfee0
    lhbrx    r3, r0, r4
    blr

    .globl __pci_config_read_8
__pci_config_read_8:
    lis     r4, 0xfec0
    andi.    r5, r3, 3
    stwbrx  r3, r0, r4
    sync
    oris     r4, r5, 0xfee0
    lbz      r3, 0(r4)
    blr

    .globl __pci_config_write_32
__pci_config_write_32:
    lis     r5, 0xfec0
    stwbrx   r3, r0, r5
    sync
    lis      r5, 0xfee0
    stwbrx   r4, r0, r5
    sync
    blr

    .globl __pci_config_write_16
__pci_config_write_16:
    lis     r5, 0xfec0
    andi.    r6, r3, 2
    stwbrx  r3, r0, 5
    sync
    oris     r5, r6, 0xfee0
    sthbrx    r4, r0, r5
    sync
    blr

    .globl __pci_config_write_8
__pci_config_write_8:
    lis      r5, 0xfec0
    andi.    r6, r3, 3
    stwbrx   r3, r0, r5
    sync
    oris      r5, r6, 0xfee0
    stb       r4, 0(r5)
    sync
    blr

    .globl  in_8
in_8:
    oris    r3, r3, 0xfe00
    lbz     r3,0(r3)
    blr

    .globl  in_16
in_16:
    oris    r3, r3, 0xfe00
    lhbrx   r3, 0, r3
    blr

    .globl in_16_ne
in_16_ne:
    oris    r3, r3, 0xfe00
    lhzx    r3, 0, r3
    blr

    .globl  in_32
in_32:
    oris    r3, r3, 0xfe00
    lwbrx   r3, 0, r3
    blr

    .globl  out_8
out_8:
    oris    r3, r3, 0xfe00
    stb     r4, 0(r3)
    eieio
    blr

    .globl  out_16
out_16:
    oris    r3, r3, 0xfe00
    sthbrx  r4, 0, r3
    eieio
    blr

    .globl  out_16_ne
out_16_ne:
    oris    r3, r3, 0xfe00
    sth     r4, 0(r3)
    eieio
    blr

    .globl  out_32
out_32:
    oris    r3, r3, 0xfe00
    stwbrx  r4, 0, r3
    eieio
    blr

    .globl  read_8
read_8:
    lbz     r3,0(r3)
    blr

    .globl  read_16
read_16:
    lhbrx   r3, 0, r3
    blr

    .globl  read_32
read_32:
    lwbrx   r3, 0, r3
    blr

    .globl  read_32_ne
read_32_ne:
    lwz     r3, 0(r3)
    blr

    .globl  write_8
write_8:
    stb     r4, 0(r3)
    eieio
    blr

    .globl  write_16
write_16:
    sthbrx  r4, 0, r3
    eieio
    blr

    .globl  write_32
write_32:
    stwbrx  r4, 0, r3
    eieio
    blr

    .globl  write_32_ne
write_32_ne:
    stw     r4, 0(r3)
    eieio
    blr
