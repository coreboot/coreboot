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
/* Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 * rminnich@lanl.gov
 */


#ifndef ROM_INTEL_H
#define	ROM_INTEL_H

/*
 * Bootstrap code for the Intel
 *
 * $Id$
 *
 */

/* 
 *	Config registers. 
 */
/* yeah, yeah, I know these are macros, which is bad. Don't forget: 
 * we have almost no assembly, so I am not worrying just yet about this. 
 * we'll fix it someday if we care. My guess is we won't.
 */

/* well we want functions. But first we want to see it work at all. */
#undef FUNCTIONS
#ifndef FUNCTIONS

#define RET_LABEL(label)	\
	jmp label##_done

#define CALL_LABEL(label)	\
	jmp label		;\
label##_done:

#define CALLSP(func) \
	lea	0f, %esp	; \
	jmp func		; \
0:

#define RETSP \
	jmp *%esp


#define DELAY(x) mov x, %ecx ;\
        1: loop 1b ;\


	
	/*
	 * Macro:	PCI_WRITE_CONFIG_BYTE
	 * Arguments:	%eax address to write to (includes bus, device, function, &offset)
	 *              %dl byte to write
	 *
	 * Results:	none
	 *
	 * Trashed:	%eax, %edx
	 * Effects:	writes a single byte to pci config space
	 *
	 * Notes:	This routine is optimized for minimal register usage.
	 *              And the tricks it does cannot scale beyond writing a single byte.
	 *               
	 *              What it does is almost simple.
	 *              It preserves %eax (baring special bits) until it is written
	 *              out to the appropriate port.  And hides the data byte
	 *              in the high half of edx.
	 *
	 *              In %edx[3] it stores the byte to write.
	 *              In %edx[2] it stores the lower three bits of the address.
	 */


#define PCI_WRITE_CONFIG_BYTE \
	shll $8,   %edx		; \
	movb %al,  %dl		; \
	andb $0x3, %dl		; \
	shll $16,  %edx		; \
	\
	orl  $0x80000000, %eax	; \
	andl $0xfffffffc, %eax	; \
	movw $0xcf8, %dx	; \
	outl %eax,  %dx		; \
	\
	shrl $16,  %edx		; \
	movb %dh,  %al		; \
	movb $0,   %dh		; \
	addl $0xcfc, %edx	; \
	outb %al,  %dx


	/*
	 * Macro:	PCI_WRITE_CONFIG_WORD
	 * Arguments:	%eax address to write to (includes bus, device, function, &offset)
	 *              %ecx word to write
	 *
	 * Results:	none
	 *
	 * Trashed:	%eax, %edx
	 * Preserved:   %ecx
	 * Effects:	writes a single byte to pci config space
	 *
	 * Notes:	This routine is optimized for minimal register usage.
	 *               
	 *              What it does is almost simple.
	 *              It preserves %eax (baring special bits) until it is written
	 *              out to the appropriate port.  And hides the least significant
	 *              bits of the address in the high half of edx.
	 *
	 *              In %edx[2] it stores the lower three bits of the address.
	 */


#define PCI_WRITE_CONFIG_WORD \
	movb %al,  %dl		; \
	andl $0x3, %edx		; \
	shll $16,  %edx		; \
	\
	orl  $0x80000000, %eax	; \
	andl $0xfffffffc, %eax	; \
	movw $0xcf8, %dx	; \
	outl %eax,  %dx		; \
	\
	shrl $16,  %edx		; \
	movl %ecx, %eax		; \
	addl $0xcfc, %edx	; \
	outw %ax,  %dx



	/*
	 * Macro:	PCI_WRITE_CONFIG_DWORD
	 * Arguments:	%eax address to write to (includes bus, device, function, &offset)
	 *              %ecx dword to write
	 *
	 * Results:	none
	 *
	 * Trashed:	%eax, %edx
	 * Preserved:   %ecx
	 * Effects:	writes a single byte to pci config space
	 *
	 * Notes:	This routine is optimized for minimal register usage.
	 *               
	 *              What it does is almost simple.
	 *              It preserves %eax (baring special bits) until it is written
	 *              out to the appropriate port.  And hides the least significant
	 *              bits of the address in the high half of edx.
	 *
	 *              In %edx[2] it stores the lower three bits of the address.
	 */


#define PCI_WRITE_CONFIG_DWORD \
	movb %al,  %dl		; \
	andl $0x3, %edx		; \
	shll $16,  %edx		; \
	\
	orl  $0x80000000, %eax	; \
	andl $0xfffffffc, %eax	; \
	movw $0xcf8, %dx	; \
	outl %eax,  %dx		; \
	\
	shrl $16,  %edx		; \
	movl %ecx, %eax		; \
	addl $0xcfc, %edx	; \
	outl %eax,  %dx



	
	/*
	 * Macro:	PCI_READ_CONFIG_BYTE
	 * Arguments:	%eax address to read from (includes bus, device, function, &offset)
	 *
	 * Results:	%al Byte read
	 *
	 * Trashed:	%eax, %edx
	 * Effects:	reads a single byte from pci config space
	 *
	 * Notes:	This routine is optimized for minimal register usage.
	 *               
	 *              What it does is almost simple.
	 *              It preserves %eax (baring special bits) until it is written
	 *              out to the appropriate port.  And hides the least significant
	 *              bits of the address in the high half of edx.
	 *
	 *              In %edx[2] it stores the lower three bits of the address.
	 */


