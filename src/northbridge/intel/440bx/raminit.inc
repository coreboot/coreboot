jmp intel_440_out

#define USE_SPD 1

#define REGISTERED_DRAM 		$0x10
#define NONREGISTERED_DRAM 		$0x08
#define REGISTERED_DRAM_REGISTER 	$0x57

#define SMBUS_MEM_DEVICE_0 (0xa << 3)
#define SMBUS_MEM_DEVICE_1 (SMBUS_MEM_DEVICE_0 +1)
#define SMBUS_MEM_DEVICE_2 (SMBUS_MEM_DEVICE_0 +2)
#define SMBUS_MEM_DEVICE_3 (SMBUS_MEM_DEVICE_0 +3)

#define LAST_SMBUS_MEM_DEVICE (SMBUS_MEM_DEVICE_3)

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

/* Default memory set to 0 */
#define DRB 0x00

/* The DRB register for the first row */
#define DRB_REG0 $0x60

/* The maximum allowed rows of memory banks */
#define MAX_ROWS $0x8

#define RAM_READ 0x0400

#define DIMM0_BASE \
	xorl %eax, %eax

#define DIMM_BASE(n) \
	movl $(0x60 + ((n) -1)), %eax	; \
	PCI_READ_CONFIG_BYTE		; \
	andl $0xFF, %eax		; \
	shll $23, %eax			; \

#define DIMM_READ \
	addl %ebx, %eax			; \
	movl (%eax), %edx		; \
	xorl $0xdff8, %eax		; \
	movl (%eax), %edx

#define DIMM0_READ DIMM0_BASE   ; DIMM_READ
#define DIMM1_READ DIMM_BASE(1) ; DIMM_READ
#define DIMM2_READ DIMM_BASE(2) ; DIMM_READ
#define DIMM3_READ DIMM_BASE(3) ; DIMM_READ
#define DIMM4_READ DIMM_BASE(4) ; DIMM_READ
#define DIMM5_READ DIMM_BASE(5) ; DIMM_READ
#define DIMM6_READ DIMM_BASE(6) ; DIMM_READ
#define DIMM7_READ DIMM_BASE(7) ; DIMM_READ

#define DIMMS_READ_EBX_OFFSET \
	DIMM0_READ	; \
	DIMM1_READ	; \
	DIMM2_READ	; \
	DIMM3_READ	; \
	DIMM4_READ	; \
	DIMM5_READ	; \
	DIMM6_READ	; \
	DIMM7_READ

#define DIMMS_READ(offset) \
	movl $offset, %ebx	; \
	DIMMS_READ_EBX_OFFSET

#define RAM_COMMAND_NONE	0x0
#define RAM_COMMAND_NOP		0x1
#define RAM_COMMAND_PRECHARGE	0x2
#define RAM_COMMAND_MRS		0x3
#define RAM_COMMAND_CBR		0x4

#define SET_RAM_COMMAND(command) \
	movl $0x76, %eax		; \
	PCI_READ_CONFIG_BYTE		; \
	andl $0x1F, %eax		; \
	orl  $((command) << 5), %eax	; \
	movl %eax, %edx			; \
	movl $0x76, %eax		; \
	PCI_WRITE_CONFIG_BYTE

#define COMPUTE_CAS_MODE \
	movl $0x76, %eax	; \
	PCI_READ_CONFIG_BYTE	; \
	andl $0x4,  %eax	; \
	xorl $0x4,  %eax	; \
	shll $2,    %eax	; \
	orl  $0x2a, %eax	; \

#define SET_RAM_MODE_REGISTER \
	SET_RAM_COMMAND(RAM_COMMAND_MRS)	; \
	COMPUTE_CAS_MODE	; \
	shll $3,    %eax	; \
	movl %eax,  %ebx	; \
	DIMMS_READ_EBX_OFFSET

#define ASSERT_RAM_COMMAND()		DIMMS_READ(RAM_READ)
#define ASSERT_MRS_RAM_COMMAND(mode)	DIMMS_READ(mode)
#define ENABLE_REFRESH()		CALL_LABEL(spd_enable_refresh)


/* Default values for config registers */

#define SET_NBXCFG \
	CS_WRITE_LONG(0x50, 0xff00a00c)

/*  This insures refresh is off */
#define SET_DRAMC						\
	CS_WRITE_BYTE(0x57, 0x8)

	/* PAM - Programmable Attribute Map Registers */
	/* Ideally we want to enable all of these as DRAM and teach
	 * linux it is o.k. to use them...
	 */
