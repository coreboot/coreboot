/* 
 * Copyright (C) 2002 Eric Biederman 
 */

#include "82801.h"

/* jump around these subrs */
jmp smbus_end

	/*
	 * Routine:	smbus_setup
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	eax, edx, ecx
	 * Effects:	The smbus is enabled
	 */

smbus_setup:	
	/* set smbus iobase */
	movl	$((SMBUS_DEVFN << 8) + 0x20), %eax
	movl	$(SMBUS_IO_BASE | 1), %ecx
	PCI_WRITE_CONFIG_DWORD

	/* Set smbus enable */
	movl	$((SMBUS_DEVFN << 8) + 0x40), %eax
	movl	$1, %edx
	PCI_WRITE_CONFIG_BYTE

	/* Set smbus iospace enable */
	movl	$((SMBUS_DEVFN << 8) + 0x4), %eax
	movl	$1, %ecx
	PCI_WRITE_CONFIG_WORD

	/* Disable interrupt generation */
	movl	$(SMBUS_IO_BASE + SMBHSTCTL), %edx
	xorl	%eax, %eax
	outb	%al, %dx

	RET_LABEL(smbus_setup)


	/*
	 * Routine:	smbus_wait_until_ready
	 * Arguments:	none
	 * Results:	Carry set on timeout
	 * Trashed:	eax, edx
	 * Effects:	Upon return the smbus is ready to accept commands
	 */
#define SMBUS_WAIT_UNTIL_READY() \
	movl	$(SMBUS_TIMEOUT << 8), %eax			; \
	movl	$(SMBUS_IO_BASE + SMBHSTSTAT), %edx	; \
1:	outb	%al, $0x80				; \
	inb	%dx, %al				; \
	subl	$0x00000100, %eax			; \
	testl	$0xffffff00, %eax			; \
	stc						; \
	jz	2f					; \
	testb	$(1<<0), %al				; \
	jnz	1b					; \
	clc						; \
2:

	/*
	 * Routine:	smbus_wait_until_done
	 * Arguments:	none
	 * Results:	carry set on timeout
	 * Trashed:	eax, edx
	 * Effects:	Upon return the smbus has completed it's most recent transation
	 */
#define SMBUS_WAIT_UNTIL_DONE() \
	movl	$(SMBUS_TIMEOUT << 8), %eax			; \
	movl	$(SMBUS_IO_BASE + SMBHSTSTAT), %edx	; \
1:	outb	%al, $0x80				; \
	inb	%dx, %al				; \
	subl	$0x00000100, %eax			; \
	testl	$0xffffff00, %eax			; \
	stc						; \
	jz	2f					; \
	testb	$1, %al					; \
	jnz	1b					; \
	testb	$~((1<<6)|(1<<0)), %al			; \
	jz	1b					; \
	clc						; \
2:

	/*
	 * Routine:	smbus_wait_until_next
	 * Arguments:	none
	 * Results:	al smbus status
	 *              edx status register addr
	 * Trashed:	eax, edx
	 * Effects:	Upon return the smbus is ready for the next byte
	 */
#define SMBUS_WAIT_UNTIL_NEXT() \
	movl	$(SMBUS_TIMEOUT << 8), %eax			; \
	movl	$(SMBUS_IO_BASE + SMBHSTSTAT), %edx	; \
1:	outb	%al, $0x80				; \
	inb	%dx, %al				; \
	subl	$0x00000100, %eax			; \
	testl	$0xffffff00, %eax			; \
	stc						; \
	jz	2f					; \
	testb	$~((1<<6)|(1<<1)|(1<<0)), %al		; \
	jz	1b					; \
	clc						; \
2:

	/*
	 * Routine:	smbus_kill_command
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	eax, edx, flags
	 * Effects:	Upon return the smbus is ready to accept commands
	 */

#define SMBUS_KILL_COMMAND() \
	movl	$(SMBUS_IO_BASE + SMBHSTCTL), %edx	; \
	inb	%dx, %al				; \
	orb	$(1<<1), %al				; \
	outb	%al, %dx				; \
	SMBUS_WAIT_UNTIL_DONE()				; \
	movl	$(SMBUS_IO_BASE + SMBHSTCTL), %edx	; \
	inb	%dx, %al				; \
	andb	$~(1<<1), %al				; \
	outb	%al, %dx
	

smbus_end:
CALL_LABEL(smbus_setup)
