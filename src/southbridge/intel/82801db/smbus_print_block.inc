.section ".rom.data"

/* smbus print a block of data
	input: 	bl device, bh command, cl count, 
	output: cf set on error
	trashed: eax,edx
*/
smbus_print_block:
	/* poll until the smbus is ready for commands */
	SMBUS_WAIT_UNTIL_READY()
	jc smbus_block_print_error

	/* setup transaction */
	/* disable interrupts */
	movl	$(SMBUS_IO_BASE + SMBHSTCTL), %edx
	inb	%dx, %al
	andb	$0xFE, %al
	outb	%al, %dx

	/* set the device I'm talking to and specify a read */
	movl	$(SMBUS_IO_BASE + SMBXMITADD), %edx
	movb	%bl /* device */, %al
	shlb	$1, %al
	orb	$1, %al
	outb	%al, %dx

	/* set the command address... */
	movl	$(SMBUS_IO_BASE + SMBHSTCMD), %edx
	movb	%bh /* address */, %al
	outb	%al, %dx

	/* setup for a block data read/write */
	movl	$(SMBUS_IO_BASE + SMBHSTCTL), %edx
	inb	%dx, %al
	andb	$0xE3, %al
	orb	$(0x5 << 2), %al
	outb	%al, %dx

	/* set the block count */
	movl	$(SMBUS_IO_BASE + SMBHSTDAT0), %edx
	movb	%cl /* count */, %al
	outb	%al, %dx

	/* clear any lingering errors, so the transaction will run */
	movl	$(SMBUS_IO_BASE + SMBHSTSTAT), %edx
	inb	%dx, %al
	outb	%al, %dx

	/* start a block read, with interrupts disabled */
	movl	$(SMBUS_IO_BASE + SMBHSTCTL), %edx
	inb	%dx, %al
	orb	$(1 << 6), %al
	outb	%al, %dx

smbus_block_print_next:
	/* wait until the controller has more data ready */
	SMBUS_WAIT_UNTIL_NEXT()
	jc	smbus_block_print_error

	/* Test the results to see if we succeeded */
	testb	$(1<<7), %al
	jz	smbus_block_print_error

	/* read the next byte */
        movl    $(SMBUS_IO_BASE + SMBBLKDAT), %edx
	inb	%dx, %al
	CONSOLE_DEBUG_INLINE_TX_HEX8(%al)
	CONSOLE_DEBUG_INLINE_TX_CHAR($',')

	/* finish this byte read */
	movl    $(SMBUS_IO_BASE + SMBHSTSTAT), %edx
	movb	$(1<<7), %al
        outb    %al, %dx
	decb	%cl
	jnz	smbus_block_print_next

	/* No error I am done */
	clc
smbus_block_print_end:
	RETSP
smbus_block_print_error:
	SMBUS_KILL_COMMAND()
	stc
	jmp smbus_block_print_end

.previous