#define SET_PAM \
	CS_WRITE_BYTE(0x59, 0x00)				; \
	CS_WRITE_BYTE(0x5a, 0x00)				; \
	CS_WRITE_BYTE(0x5b, 0x00)				; \
	CS_WRITE_BYTE(0x5c, 0x00)				; \
	CS_WRITE_BYTE(0x5d, 0x00)				; \
	CS_WRITE_BYTE(0x5e, 0x00)				; \
	CS_WRITE_BYTE(0x5f, 0x00)

#define SET_DRB \
	/* DRB - DRAM Row Boundary Registers */			\
	CS_WRITE_BYTE(0x60,  DRB)				; \
	CS_WRITE_BYTE(0x61,  DRB)				; \
	CS_WRITE_BYTE(0x62,  DRB)				; \
	CS_WRITE_BYTE(0x63,  DRB)				; \
	CS_WRITE_BYTE(0x64,  DRB)				; \
	CS_WRITE_BYTE(0x65,  DRB)				; \
	CS_WRITE_BYTE(0x66,  DRB)				; \
	CS_WRITE_BYTE(0x67,  DRB)

#define SET_FDHC \
	CS_WRITE_BYTE(0x68,  0x00)

#define SET_RPS \
	CS_WRITE_WORD(0x74, 0x0000)

#define SET_SDRAMC \
	CS_WRITE_BYTE(0x76, 0x00)

#define SET_PGPOL \
	CS_WRITE_WORD(0x78, 0xff00)

	/* PMCR - Power Management Control Register
	   Enable normal refresh operation and
	   the gated clock	*/
#define SET_PMCR \
	CS_WRITE_BYTE(0x7a, 0x14)

ram_set_registers:
	SET_NBXCFG
	SET_DRAMC
	SET_PAM
	SET_DRB
	SET_FDHC
	SET_RPS
	SET_SDRAMC
	SET_PGPOL
	SET_PMCR
	RET_LABEL(ram_set_registers)


#define DEVFN(device, function) (((device) << 3) + (function))
#define CONFIG_ADDR(bus,devfn,where) (((bus) << 16) | ((devfn) << 8) | (where))
#define PM_FUNCTION  CONFIG_ADDR(0, PIIX4_DEVFN+3, 0)

#define SMBUS_IO_BASE 0x1000
#define SMBHSTSTAT 0
#define SMBHSTCTL  2
#define SMBHSTCMD  3
#define SMBHSTADD  4
#define SMBHSTDAT0 5
#define SMBHSTDAT1 6
#define SMBBLKDAT  7

enable_smbus:
	CS_WRITE_LONG(PM_FUNCTION + 0x90, SMBUS_IO_BASE | 1)   /* iobase addr */
	CS_WRITE_BYTE(PM_FUNCTION + 0xd2, (0x4 << 1) | 1)      /* smbus enable */
	CS_WRITE_WORD(PM_FUNCTION + 0x4, 1)                    /* iospace enable */
	RET_LABEL(enable_smbus)

	/*
	 * Routine:	setup_smbus
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	eax, edx
	 * Effects:	The smbus is enabled
	 */
setup_smbus:
	mov $0x1e, %al
	movl $(SMBUS_IO_BASE + SMBHSTSTAT), %edx
	outb %al, %dx
	RET_LABEL(setup_smbus)

smbus_wait_until_ready:
	movl $(SMBUS_IO_BASE + SMBHSTSTAT), %edx
1:	inb %dx, %al
	testb $1, %al
	jnz 1b
	RET_LABEL(smbus_wait_until_ready)

smbus_wait_until_done:
	movl $(SMBUS_IO_BASE + SMBHSTSTAT), %edx
1:	inb %dx, %al
	testb $1, %al
	jnz 1b
	testb $0xfe, %al
	jz 1b			/* wait until interrupt status is set */
	RET_LABEL(smbus_wait_until_done)

smbus_read_byte:
	/* poll until the smbus is ready for commands */
	CALL_LABEL(smbus_wait_until_ready)

	/* clear any lingering errors, so that the transaction will run */
	movl $(SMBUS_IO_BASE + SMBHSTSTAT), %edx
	mov $0x1e, %al
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
1:	movl $(SMBUS_IO_BASE + SMBHSTSTAT), %edx
	inb %dx, %al
	testb $0x04, %al
	jnz 1f
	movl $(SMBUS_IO_BASE + SMBHSTDAT0), %edx
	inb %dx, %al