#define PCI_READ_CONFIG_BYTE \
	movb %al,  %dl		; \
	andl $0x3, %edx		; \
	shll $16,  %edx		; \
	\
	orl  $0x80000000, %eax	; \
	andl $0xfffffffc, %eax	; \
	movw $0xcf8, %dx	; \
	outl %eax,  %dx		; \
	\
	shrl $16,  %edx		; \
	addl $0xcfc, %edx	; \
	inb  %dx,  %al



	/*
	 * Macro:	PCI_READ_CONFIG_WORD
	 * Arguments:	%eax address to read from (includes bus, device, function, &offset)
	 *
	 * Results:	%ax word read
	 *
	 * Trashed:	%eax, %edx
	 * Effects:	reads a 2 bytes from pci config space
	 *
	 * Notes:	This routine is optimized for minimal register usage.
	 *               
	 *              What it does is almost simple.
	 *              It preserves %eax (baring special bits) until it is written
	 *              out to the appropriate port.  And hides the least significant
	 *              bits of the address in the high half of edx.
	 *
	 *              In %edx[2] it stores the lower three bits of the address.
	 */


#define PCI_READ_CONFIG_WORD \
	movb %al,  %dl		; \
	andl $0x3, %edx		; \
	shll $16,  %edx		; \
	\
	orl  $0x80000000, %eax	; \
	andl $0xfffffffc, %eax	; \
	movw $0xcf8, %dx	; \
	outl %eax,  %dx		; \
	\
	shrl $16,  %edx		; \
	addl $0xcfc, %edx	; \
	inw  %dx,  %ax



	/*
	 * Macro:	PCI_READ_CONFIG_DWORD
	 * Arguments:	%eax address to read from (includes bus, device, function, &offset)
	 *
	 * Results:	%eax
	 *
	 * Trashed:	%edx
	 * Effects:	reads 4 bytes from pci config space
	 *
	 * Notes:	This routine is optimized for minimal register usage.
	 *               
	 *              What it does is almost simple.
	 *              It preserves %eax (baring special bits) until it is written
	 *              out to the appropriate port.  And hides the least significant
	 *              bits of the address in the high half of edx.
	 *
	 *              In %edx[2] it stores the lower three bits of the address.
	 */


#define PCI_READ_CONFIG_DWORD \
	movb %al,  %dl		; \
	andl $0x3, %edx		; \
	shll $16,  %edx		; \
	\
	orl  $0x80000000, %eax	; \
	andl $0xfffffffc, %eax	; \
	movw $0xcf8, %dx	; \
	outl %eax,  %dx		; \
	\
	shrl $16,  %edx		; \
	addl $0xcfc, %edx	; \
	inl  %dx,  %eax




#define CS_READ(which)  \
                 mov  $0x80000000,%eax ; \
                 mov  which,%ax ; \
                 and  $0xfc,%al            /* clear bits 1-0 */ ; \
                 mov  $0xcf8,%dx           /* port 0xcf8 ?*/ ; \
                 outl  %eax,%dx             /* open up CS config */ ; \
                 add  $0x4,%dl             /* 0xcfc data port 0 */ ; \
                 mov  which,%al ; \
                 and  $0x3,%al             /* only bits 1-0 */ ; \
                 add  %al,%dl ; \
                 inb   %dx,%al              /* read  */ ; \


#define CS_WRITE(which, data)  \
                 mov  $0x80000000,%eax       /* 32bit word with bit 31 set */ ; \
                 mov  which,%ax             /* put the reg# in the low part */ ; \
                 and  $0xfc,%al             /* dword align the reg# */ ; \
                 mov  $0xcf8,%dx            /* enable port  */ ; \
                 outl  %eax,%dx ; \
                 add  $0x4,%dl             /* 1st data port */ ; \
                 mov  which,%ax             /* register# */ ; \
                 and  $0x3,%ax ; \
                 add  %al,%dl ; \
		 mov  data, %al ; \
                 outb  %al,%dx                /* write to reg */ 

#define REGBIS(which, bis) \
	CS_READ(which) ;\
	movb bis, %cl ;\
	orb %al, %cl ;\
	CS_WRITE(which, %cl)

#define REGBIC(which, bic) \
	CS_READ(which) ;\
	movb bic, %cl ;\
	notb %cl ;\
	andb %al, %cl ;\
	CS_WRITE(which, %cl)


/* macro to BIC and BIS a reg. calls read a reg,
 * does a BIC and then a BIS on it.
 * to clear no bits, make BIC 0.
 * to set no bits, make BIS 0
 */
#define REGBICBIS(which, bic, bis) \
	CS_READ(which) ;\
	movb bic, %cl ;\
	notb %cl ;\
	andb %cl, %al ;\
	movb bis, %cl ;\
	orb %al, %cl ;\
	CS_WRITE(which, %cl)

#else
NO FUNCTIONS YET!
#endif



/* originally this macro was from STPC BIOS */
#define	intel_chip_post_macro(value)			 \
	movb	$value, %al				; \
	outb	%al, $0x80

#define INTEL_PDATA_MAGIC 0xdeadbeef

/* SLOW_DOWN_IO is a delay we can use that is roughly cpu neutral,
 * and can be used before memory or timer chips come up.
 * Since this hits the isa bus it's roughly
 */
#define SLOW_DOWN_IO inb $0x80, %al

#endif /* ROM_INTEL_H */
