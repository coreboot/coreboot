/* $Id$ */
/* Copyright 1999-2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

/*    .text*/
    .globl  _init_float_registers
    
_init_float_registers:
    lfd     0, 0(3)
    lfd     1, 0(3)
    lfd     2, 0(3)
    lfd     3, 0(3)
    lfd     4, 0(3)
    lfd     5, 0(3)
    lfd     6, 0(3)
    lfd     7, 0(3)
    lfd     8, 0(3)
    lfd     9, 0(3)
    lfd     10, 0(3)
    lfd     11, 0(3)
    lfd     12, 0(3)
    lfd     13, 0(3)
    lfd     14, 0(3)
    lfd     15, 0(3)
    lfd     16, 0(3)
    lfd     17, 0(3)
    lfd     18, 0(3)
    lfd     19, 0(3)
    lfd     20, 0(3)
    lfd     21, 0(3)
    lfd     22, 0(3)
    lfd     23, 0(3)
    lfd     24, 0(3)
    lfd     25, 0(3)
    lfd     26, 0(3)
    lfd     27, 0(3)
    lfd     28, 0(3)
    lfd     29, 0(3)
    lfd     30, 0(3)
    lfd     31, 0(3)
    blr
    .end
    
