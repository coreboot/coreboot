.section ".rom.data"

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
	SMBUS_WAIT_UNTIL_READY()
	jc	smbus_read_byte_failed

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
	SMBUS_WAIT_UNTIL_DONE()
	jc	smbus_read_byte_failed

	/* read the results and see if we succeded */
	movl	$(SMBUS_IO_BASE + SMBHSTSTAT), %edx
	inb	%dx, %al
	andb	$~(1 << 6), %al		/* Ignore the In Use Status... */
	cmpb	$0x02, %al
	jne	smbus_read_byte_failed

	movl $(SMBUS_IO_BASE + SMBHSTDAT0), %edx
	testl	%edx, %edx	/* clear zf */
	inb	%dx, %al
smbus_read_byte_done:
	RETSP
smbus_read_byte_failed:
	SMBUS_KILL_COMMAND()
	xorl	%eax, %eax		/* set zf */
	jmp smbus_read_byte_done

.previous
