jmp intel_440_out

#define USE_SPD 1

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

#define SET_DRAMC						\
	CS_WRITE_BYTE(0x57, 0x8)				\

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

#define SMBUS_MEM_DEVICE_0 (0xa << 3)
#define SMBUS_MEM_DEVICE_1 (SMBUS_MEM_DEVICE_0 +1)
#define SMBUS_MEM_DEVICE_2 (SMBUS_MEM_DEVICE_0 +2)
#define SMBUS_MEM_DEVICE_3 (SMBUS_MEM_DEVICE_0 +3)


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

configure_rps_pgpol_drb:
	
	/* %bl is the row index */
	xorl %ebx, %ebx

	/* %si is the aggregation of bits for RPS */
	xorl %esi, %esi

	/* %di has bits to be set in PGPOL */
	xorl %edi, %edi

	/* %cx holds the cumulative memory size of DIMMs */
	xorl %ecx, %ecx

	
next_row:
	movzx %bl, %dx
	shl $16, %ebx
	shr $1, %dx
	mov $((5 << 8) | SMBUS_MEM_DEVICE_0), %bx
	or %dx, %bx 
        CALLSP(smbus_read_byte)
	jnz shift_before_moving_on 
	shr $16, %ebx

	testb $2, %al
	jnz two_rows
	testb $1, %bl
	/* this used to be jnz but seems it should be jz */
	jz ready_for_next_row

two_rows:
	movzx %bl, %dx
	shl $16, %ebx
	shr $1, %dx
	mov $((4 << 8) | SMBUS_MEM_DEVICE_0), %bx
	or %dx, %bx 
        CALLSP(smbus_read_byte)
	shr $16, %ebx
	
	subb $8, %al	
	shrd $2, %ax, %si
	
	movzx %bl, %dx
	shl $16, %ebx
	shr $1, %dx
	mov $((17 << 8) | SMBUS_MEM_DEVICE_0), %bx
	or %dx, %bx 
        CALLSP(smbus_read_byte)
	shr $16, %ebx

	cmpb $4, %al
	sete %al
	shrd $1, %ax, %di

	movzx %bl, %dx
	shl $16, %ebx
	shr $1, %dx
	mov $((31 << 8) | SMBUS_MEM_DEVICE_0), %bx
	or %dx, %bx 
        CALLSP(smbus_read_byte)
	shr $16, %ebx
	
	shr $1, %al
	xorb %ah, %ah
	add %ax, %cx
	movzx %cx, %edx
	movzx %bl, %eax
	addl $0x60, %eax
	PCI_WRITE_CONFIG_BYTE

	jmp 1f

shift_before_moving_on:
	shr $16, %ebx
ready_for_next_row:
	shr $2, %si
	shr $1, %di
	movzx %cx, %edx
	movzx %bl, %eax
	addl $0x60, %eax
	PCI_WRITE_CONFIG_BYTE

1:	
	inc %bl
	cmp MAX_ROWS, %bl
	jb next_row
		
/* Now to finally write the RPS and PGPOL registers */

	movzx %si, %ecx
	movl $0x74, %eax
	PCI_WRITE_CONFIG_WORD 
	
	mov %di, %cx
	movzx %ch, %edx
	movl $0x79, %eax
	PCI_WRITE_CONFIG_BYTE

RET_LABEL(configure_rps_pgpol_drb)

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
	 
spd_set_dramc:
	movl $((21 << 8) | SMBUS_MEM_DEVICE_0), %ebx
1:	CALLSP(smbus_read_byte)
	jnz 2f
	andl $0x12, %eax
	jmp spd_set_dramc_out

2:	addl $1, %ebx	/* increment the device */
	cmpb $SMBUS_MEM_DEVICE_3, %bl
	jbe 1b
	jmp no_memory	
	
spd_set_dramc_out:
	testb $0x2, %al
	movl $8, %eax
	jz 1f
	movl $0x10, %eax
1:	movl %eax, %edx
	movl $0x57, %eax
	PCI_WRITE_CONFIG_BYTE
	RET_LABEL(spd_set_dramc)


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
	CALL_LABEL(configure_sdramc)
	CALL_LABEL(configure_rps_pgpol_drb)
	CALL_LABEL(spd_set_dramc)
	CALL_LABEL(spd_set_nbxcfg)

	RET_LABEL(ram_set_spd_registers)
	

/* things that are not used */
#define FIRST_NORMAL_REFERENCE()
#define SPECIAL_FINISHUP()

intel_440_out:

