jmp smbus_code_end

#define PM_DEV_FN (AMD766_DEV + 0x300)


#define PM_BASE 0xDD00
#define SMBUS_IO_BASE (PM_BASE + 0xE0)
#define SMB_GSTAT      (SMBUS_IO_BASE + 0x0)
#define SMB_GCTL       (SMBUS_IO_BASE + 0x2)
#define SMB_HOST_ADDR  (SMBUS_IO_BASE + 0x4)
#define SMB_HOST_DAT   (SMBUS_IO_BASE + 0x6)
#define SMB_HOST_CMD   (SMBUS_IO_BASE + 0x8)
#define SMB_BLKDAT     (SMBUS_IO_BASE + 0x9)
#define SMB_SLAVE_DAT  (SMBUS_IO_BASE + 0xa)
#define SMB_SLAVE_DEV  (SMBUS_IO_BASE + 0xc)
#define SMB_SLAVE_ADDR (SMBUS_IO_BASE + 0xe)
#define SMB_SNOOP_ADDR (SMBUS_IO_BASE + 0xf)

#define SMB_GSTAT_ABORT		(1 << 0)
#define SMB_GSTAT_COLLISION	(1 << 1)
#define SMB_GSTAT_PROTO_ERROR	(1 << 2)
#define SMB_GSTAT_HOST_BUSY	(1 << 3)
#define SMB_GSTAT_COMPLETE	(1 << 4)
#define SMB_GSTAT_TIMEOUT	(1 << 5)
#define SMB_GSTAT_SNOOP_MATCH	(1 << 8)
#define SMB_GSTAT_SLAVE_MATCH	(1 << 9)
#define SMB_GSTAT_ALERT		(1 << 10)
#define SMB_GSTAT_BUSY		(1 << 11)

/* The low bit of the address specifies read or write */
#define SMB_GCTL_QUICK_COMMAND    0
#define SMB_GCTL_SEND_RECV_BYTE   1
#define SMB_GCTL_WRITE_READ_BYTE  2
#define SMB_GCTL_WRITE_READ_WORD  3
#define SMB_GCTL_PROCESS_CALL     4
#define SMB_GCTL_WRITE_READ_BLOCK 5

#define SMB_GCTL_HOST_START      (1 << 3)
#define SMB_GCTL_HOST_INTERRUPT  (1 << 4)
#define SMB_GCTL_ABORT           (1 << 5)
#define SMB_GCTL_SNOOP           (1 << 8)
#define SMB_GCTL_SLAVE_INTERRUPT (1 << 9)
#define SMB_GCTL_ALERT_INTERRUPT (1 << 10)

enable_string:	.string "Enabling smbus\r\n"
setup_string:	.string "Setting up smbus\r\n"

enable_smbus:
	CONSOLE_INFO_TX_STRING($enable_string)
	/* Enable PM IO C3A41 */
	movl	$(0x80000000 | PM_DEV_FN | 0x40), %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfd, %dx
	inb	%dx, %al
	orb	$0x80, %al
	outb	%al, %dx

	/* Set the PM registers to 0xDD00 */
	movl	$(0x80000000 | PM_DEV_FN | 0x58), %eax
	mov	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfc, %dx
	movl	$(PM_BASE | 0x01), %eax
	outl	%eax, %dx

	RET_LABEL(enable_smbus)

	/*
	 * Routine:	setup_smbus
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	eax, edx
	 * Effects:	The smbus is enabled
	 */
setup_smbus:
	CONSOLE_INFO_TX_STRING($setup_string)
	movl $(SMB_GSTAT_ABORT | SMB_GSTAT_COLLISION | \
		SMB_GSTAT_PROTO_ERROR | SMB_GSTAT_COMPLETE | \
		SMB_GSTAT_TIMEOUT | SMB_GSTAT_SNOOP_MATCH | \
		SMB_GSTAT_SLAVE_MATCH | SMB_GSTAT_ALERT),%eax
	movw $SMB_GSTAT, %dx
	outb %al, %dx
	RET_LABEL(setup_smbus)

	/*
	 * Routine:	smbus_wait_until_ready
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	eax, edx
	 * Effects:	Upon return the smbus is ready to accept commands
	 */
smbus_wait_until_ready:	
	movl $SMB_GSTAT, %edx
1:	inb %dx, %al
	testb $SMB_GSTAT_HOST_BUSY, %al
	jnz 1b
	RET_LABEL(smbus_wait_until_ready)


	/*
	 * Routine:	smbus_wait_until_done
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	eax, edx
	 * Effects:	Upon return the smbus has completed it's most recent transation
	 */

/*
 *smbus_wait_until_done:	
 *	movl $SMB_GSTAT, %edx
 *1:	inb %dx, %al
 *	testb $(SMB_GSTAT_HOST_BUSY), %al
 *	jnz 1b
 *2:	testb $(~(SMB_GSTAT_HOST_BUSY)), %al
 *	jnz 3f
 *	inb %dx, %al
 *	testb $(~(SMB_GSTAT_HOST_BUSY)), %al
 *	jz 2b
 *3:	RET_LABEL(smbus_wait_until_done)
 *
 */

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

smbus_read_byte:
	/* poll until the smbus is ready for commands */
	CALL_LABEL(smbus_wait_until_ready)

	/* clear any lingering errors, so that the transaction will run */
	movw $SMB_GSTAT, %dx
	inw %dx, %ax
	outw %ax, %dx

	/* set the device I'm talking to, and set the low bit for a read */
	movw $SMB_HOST_ADDR, %dx	
	xorl %eax, %eax
	movb %bl /* device */, %al
	shlb $1, %al
	orb  $1, %al
	outw %ax, %dx

	/* set the command address... */
	movw $SMB_HOST_CMD, %dx
	movb %bh /* address */, %al
	outb %al, %dx

	/* clear the data byte */
	movw $SMB_HOST_DAT, %dx
	xorl %eax, %eax
	outw %ax, %dx

	/* start a byte read, with interrupts disabled */
	movw $SMB_GCTL, %dx
	movl $(SMB_GCTL_HOST_START | SMB_GCTL_WRITE_READ_BYTE), %eax
	outw %ax, %dx

	/* poll for transaction completion */
#	CALL_LABEL(smbus_wait_until_done)
#	code copied since CALL_LABEL can only be used once!
	movl $SMB_GSTAT, %edx
1:	inb %dx, %al
	testb $SMB_GSTAT_HOST_BUSY, %al
	jnz 1b
#endcopy of smbus_wait_until_ready

	/* read the results and see if we succeded */
	movl $SMB_GSTAT, %edx
	inb %dx, %al
	testb $(SMB_GSTAT_COMPLETE), %al
	jz 1f
	movw $SMB_HOST_DAT, %dx
	inb %dx, %al
1:
	RETSP



smbus_code_end:
CALL_LABEL(enable_smbus)
CALL_LABEL(setup_smbus)
