/*
This software and ancillary information (herein called SOFTWARE )
called LinuxBIOS          is made available under the terms described
here.  The SOFTWARE has been approved for release with associated
LA-CC Number 00-34   .  Unless otherwise indicated, this SOFTWARE has
been authored by an employee or employees of the University of
California, operator of the Los Alamos National Laboratory under
Contract No. W-7405-ENG-36 with the U.S. Department of Energy.  The
U.S. Government has rights to use, reproduce, and distribute this
SOFTWARE.  The public may copy, distribute, prepare derivative works
and publicly display this SOFTWARE without charge, provided that this
Notice and any statement of authorship are reproduced on all copies.
Neither the Government nor the University makes any warranty, express
or implied, or assumes any liability or responsibility for the use of
this SOFTWARE.  If SOFTWARE is modified to produce derivative works,
such modified SOFTWARE should be clearly marked, so as not to confuse
it with the version available from LANL.
 */
/* Parts of this code Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 * rminnich@lanl.gov
 * Modifications by Ronnie Liu of Acer Labs Inc.
 */
#include <asm.h>
#include <cpu/p5/macros.h>

/* this is an expanded version of IPL. We do this here because
 * we can't fit all the init into the IPL. 
 */
/* NOTE: for this to work, we're going to have to turn off refresh. 
 * but, when we do that, we start to lose data afer a time. 
 * as it happens, you CAN turn on and off refresh and maybe live. 
 * but, that's a horrible thing to do ... we'll see.
 */

/* wow, the usual way to to this hurts. So we do it our way:
 * 32-bit test not needed. 
 */
/* the MCR is 32-bits. You set it, it programs SDRAM. 
 * first check: get Column address size (CAS)
 * Start out assuming that it is 8 bits, then grow. 
 * nicely, if we put the 32-bit MCR value in %ecs, we can 
 * ror it 16 bits, and as we grow the CAS, we just inc cs, and that will
 * set the right value. 
 */
#define INIT_MCR $0xf663f83c
/* GNU AS misassembles this. I can't believe what a piece of 
 * trash gas has turned out to be ...
#define LONGDELAY movw	$0x1000, ax; 1: dec %ax; jnz 1b
 */
#define LONGDELAY  1: dec %ax; jnz 1b
#define WDELAY outb %al, $0x80; jmp 1f; 1: outb %al, $0x80
#define WRITE_MCR0 movl %ecx, %eax ;  outl %eax, %dx ; WDELAY
#include <cpu/p5/start32.inc>

	// Initialization -- set up basic parameters.
	movl	$0x8000006c, %eax
	movw	$0xcf8,%dx
	outl	%eax,%dx
	xorl	%edi, %edi

	// if we are using DOC MIL, the initial setup is not needed. 
#ifdef USE_DOC_MIL
	// pull in the value of the current register. 
	inl	%dx, %ecx
	jmp	oddslot
#endif
loop: 
	movb	$0xfc, %dl
	movl	$0xf663f83c, %ecx
	WRITE_MCR0
	rorl	$16, %ecx
	/* clear out the CAS, since we assume start at zero
	 * and increment 
	 */
	andb	$0xf0, %cl
	/* test 8 bit CAS */
Secondtry: 
	movl	$0x800, %esi
	movb	$0, (%edi)
	movb	$1, (%esi) // esi is at 0x800
	cmpb	$0,(%edi)
	jnz	sizeram
	inc	%cl
	roll	%esi
	/* change the value we store each time. It makes debugging easier */
	movb	$2, (%esi) // esi is at 0x1000
	cmpb	$0, (%edi)
	jnz	sizeram
	inc	%cl
	roll	%esi
	movb	$3, (%esi) // esi is at 0x2000
	cmpb	$0, (%edi)
	jnz	sizeram
	inc	%cl
	roll	%esi
	movb	$4, (%esi) // esi is at 0x4000
	cmpb	$0, (%edi)
	jnz	sizeram
	
sizeram:		
	rorl	$16, %ecx
	/* clear the 'no multi page' bit. */
	/* NOTE: 3/22/01 -- I think this is wrong! -- rgm
	 * I think we should OR IN 0x1000, which we already did!
	andw	$0xefff, %cx
	 */
	WRITE_MCR0 
	/*
	mov	$0x6c, %al
	CALLSP(pci_write_dword)
	*/
/* size is now in cx[19:16] */
/* now size the dram */
/* you had best have at least 4M; that's as small as we go */
/* rorr 20 the ecx value, to get row size into lsb */
	movb	$0, (%edi)
	ror	$20, %ecx
	/* clear the size out to 4 MB */
	andb	$0xf8, %cl
	/* 4 MB */
	movl	$0x400000, %esi
1:
	/* move a 4 to the next power-of-two address. 
	 * if there is no memory there, it will wrap to zero 
	 */
 	movb	$4, %es:(%esi)
	cmpb	$0, (%edi)
	/* if there is not a 0 at 0, the last write wrapped. Hop out */
	jne	1f
	inc	%cl
	rol	$1, %esi
	jmp	1b
/* size is now in esi */
/* %ecx has setting for register */
1:
	rol	$20, %ecx
/* set 4 banks. */
	orb	$1, %cl
/* COMMENT 3-22-01 RGM -- I THINK WE WANT THIS HERE TO TURN
 * MULTIBANK ON
 */
	andw	$0xefff, %cx
	WRITE_MCR0
	/*
	mov	$0x6c, %al
	CALLSP(pci_write_dword)
	*/
/* bank detect */
/* you don't need to even care how large CAS is. 
 * Just set 4-bank mode, and set non-zero numbers into a few places. 
 * if the byte at 0 changes, you have two banks. Trivial.
	LONGDELAY
 */
	movl	$0x1000, %esi
	movb	$0, (%edi)
	movb	$5, (%esi)
	roll	$1, %esi
	movb	$6, (%esi)
	roll	$1, %esi
	movb	$7, (%esi)
	cmpb	$0, (%edi)
	// If @0 is unchanged, then we have four banks.
	jz	1f /* 4 banks */
	// If @0 got overwritten, then we only have two banks.
	/* clear 4 banks */
	andb	$0xfe, %cl
1:
	WRITE_MCR0
oddslot:
	// OK, memory is on. Size is in @0. 
	// Turn on the odd slot and see if it's there. 
	orl	$0x1800000, %ecx
	WRITE_MCR0
	// ok, the odd bank is on. See if there's something there. 
	// put a 0 in, if there's something there, it will read back. 
	movl	0, %esi
	movl	$0, (%esi)
	cmp	$0, (%esi)
	jz	odd_slot_ok
	andl	(~$0x1800000), %ecx
	WRITE_MCR0
odd_slot_ok:
	// pick the next memory register to configure. 
	// If done, drop out. 
	
// turn on refresh	
/* Next line ASSUMES that eax contains 8000000xx
	movb	$0x7c, %al
	movb	$0xf8, %dl
	outl	%eax, %dx
	movl	$0x2428c411, %eax
	movb	$0xfc, %dl
	outl	%eax, %dx
	/*
	mov	$0x6c, %al
	CALLSP(pci_write_dword)
	*/
	
