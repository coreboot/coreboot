/* Useful macros PCIBUS, and SMBUS functions for getting DRAM going. */
/* courtesy Eric Biederman of linuxnetworx.com */

#define CS_WRITE_BYTE(addr, byte) \
	movl $addr, %eax	; \
	movl $byte, %edx	; \
	PCI_WRITE_CONFIG_BYTE

#define CS_WRITE_WORD(addr, word) \
	movl $addr, %eax	; \
	movl $word, %ecx	; \
	PCI_WRITE_CONFIG_WORD

#define CS_WRITE_LONG(addr, dword) \
	movl $addr, %eax	; \
	movl $dword, %ecx	; \
	PCI_WRITE_CONFIG_DWORD
	
#define DEVFN(device, function) (((device) << 3) + (function))
#ifndef CONFIG_ADDR
#define CONFIG_ADDR(bus,devfn,where) (((bus) << 16) | ((devfn) << 8) | (where))
#endif

/* jump around these subrs */
jmp smbus_end

/* generic SMB routines that work for many systems. The only one that might
 * not work is the enable_smbus. 
 * you have to define PM_FUNCTION for this to work. 
 */
#define SMBUS_IO_BASE 0xf00
#define SMBHSTSTAT 0
#define SMBHSTCTL  2
#define SMBHSTCMD  3
#define SMBHSTADD  4
#define SMBHSTDAT0 5
#define SMBHSTDAT1 6
#define SMBBLKDAT  7

	/*
	 * Routine:	setup_smbus
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	eax, edx
	 * Effects:	The smbus is enabled
	 */
setup_smbus:	
	xor %eax,%eax
	movl $(SMBUS_IO_BASE +SMBHSTSTAT), %edx
	outb %al, %dx
	RET_LABEL(setup_smbus)

#define SMBUS_MEM_DEVICE_0 (0xa << 3)
#define SMBUS_MEM_DEVICE_1 (SMBUS_MEM_DEVICE_0 +1)
#define SMBUS_MEM_DEVICE_2 (SMBUS_MEM_DEVICE_0 +2)
#define SMBUS_MEM_DEVICE_3 (SMBUS_MEM_DEVICE_0 +3)
#define SMBUS_SPD 0

#if SMBUS_SPD
	/*
	 * Routine:	smbus_wait_until_ready
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	eax, edx
	 * Effects:	Upon return the smbus is ready to accept commands
	 */
smbus_wait_until_ready:	
	movl $(SMBUS_IO_BASE + SMBHSTSTAT), %edx
1:	inb %dx, %al
	testb $1, %al
	jnz 1b
	RET_LABEL(smbus_wait_until_ready)

	/*
	 * Routine:	smbus_wait_until_done
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	eax, edx
	 * Effects:	Upon return the smbus has completed it's most recent transation
	 */
smbus_wait_until_done:	
	movl $(SMBUS_IO_BASE + SMBHSTSTAT), %edx
1:	inb %dx, %al
	testb $1, %al
	jnz 1b
2:	testb $0xFE, %al
	jnz 3f
	inb %dx, %al
	testb $0xFE, %al
	jz 2b
3:	RET_LABEL(smbus_wait_until_done)
#endif

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

#if SMBUS_SPD
smbus_read_byte:
	/* poll until the smbus is ready for commands */
	CALL_LABEL(smbus_wait_until_ready)

	/* clear any lingering errors, so that the transaction will run */
	movl $(SMBUS_IO_BASE + SMBHSTSTAT), %edx
	inb %dx, %al
	outb %al, %dx

	/* set the device I'm talking to */
	movl $(SMBUS_IO_BASE + SMBHSTADD), %edx
	movb %bl /* device */, %al
	shlb $1, %al
	orb  $1, %al
	outb %al, %dx

	/* set the command address... */
	movl $(SMBUS_IO_BASE + SMBHSTCMD), %edx
	movb %bh /* address */, %al
	outb %al, %dx

	/* clear the data byte */
	movl $(SMBUS_IO_BASE + SMBHSTDAT0), %edx
	xorl %eax, %eax
	outb %al, %dx

	/* start a byte read, with interrupts disabled */
	movl $(SMBUS_IO_BASE + SMBHSTCTL), %edx
	movl $((0x2 << 2) | (1 << 6)), %eax
	outb %al, %dx

	/* poll for transaction completion */
	CALL_LABEL(smbus_wait_until_done)

	/* read the results and see if we succeded */
	movl $(SMBUS_IO_BASE + SMBHSTSTAT), %edx
	inb %dx, %al
	testb $0x02, %al
	jz 1f
	movl $(SMBUS_IO_BASE + SMBHSTDAT0), %edx
	inb %dx, %al
1:
	RETSP
#else
	/* This table and code are used because I could not get
	   the smbus to work, so the table below is for dimm ram
	   that is compatable to the Samsung M383L3313BT1-B0.
	   The routine below simulates an smbus read for address
	   0x50 and 0x51.
	*/
smbus_spd_table:
.byte	0x80, 0x08, 0x07, 0x0c, 0x0a, 0x02, 0x48, 0x00
.byte	0x04, 0x75, 0x75, 0x02, 0x80, 0x08, 0x08, 0x01
.byte	0x0e, 0x04, 0x0c, 0x01, 0x02, 0x26, 0x00, 0xa0
.byte	0x75, 0x00, 0x00, 0x50, 0x3c, 0x50, 0x2d, 0x20
.byte	0x90, 0x90, 0x50, 0x50, 0x00, 0x00, 0x00, 0x00
smbus_spd_table_end:

smbus_read_byte:
	/* test for a valid device */
	cmpb $0x50, %bl
	jz  srb10;
	cmpb $0x51, %bl
	jnz srb_err
srb10:
	/* get the index in eax */
	xorl %eax, %eax
        movb %bh, %al
	/* load the byte from the table */
        movl $smbus_spd_table, %edx
	addl %eax, %edx
	movb 0(%edx), %al
	cmpl $smbus_spd_table_end, %edx
	jb   srb20 
	movb $0, %al
	/* clear the zero flag */
srb20:
	testl	%edx, %edx
	RETSP
srb_err:
	xorl %eax, %eax
	RETSP
#endif

smbus_end:
CALL_LABEL(setup_smbus)