1:
	RETSP

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
	movl $((0x60 << 16) |SMBUS_MEM_DEVICE_0), %ebx
spd_set_drb_loop_top:
	xorl %edi, %edi
	subl $1,   %edi
	xorl %esi, %esi
	subl $1,   %esi

	movb $3,   %bh			/* rows */
	CALLSP(smbus_read_byte)
	jnz 20f
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
	subl $25, %edi /* Make it multiples of 8MB */

	/* side two */
	movb $5,    %bh			/* number of physical banks */
	CALLSP(smbus_read_byte)
	cmp $1, %al
	jbe 20f

	/* for now only handle the symmetrical case */
	movl %edi, %esi
20:
	/* Compute the end address for the DRB register */
	cmpl $8, %edi  /* Ignore the dimm if it is over 2GB */
	jae 21f
	movl $1, %eax
	movl %edi, %ecx
	shll %cl, %eax
	addl %eax, %ebp
21:
	/* Write the computed value for the first half of the DIMM */
	movl %ebp, %edx /* value to write into %edx */
	movl %ebx, %eax
	shrl $16,  %eax /* port address into %eax */
	PCI_WRITE_CONFIG_BYTE

	/* Compute the end address for the DRB register */
	cmpl $8, %esi /* Ignore the dimm if it is over 2GB */
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
	cmpb $SMBUS_MEM_DEVICE_3, %bl /* see if I have reached the end */
	jbe spd_set_drb_loop_top

	/* o.k. I'm done return now */
	RET_LABEL(spd_set_drb)


spd_set_dramc:

	/* auto detect if ram is registered or not. */
	/* The DRAMC register also contorls the refresh rate but we can't
         * set that here because we must leave refresh disabled.
	 * see:	spd_enable_refresh
	 */
	/* Find the first dimm and assume the rest are the same */
	/* Load the smbus device and port int %ebx */
	mov $((21 << 8) | SMBUS_MEM_DEVICE_0), %bx
1:	CALLSP(smbus_read_byte)
	jnz 2f
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

	Richard Smith ported this from the 440gx board.
	 one thing to note is that the polarity of an error from
	smbus_read_byte is opposite of the 440gx
	 */

spd_set_rps:
	/* The RPS register holds the size of a ``page'' of DRAM on each DIMM */
	/* default all page sizes to 2KB */
	xorl %esi, %esi
	/* Index into %esi of bit to set */
	movl $0 , %ecx
	/* Load the smbus device into %ebx */
	movl $SMBUS_MEM_DEVICE_0, %ebx

1:	movb $4, %bh
	CALLSP(smbus_read_byte)		/* row address bits */
	jnz 2f
	andl $0xf, %eax
	movl %eax, %edi

	/* Number of colums indicates which row page size to use */
	subl $8, %edi
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
	/* one too many shifts here. */
	/*	shll %cl, %edi*/
	orl %edi, %esi

2:	addl $1, %ebx	/* increment the device */
	addl $4, %ecx   /* increment the shift count */
	cmpb $SMBUS_MEM_DEVICE_3, %bl
	jbe 1b

/* next block is for Ron's attempt to get registered to work. */
/* we have just verified that we have to have this code. It appears that
 * the registered SDRAMs do indeed set the RPS wrong. sheesh.
 */
	/* at this point, %esi holds the RPS for all ram.
	 * we have verified that for registered DRAM the values are
	 * 1/2 the size they should be. So we test for registered
	 * and then double the sizes if needed.
	 */
	movl $0x57, %eax
	PCI_READ_CONFIG_BYTE

	/* is it registered? */
	testb $0x10, %eax
	jz	1f

	/* BIOS makes weird page size for registered! */
	/* what we have found is you need to set the EVEN banks to
	 * twice the size. Fortunately there is a very easy way to
	 * do this. First, read the WORD value of register 0x74.
	 */

	/* now to double the size of the EVEN banks we only need to add 1 */
        /* because the size is log2
         */
	addl $0x1111, %esi
	/* now write that final value of %esi into register 0x74 */
