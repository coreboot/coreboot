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

	/* Diable interrupt generation */
	movl	$(SMBUS_IO_BASE + SMBHSTCTL), %edx
	xorl	%eax, %eax
	outb	%al, %dx

	RET_LABEL(smbus_setup)


	/*
	 * Routine:	smbus_wait_until_ready
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	eax, edx
	 * Effects:	Upon return the smbus is ready to accept commands
	 */
smbus_wait_until_ready:	
	movl	$(SMBUS_IO_BASE + SMBHSTSTAT), %edx
1:	inb	%dx, %al
	testb	$1, %al
	jnz	1b
	RET_LABEL(smbus_wait_until_ready)

	/*
	 * Routine:	smbus_wait_until_done
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	eax, edx
	 * Effects:	Upon return the smbus has completed it's most recent transation
	 */
smbus_wait_until_done:	
	movl	$(SMBUS_IO_BASE + SMBHSTSTAT), %edx
1:	inb	%dx, %al
	testb	$1, %al
	jnz	1b
	jmp	3f
2:	
	inb	%dx, %al
3:	testb	$~((1<<6)|(1<<0)), %al
	jz	2b
	RET_LABEL(smbus_wait_until_done)


	/*
	 * Routine:	smbus_read_byte
	 * Arguments:	%esp return address
	 *              %bl device on the smbus to read from 
	 *              %bh address on the smbus to read
	 *
	 * Results:	zf clear
	 *              byte read %eax
	 * 	On Error:	
	 *              zf set
	 *              %eax trashed
	 *
	 * Trashed:	%edx, %eax
	 * Effects:	reads a byte off of the smbus
	 */

#define SMBUS_READ_BYTE(device, address) \
	movl $( (device) | ((address) << 8)), %ebx ; \
	CALLSP(smbus_read_byte)

smbus_read_byte:
	/* poll until the smbus is ready for commands */
	CALL_LABEL(smbus_wait_until_ready)

	/* setup transaction */
	/* disable interrupts */
	movl	$(SMBUS_IO_BASE + SMBHSTCTL), %edx
	inb	%dx, %al
	andb	$0xFE, %al
	outb	%al, %dx

	/* set the device I'm talking to */
	movl	$(SMBUS_IO_BASE + SMBXMITADD), %edx
	movb	%bl /* device */, %al
	shlb	$1, %al
	orb	$1, %al
	outb	%al, %dx

	/* set the command address... */
	movl	$(SMBUS_IO_BASE + SMBHSTCMD), %edx
	movb	%bh /* address */, %al
	outb	%al, %dx

	/* setup for a byte data read */
	movl	$(SMBUS_IO_BASE + SMBHSTCTL), %edx
	inb	%dx, %al
	andb	$0xE3, %al
	orb	$(0x2 << 2), %al
	outb	%al, %dx

	/* clear any lingering errors, so the transaction will run */
	movl	$(SMBUS_IO_BASE + SMBHSTSTAT), %edx
	inb	%dx, %al
	outb	%al, %dx

	/* clear the data byte... */
	movl	$(SMBUS_IO_BASE + SMBHSTDAT0), %edx
	xorl	%eax, %eax
	outb	%al, %dx

	/* start a byte read, with interrupts disabled */
	movl	$(SMBUS_IO_BASE + SMBHSTCTL), %edx
	inb	%dx, %al
	orb	$0x40, %al
	outb	%al, %dx

	/* poll for transaction completion */
	CALL_LABEL(smbus_wait_until_done)

	/* read the results and see if we succeded */
	movl	$(SMBUS_IO_BASE + SMBHSTSTAT), %edx
	inb	%dx, %al
	andb	$~(1 << 6), %al		/* Ignore the In Use Status... */
	cmpb	$0x02, %al
	sete	%al
	testb	%al, %al
	jz	1f

	movl $(SMBUS_IO_BASE + SMBHSTDAT0), %edx
	inb %dx, %al
1:
	RETSP


smbus_end:
CALL_LABEL(smbus_setup)
