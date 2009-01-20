#include <arch/io.h>
#include <pc80/i8259.h>
/* code taken from: 
!
!	setup.S		Copyright (C) 1991, 1992 Linus Torvalds
!
! setup.s is responsible for getting the system data from the BIOS,
! and putting them into the appropriate places in system memory.
! both setup.s and system has been loaded by the bootblock.
 */
/* we're getting screwed again and again by this problem of the 8259. 
 * so we're going to leave this lying around for inclusion into 
 * crt0.S on an as-needed basis. 
! well, that went ok, I hope. Now we have to reprogram the interrupts :-(
! we put them right after the intel-reserved hardware interrupts, at
! int 0x20-0x2F. There they won't mess up anything. Sadly IBM really
! messed this up with the original PC, and they haven't been able to
! rectify it afterwards. Thus the bios puts interrupts at 0x08-0x0f,
! which is used for the internal hardware interrupts as well. We just
! have to reprogram the 8259's, and it isn't fun.
 */

void setup_i8259(void)
{
	outb(0x11, 0x20);		/*! initialization sequence to 8259A-1*/
	outb(0x11, 0xA0);		/*! and to 8259A-2*/
	outb(0x20, 0x21);		/*! start of hardware int's (0x20)*/
	outb(0x28, 0xA1);		/*! start of hardware int's 2 (0x28)*/
	outb(0x04, 0x21);		/*! 8259-1 is master*/
	outb(0x02, 0xA1);		/*! 8259-2 is slave*/
	outb(0x01, 0x21);		/*! 8086 mode for both*/
	outb(0x01, 0xA1);		
	outb(0xFF, 0xA1);		/*! mask off all interrupts for now*/
	outb(0xFB, 0x21);		/*! mask all irq's but irq2 which is cascaded*/
}

/*
 * I like the way Linus says it: 
! Well, that certainly wasn't fun :-(. Hopefully it works, and we don't
! need no steenking BIOS anyway (except for the initial loading :-).
*/