1:
	movl	%esi, %ecx
	movl $0x74, %eax
	PCI_WRITE_CONFIG_WORD

	RET_LABEL(spd_set_rps)

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
	jnz 2f
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
	cmpb $SMBUS_MEM_DEVICE_3, %bl
	jbe 1b

	shll $8, %esi
	orl $0x7, %esi /* 32 clocks idle time */
	movl %esi, %ecx
	movl $0x78, %eax
	PCI_WRITE_CONFIG_WORD
	RET_LABEL(spd_set_pgpol)

configure_sdramc:
	movl $((18 << 8) | SMBUS_MEM_DEVICE_0), %ebx
0:      CALLSP(smbus_read_byte)
	jnz 1f

	testb $2, %al
	jz 2f	      /* Default settings are OK if only CAS=3 is supported */
	movl $0x06, %edx
	movl $0x76, %eax
	PCI_WRITE_CONFIG_BYTE
	jmp 2f

1:	addl $1, %ebx	/* increment the device */
	cmpb $SMBUS_MEM_DEVICE_3, %bl
	jbe 0b
2:
	RET_LABEL(configure_sdramc)


	/*
	 * Routine:	spd_enable_refresh
	 * Arguments:	None
	 *
	 * Trashed:	%eax, %ebx, %ecx, %edx, %esp, %eflags
	 * Effects:	Uses serial presence detect to set the
	 *              refresh rate in the DRAMC register.
	 *		see spd_set_dramc for the other values.
	 * FIXME:	Check for illegal/unsupported ram configurations and abort
	 */

refresh_rates:
	.byte 0x01 /* Normal        15.625 us -> 15.6 us */
	.byte 0x05 /* Reduced(.25X) 3.9 us    -> 7.8 us */
	.byte 0x05 /* Reduced(.5X)  7.8 us    -> 7.8 us */
	.byte 0x02 /* Extended(2x)  31.3 us   -> 31.2 us */
	.byte 0x03 /* Extended(4x)  62.5 us   -> 62.4 us */
	.byte 0x04 /* Extended(8x)  125 us    -> 124.8 us */

spd_enable_refresh:
	/* Find the first dimm and assume the rest are the same */
	/* Load the smbus device and port int %ebx */
	movl $((12 << 8) | SMBUS_MEM_DEVICE_0), %ebx
1:	CALLSP(smbus_read_byte)
	jnz 2f
	andl $0x7f, %eax
	jmp spd_enable_refresh_out

2:	addl $1, %ebx	/* increment the device */
	cmpb $SMBUS_MEM_DEVICE_3, %bl
	jbe 1b
	/* We couldn't find anything we must have no memory */
	jmp no_memory

spd_enable_refresh_out:
	cmpb $0x06, %al
	ja	1f
	addl $refresh_rates, %eax
	movb (%eax), %cl
	jmp 2f
1:	movb $0x05, %cl
2:	movl $0x57, %eax
	PCI_READ_CONFIG_BYTE
	andb $0xf8, %al
	orb %cl, %al
	movb %al, %dl
	movl $0x57, %eax
	PCI_WRITE_CONFIG_BYTE
	RET_LABEL(spd_enable_refresh)

spd_set_nbxcfg:
	/* say all dimms have no ECC support */
	movl $0xFF, %esi
	/* Index into %esi of bit to set */
	movl $0	  , %ecx
	/* Load the smbus device into %ebx */
	movl $SMBUS_MEM_DEVICE_0, %ebx

1:	movb $11, %bh
	CALLSP(smbus_read_byte)		/* module error correction type */
	jnz 2f
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
	cmpb $SMBUS_MEM_DEVICE_3, %bl
	jbe 1b

	movl %esi, %edx
	movl $0x53, %eax
	PCI_WRITE_CONFIG_BYTE

	movl $((126 << 8) | SMBUS_MEM_DEVICE_0), %ebx
	CALLSP(smbus_read_byte)

	RET_LABEL(spd_set_nbxcfg)

ram_set_spd_registers:
	CALL_LABEL(enable_smbus)
	CALL_LABEL(setup_smbus)
	CALL_LABEL(spd_set_drb)
	CALL_LABEL(spd_set_dramc)
	CALL_LABEL(spd_set_rps)
	CALL_LABEL(configure_sdramc)
	CALL_LABEL(spd_set_pgpol)
	CALL_LABEL(spd_set_nbxcfg)

	RET_LABEL(ram_set_spd_registers)


/* things that are not used */
#define FIRST_NORMAL_REFERENCE()
#define SPECIAL_FINISHUP()

intel_440_out:

