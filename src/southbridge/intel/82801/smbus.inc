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
jmp smbus_pcibus_end

/* generic SMB routines that work for many systems. The only one that might
 * not work is the enable_smbus. 
 * you have to define PM_FUNCTION for this to work. 
 */#define SMBUS_IO_BASE 0xf00
#define SMBHSTSTAT 0
#define SMBHSTCTL  2
#define SMBHSTCMD  3
#define SMBHSTADD  4
#define SMBHSTDAT0 5
#define SMBHSTDAT1 6
#define SMBBLKDAT  7

enable_smbus:	
	/* put the SMBUS at port 0xf00 */
	CS_WRITE_LONG(PM_DEVFN+ 0x90, SMBUS_IO_BASE|1)   /* iobase addr */
	CS_WRITE_BYTE(PM_DEVFN + 0xd2, (0x4 << 1) | 1)      /* smbus enable */
	CS_WRITE_WORD(PM_DEVFN + 0x4, 1)                    /* iospace enable */
	RET_LABEL(enable_smbus)

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

/* now for code to actually do the deed. Eric did such a good job that
 * this stuff is basically generic. 
 */
	/*
	 * Routine:	spd_set_drb
	 * Arguments:	None
	 *
	 * Trashed:	%eax, %ebx, %ecx, %edx, %esi, %edi, %ebp, %esp, %eflags
	 * Effects:	Uses serial presence detect to set the
	 *              DRB registers which holds the ending memory address assigned
	 *              to each DIMM.
	 * Notes:	%ebp holds the currently detected end of memory.
	 *              %ebx holds the configuration port & SMBUS_MEM_DEVICE for
	 *                   the current iteration through the loop.
	 *              %edi holds the memory size for the first side of the DIMM.
	 *		%esi holds the memory size for the second side of the DIMM.
	 *		     memory size is represent as a power of 2.
	 *                   An unset memory size is represented as -1 ie. 0xFFFFFFFF
	 */

spd_set_drb:
	xorl %ebp, %ebp /* clear the memory address */
	movl $((DRAM_CONFIG_PORT << 16) |SMBUS_MEM_DEVICE_0), %ebx
spd_set_drb_loop_top:	
	// set -1 power-of-two for side 1 (called bank0 in most chipset docs)
	xorl %edi, %edi
	subl $1,   %edi
	// set -1 power-of-two for side 2 (called bank1 in most chipset docs)
	xorl %esi, %esi
	subl $1,   %esi

	movb $3,   %bh			/* rows */
	CALLSP(smbus_read_byte)	
	// If it's zero, then we just set current %ebp into the row 
	// end register 
	jz 20f
	andl $0xf, %eax
	addl %eax, %edi
	
	movb $4,   %bh			/* columns */
	CALLSP(smbus_read_byte)
	andl $0xf, %eax
	addl %eax, %edi

	movb $17,  %bh			/* banks */
	CALLSP(smbus_read_byte)
	andl $0xff, %eax
	bsrl %eax, %ecx
	addl %ecx, %edi
	
	/* Get the module data width and convert it to a power of two */
	movb $7,   %bh			/* (high byte) */
	CALLSP(smbus_read_byte)
	andl $0xff, %eax
	movl %eax, %ecx
	shll $8, %ecx

	movb $6,   %bh			/* (low byte) */
	CALLSP(smbus_read_byte)
	andl $0xff, %eax
	orl  %eax,  %ecx
	bsrl %ecx,  %eax
	addl %eax,  %edi
	
	/* now I have the ram size in bits as a power of two (less 1) */
	// It is less 1 since we started with -1 above. 
	// OK, BITS as power of two (but minus 1) 
	// So, e.g., 8 MB is 64 Mb, 64 Mb is 26 bits. Subtract 
	// (26-1) or 25
	subl $25, %edi /* Make it multiples of 8MB */

	/* side two */
	movb $5,    %bh			/* number of physical banks */
	CALLSP(smbus_read_byte)
	cmp $1, %al
	// it's only one bank
	jbe 20f
	// It's two banks. So assign edi to esi
	/* for now only handle the symmetrical case */
	// it's two banks, assume however that they're the same size. 
	// it's stupid to have any other kind, right?
	movl %edi, %esi
20:
	/* Compute the end address for the DRB register */
	// If it is >= 8, i.e. >= 2^8 or 256, skip it.
	// >= 8 is a bogus value. 
	cmpl $8, %edi
	jae 21f
	movl $1, %eax
	movl %edi, %ecx
	shll %cl, %eax
	// increment row-end by the size of this DIMM half
	addl %eax, %ebp
21:
	/* Write the comuputed value for the first half of the DIMM */
	movl %ebp, %edx /* value to write into %edx */
	movl %ebx, %eax
	shrl $16,  %eax /* port address into %eax */
	PCI_WRITE_CONFIG_BYTE
	
	/* Compute the end address for the DRB register */
	cmpl $8, %esi
	jae 30f
	mov $1, %eax
	movl %esi, %ecx
	shll %cl,  %eax
	addl %eax, %ebp
30:	
	/* Write the comuputed value for the second half of the DIMM */
	movl %ebp, %edx /* value to write into %edx */
	movl %ebx, %eax
	shrl $16,  %eax /* port address into %eax */
	addl $1,   %eax /* The second half uses one port high */
	PCI_WRITE_CONFIG_BYTE

	addl $0x00020001, %ebx /* increment the smbus device & the config port */
	cmpb $LAST_SMBUS_MEM_DEVICE, %bl /* see if I have reached the end */
	jbe spd_set_drb_loop_top

	/* o.k. I'm done return now */
	RET_LABEL(spd_set_drb)

	/*
	 * Routine:	spd_set_dramc
	 * Arguments:	None
	 *
	 * Trashed:	%eax, %ebx, %edx, %esp, %eflags
	 * Effects:	Uses serial presence detect to set the
	 *              DRAMC register, which records if ram is registerd or not,
	 *              and controls the refresh rate.
	 *              The refresh rate is not set here, as memory refresh
	 *              cannot be enbaled until after memory is initialized.
	 *		see spd_enable_refresh.
	 * Notes:	
	 * FIXME:	Check for illegal/unsupported ram configurations and abort
	* FIXME: won't work with non-contiguous DRAM size regs (like VIA)
	* need an indirect pointer to an array of reg #s
	 */

spd_set_dramc:
	/* auto detect if ram is registered or not. */
	/* The DRAMC register also contorls the refresh rate but we can't
         * set that here because we must leave refresh disabled.
	 * see:	spd_enable_refresh
	 */
	/* Find the first dimm and assume the rest are the same */
	/* Load the smbus device and port int %ebx */
	movl $((21 << 8) | SMBUS_MEM_DEVICE_0), %ebx
1:	CALLSP(smbus_read_byte)
	jz 2f
	andl $0x12, %eax
	jmp spd_set_dramc_out

2:	addl $1, %ebx	/* increment the device */
	cmpb $LAST_SMBUS_MEM_DEVICE, %bl
	jbe 1b
	/* We couldn't find anything we must have no memory */
	jmp no_memory	
	
spd_set_dramc_out:
	testb $0x12, %al
	jz 2f
	movl REGISTERED_DRAM, %eax
	jmp 1f
2:	movl NONREGISTERED_DRAM, %eax
1:	movl %eax, %edx
	movl REGISTERED_DRAM_REGISTER, %eax
	PCI_WRITE_CONFIG_BYTE
	RET_LABEL(spd_set_dramc)



	/*
	 * Routine:	spd_read_refresh
	 * Arguments:	None
	 *
	 * Trashed:	%eax, %ebx, %ecx, %edx, %esp, %eflags
	 * Effects:	Uses serial presence detect to find refresh rates. 
	 *              returns the rate in %eax
	 *              It's up to you to set hardware up. 
	 * FIXME:	Check for illegal/unsupported ram configurations and abort
	 */


spd_read_refresh:
	/* Find the first dimm and assume the rest are the same */
	/* Load the smbus device and port int %ebx */
	movl $((12 << 8) | SMBUS_MEM_DEVICE_0), %ebx
1:	CALLSP(smbus_read_byte)
	jz 2f
	andl $0x7f, %eax
	jmp spd_enable_refresh_out

2:	addl $1, %ebx	/* increment the device */
	cmpb $LAST_SMBUS_MEM_DEVICE, %bl
	jbe 1b
	/* We couldn't find anything we must have no memory */
	xorl %eax, %eax
spd_enable_refresh_out:
	RET_LABEL(spd_enable_refresh)

	/*
	 * Routine:	spd_set_rps
	 * Arguments:	None
	 *
	 * Trashed:	%eax, %ebx, %ecx, %edx, %esi, %edi, %esp, %eflags
	 * Effects:	Uses serial presence detect to set the row size 
	 *		on a given DIMM
	 * Notes:	%esi accumulates the row sizes of all of the DIMMs
	 *		%ecx holds the current bit into into %esi
	 *		%bl  holds the current SMBUS device
	 * FIXME:	Check for illegal/unsupported ram configurations and abort
	 */

spd_set_rps:
	/* The RPS register holds the size of a ``page'' of DRAM on each DIMM */
	/* default all page sizes to 2KB */
	xorl %esi, %esi
	/* Index into %esi of bit to set */
	movl $0	  , %ecx
	/* Load the smbus device into %ebx */
	movl $SMBUS_MEM_DEVICE_0, %ebx
	
1:	movb $3, %bh
	CALLSP(smbus_read_byte)		/* row address bits */
	jz 2f
	andl $0xf, %eax
	movl %eax, %edi
	/* I now have the row page size as a power of 2 */
	subl $11, %edi /* Now make it in multiples of 2Kb */
	jbe 2f
	/* FIXME: do something with page sizes greather than 8KB!! */
	shll %cl, %edi
	orl %edi, %esi
	/* side two */
	movb $5, %bh
	CALLSP(smbus_read_byte)		/* number of physical banks */
	cmp $1, %al
	jbe 2f
	/* for now only handle the symmtrical case */
	shll $2,  %edi
	shll %cl, %edi
	orl %edi, %esi

2:	addl $1, %ebx	/* increment the device */
	addl $4, %ecx   /* increment the shift count */
	cmpb $LAST_SMBUS_MEM_DEVICE, %bl
	jbe 1b

	movl $0x7f, %eax
/* I'm not sure what we should do here. Do nothing. */
/* 	PCI_WRITE_CONFIG_WORD*/ 
	RET_LABEL(spd_set_rps)

	/*
	 * Routine:	spd_set_pgpol
	 * Arguments:	None
	 *
	 * Trashed:	%eax, %ebx, %ecx, %edx, %esi, %esp, %eflags
	 * Effects:	Uses serial presence detect to set the number of banks
	 *		on a given DIMM
	 * Notes:	%esi accumulates the banks sizes of all of the DIMMs
	 *		%ecx holds the current bit into into %esi
	 *		%bl  holds the current SMBUS device
	 * FIXME:	Check for illegal/unsupported ram configurations and abort
	 */

spd_set_pgpol:
	/* The PGPOL register stores the number of logical banks per DIMM,
	 * and number of clocks the DRAM controller waits in the idle
	 * state.
	 */
	/* default all bank counts 2 */
	xorl %esi, %esi
	/* Index into %esi of bit to set */
	movl $0	  , %ecx
	/* Load the smbus device into %ebx */
	movl $SMBUS_MEM_DEVICE_0, %ebx

1:	movb $17, %bh
	CALLSP(smbus_read_byte)		/* logical banks */
	jz 2f
	cmp $0x4, %eax
	jl 2f
	movl $0x1, %eax
	shll %cl,  %eax
	orl  %eax, %esi
	/* side two */
	movb $5, %bh
	CALLSP(smbus_read_byte)		/* number of physical banks */
	cmp $1, %al
	jbe 2f
	/* for now only handle the symmtrical case */
	movl $0x2, %eax
	shll %cl,  %eax
	orl  %eax, %esi

2:	addl $1, %ebx	/* increment the device */
	addl $2, %ecx   /* increment the shift count */
	cmpb $LAST_SMBUS_MEM_DEVICE, %bl
	jbe 1b

	shll $8, %esi
	orl $0x7, %esi /* 32 clocks idle time */
	movl %esi, %ecx
	movl $0x78, %eax
/* I'm unclear on the concept for non-intel devices */
/*	PCI_WRITE_CONFIG_WORD*/
	RET_LABEL(spd_set_pgpol)



	/*
	 * Routine:	spd_enable_nbxcfg
	 * Arguments:	None
	 *
	 * Trashed:	%eax, %ebx, %ecx, %edx, %esi, %esp, %eflags
	 * Effects:	Uses serial presence detect to set the
	 *              ECC support flags in the NBXCFG register
	 * Notes:	%esi accumulates the ECC support of the individual DIMMs.
	 *              %ecx holds the bit that should be flipped for the current DIMM.
	 *		%bl holds the smbus device that corresponds to the current DIMM.
	 * FIXME:	Check for illegal/unsupported ram configurations and abort
	 */

spd_set_nbxcfg:
	/* say all dimms have no ECC support */
	movl $0xFF, %esi
	/* Index into %esi of bit to set */
	movl $0	  , %ecx
	/* Load the smbus device into %ebx */
	movl $SMBUS_MEM_DEVICE_0, %ebx
	
1:	movb $11, %bh
	CALLSP(smbus_read_byte)		/* module error correction type */
	jz 2f
	cmp $0x2, %eax			/* 0 == None, 1 == Parity, 2 == ECC */
	jne 2f
	movl $0x1, %eax
	shll %cl,  %eax
	xorl %eax, %esi
	
	/* side two */
	movb $5, %bh
	CALLSP(smbus_read_byte)		/* number of physical banks */
	cmp $1, %al
	jbe 2f
	/* The only is the symmtrical case */
	movl $0x2, %eax
	shll %cl,  %eax
	xorl %eax, %esi

2:	addl $1, %ebx	/* increment the device */
	addl $2, %ecx   /* increment the shift count */
	cmpb $LAST_SMBUS_MEM_DEVICE, %bl
	jbe 1b

	movl %esi, %edx
/* at some point, we need to indicate how to turn ECC on. Not yet. 
	movl $0x53, %eax
	PCI_WRITE_CONFIG_BYTE
 */
	RET_LABEL(spd_set_nbxcfg)



smbus_pcibus_end:
CALL_LABEL(enable_smbus)
CALL_LABEL(setup_smbus)


