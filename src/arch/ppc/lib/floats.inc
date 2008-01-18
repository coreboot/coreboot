/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 1999-2000 AG Electronics Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

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
    
