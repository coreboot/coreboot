/*
 * setup_serial.inc:	 setup serial port for m1535
 *
 *
 * Copyright 2001 Acer Labs Inc.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Reference:
 *	1. M1535D document
 * $Id$
 */
/*
* Super I/O Logical Device 4 (UART1)
     Reg 0x30 [0] = 1 -- UART1 Enable
     Reg 0x60 [7:0] = 0x03 -- The high address UART1's I/O base address
     Reg 0x61 [7:0] = 0xF8 -- The low address UART1's I/O base address
     Reg 0x70 [7:0] = 0x04 -- IRQ[4]
     Reg 0xF0 [7:0] = 0x00
     Reg 0xf1 [7:0] = 0x00
     Reg 0xf2 [7:0] = 0x00

     assume I/O port address is 0x3F0
*/

//  1 ;enter configuration mode
      movw $0x3f0,%dx
      movb $0x51,%al
      outb  %al,%dx
      movw $0x23,%ax
      outb %al,%dx

//  2 ;program logical device 4
     movw $0x3f0,%dx
     movb $0x07,%al
     outb %al,%dx   //point to device select register
     movw $0x3f1,%dx
     movb $0x04,%al
     outb %al,%dx    //point to device 4

     movw $0x3f0,%dx
     movb $0x30,%al
     outb %al,%dx
     movw $0x3f1,%dx
     movb $0x01,%al
     outb %al,%dx    //enable UART1 controller

     movw $0x3f0,%dx
     movb $0x60,%al
     outb %al,%dx
     movw $0x3f1,%dx
     movb $0x03,%al
     outb %al,%dx
     movw $0x3f0,%dx
     movb $0x61,%al
     outb %al,%dx
     movw $0x3f1,%dx
     movb $0xf8,%al
     outb %al,%dx    //select UART1's I/O base address to $0x3F$0x8

     movw $0x3f0,%dx
     movb $0x70,%al
     outb %al,%dx
     movw $0x3f1,%dx
     movb $0x04,%al
     outb %al,%dx    //select IRQ 4 used by UART1

     movw $0x3f0,%dx
     movb $0xf0,%al
     outb %al,%dx
     movw $0x3f1,%dx
     movb $0x00,%al
     outb %al,%dx
     movw $0x3f0,%dx
     movb $0xf1,%al
     outb %al,%dx
     movw $0x3f1,%dx
     movb $0x00,%al
     outb %al,%dx

     movw $0x3f0,%dx
     movb $0xf2,%al
     outb %al,%dx
     movw $0x3f1,%dx
     movb $0x00,%al
     outb %al,%dx

// 3. exit configuration mode
    movw $0x3f0,%dx
    movb $0xbb,%al
    outb %al,%dx




