.section ".rom.data"

/* smbus write a block of data
	input: 	bl device, bh command, cl count, 
		esi pointer to data block
	output: cf set on error
*/
smbus_write_block:
	/* poll until the smbus is ready for commands */
	SMBUS_WAIT_UNTIL_READY()
	jc	smbus_block_write_end

	/* setup transaction */
	/* disable interrupts */
	movl	$(SMBUS_IO_BASE + SMBHSTCTL), %edx
	inb	%dx, %al
	andb	$0xFE, %al
	outb	%al, %dx

	/* set the device I'm talking to, and specify a write */
	movl	$(SMBUS_IO_BASE + SMBXMITADD), %edx
	movb	%bl /* device */, %al
	shlb	$1, %al
	orb	$0, %al
	outb	%al, %dx

	/* set the command address... */
	movl	$(SMBUS_IO_BASE + SMBHSTCMD), %edx
	movb	%bh /* address */, %al
	outb	%al, %dx

	/* setup for block read/write */
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

	/* load the data byte in the block data reg */
	cld
	movl	$(SMBUS_IO_BASE + SMBBLKDAT), %edx
	lodsb	(%esi), %al
	outb	%al, %dx
	decb	%cl

	/* start a block write, with interrupts disabled */
	movl	$(SMBUS_IO_BASE + SMBHSTCTL), %edx
	inb	%dx, %al
	orb	$0x40, %al
	outb	%al, %dx


smbus_block_write_next:
	/* wait until the controller is ready for more */
	SMBUS_WAIT_UNTIL_NEXT()
	jc	smbus_block_write_end

	/* Test the results to see if we succeeded */
	testb	$(1<<7), %al
	stc
	jz	smbus_block_write_end

	/* load the next byte */
        movl    $(SMBUS_IO_BASE + SMBBLKDAT), %edx
        lodsb   (%esi), %al
        outb    %al, %dx

	/* start the next byte write */
	movl    $(SMBUS_IO_BASE + SMBHSTSTAT), %edx
	movb	$(1<<7), %al
        outb    %al, %dx
	decb	%cl
	jnz	smbus_block_write_next

	/* No error I am done */
	clc
smbus_block_write_end:
	RETSP

.previous
