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
#define i8259delay jmp 1f ; 1:

	movb	$0x11,%al		/*! initialization sequence*/
	outb	%al,$0x20		/*! send it to 8259A-1*/
	i8259delay
	outb	%al,$0xA0		/*! and to 8259A-2*/
	i8259delay
	movb	$0x20,%al		/*! start of hardware int's (0x20)*/
	outb	%al,$0x21
	i8259delay
	movb	$0x28,%al		/*! start of hardware int's 2 (0x28)*/
	outb	%al,$0xA1
	i8259delay
	movb	$0x04,%al		/*! 8259-1 is master*/
	outb	%al,$0x21
	i8259delay
	movb	$0x02,%al		/*! 8259-2 is slave*/
	outb	%al,$0xA1
	i8259delay
	movb	$0x01,%al		/*! 8086 mode for both*/
	outb	%al,$0x21
	i8259delay
	outb	%al,$0xA1
	i8259delay
	movb	$0xFF,%al		/*! mask off all interrupts for now*/
	outb	%al,$0xA1
	i8259delay
	movb	$0xFB,%al		/*! mask all irq's but irq2 which*/
	outb	%al,$0x21		/*! is cascaded*/

/*
 * I like the way Linus says it: 
! Well, that certainly wasn't fun :-(. Hopefully it works, and we don't
! need no steenking BIOS anyway (except for the initial loading :-).
*/
