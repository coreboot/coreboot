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
	
#if 0
#define DRB0 0x08
#define DRB1 0x10
#define DRB2 0x18
#define DRB3 0x20
#define DRB4 0x20
#define DRB5 0x20
#define DRB6 0x20
#define DRB7 0x20
#else
/* Conservative setting 8MB of ram on first DIMM... */
#define DRB0 0x01
#define DRB1 0x01
#define DRB2 0x01
#define DRB3 0x01
#define DRB4 0x01
#define DRB5 0x01
#define DRB6 0x01
#define DRB7 0x01
#endif

#define CAS_LATENCY 3

	/* CAS latency 2 */
#if (CAS_LATENCY == 2)
#define CAS_NB 0x17
	/* 
	 * 7 == 0111
	 * 1 == 0001
	 */
#define CAS_MODE 0x2a
	/*
	 * a == 1010
	 * 2 == 0010
	 */
#endif

	/* CAS latency 3 */
#if (CAS_LATENCY == 3) 
#define CAS_NB 0x13
	/*
	 * 3 == 0011
	 * 1 == 0001
	 */
#define CAS_MODE 0x3a
	/*
	 * a == 1010
	 * 3 == 0011
	 */
#endif

#ifndef CAS_NB
#error "Nothing defined" 
#endif


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
#if ! USE_SPD
#define ENABLE_REFRESH()	\
	movl $(0x57), %eax	; \
	PCI_READ_CONFIG_BYTE	; \
	orb 0x1, %al		; \
	mov %al, %dl		; \
	movl $(0x57), %eax	; \
	PCI_WRITE_CONFIG_BYTE
	
#else /* USE_SPD */
#define ENABLE_REFRESH()		CALL_LABEL(spd_enable_refresh)
#endif


/* Default values for config registers */
	
	/* NBXCFG 0x50 - 0x53 */
	/* f == 1111
	 * 0 == 0000
	 * 0 == 0000
	 * 0 == 0000
	 * 0 == 0000
	 * 1 == 0001
	 * 8 == 1000
	 * c == 1100
	 * SDRAM Row without ECC:
	 * row 0 == 1 No ECC
	 * row 1 == 1 No ECC
	 * row 2 == 1 No ECC
	 * row 3 == 1 No ECC
	 * row 4 == 1 No ECC
	 * row 5 == 1 No ECC
	 * row 6 == 1 No ECC
	 * row 7 == 1 No ECC
	 * Host Bus Fast Data Ready Enable == 0 Disabled
	 * IDSEL_REDIRECT == 0 (430TX compatibility disable?)
	 * WSC# Hanshake Disable == 0 enable (Use External IOAPIC)
	 * Host/DRAM Frequence == 00 100Mhz
	 * AGP to PCI Access Enable == 0 Disable
	 * PCI Agent to Aperture Access Disable == 0 Enable (Ignored)
	 * Aperture Access Global Enable == 0 Disable
	 * DRAM Data Integrity Mode == 11 (Error Checking/Correction)
	 * ECC Diagnostic Mode Enable == 0 Not Enabled
	 * MDA present == 0 Not Present
	 * USWC Write Post During During I/O Bridge Access Enable == 1 Enabled
	 * In Order Queue Depth (IQD) (RO) == ?? 
	 */
#if 0
#define SET_NBXCFG \
	CS_WRITE_LONG(0x50, 0xff00018c)
#else
#define SET_NBXCFG \
	CS_WRITE_LONG(0x50, 0xff00000c)
#endif

#define SET_DRAMC						\
	/* DRAMC */						\
	CS_WRITE_BYTE(0x57, 0x8)				\
	/* 0 == 0000						\
	 * 8 == 1000						\
	 * Not registered SDRAM					\
	 * refresh disabled					\
	 */

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
	CS_WRITE_BYTE(0x60,  DRB0)				; \
	CS_WRITE_BYTE(0x61,  DRB1)				; \
	CS_WRITE_BYTE(0x62,  DRB2)				; \
	CS_WRITE_BYTE(0x63,  DRB3)				; \
	CS_WRITE_BYTE(0x64,  DRB4)				; \
	CS_WRITE_BYTE(0x65,  DRB5)				; \
	CS_WRITE_BYTE(0x66,  DRB6)				; \
	CS_WRITE_BYTE(0x67,  DRB7)

	/* FDHC */
#define SET_FDHC \
	CS_WRITE_BYTE(0x68,  0x00)


#if 0
	/* MBSC - Memory Buffer Strength Control */
	/* 00c00003e820
	 * [47:44] 0 == 0000
	 * [43:40] 0 == 0000
	 * [39:36] c == 1100
	 * [35:32] 0 == 0000
	 * [31:28] 0 == 0000
	 * [27:24] 0 == 0000
	 * [23:20] 0 == 0000
	 * [19:16] 3 == 0011
	 * [15:12] e == 1110
	 * [11: 8] 8 == 1000
	 * [ 7: 4] 2 == 0010
	 * [ 3: 0] 0 == 0000
	 * MAA[14:0]#, WEA#, SRASA#, SCASA# Buffer Strengths  ==  3x
	 * MAB[14,13,10,12:11,9:0]#, WEB#, SRASB#, SCASB# Buffer Strengths == 1x
	 * MD[63:0]# Buffer Strength Control 2 == 1x
	 * MD[63:0]# Buffer Strength Control 1 == 1x
	 * MECC[7:0] Buffer Strength Control 2 == 1x
	 * MECC[7:0] Buffer Strength Control 1 == 1x
	 * CSB7# Buffer Strength == 1x	
	 * CSA7# Buffer Strength == 1x
	 * CSB6# Buffer Strength == 1x
	 * CSA6# Buffer Strength == 1x
	 * CSA5#/CSB5# Buffer Strength == 1x
	 * CSA4#/CSB4# Buffer Strength == 1x
	 * CSA3#/CSB3# Buffer Strength == 2x
	 * CSA2#/CSB2# Buffer Strength == 2x
	 * CSA1#/CSB1# Buffer Strength == 2x
	 * CSA0#/CSB0# Buffer Strength == 2x
	 * DQMA5 Buffer Strength == 2x
	 * DQMA1 Buffer Strength == 2x
	 * DQMB5 Buffer Strength == 1x
	 * DQMB1 Buffer Strength == 1x
	 * DQMA[7:6,4:2,0] Buffer Strength == 2x
	 * GCKE Buffer Strength == 1x
	 * FENA Buffer Strength == 1x
	 */

#define SET_MBSC \
	CS_WRITE_BYTE(0x69, 0x20)	; \
	CS_WRITE_BYTE(0x6a, 0xe8)	; \
	CS_WRITE_BYTE(0x6b, 0x03)	; \
	CS_WRITE_BYTE(0x6c, 0x00)	; \
	CS_WRITE_BYTE(0x6d, 0xc0)	; \
	CS_WRITE_BYTE(0x6e, 0x00)
#else
	/* MBSC - Memory Buffer Strength Control */
	/* 00c00003e820
	 * [47:44] 0 == 0000
	 * [43:40] 0 == 0000
	 * [39:36] c == 1100
	 * [35:32] 0 == 0000
	 * [31:28] 0 == 0000
	 * [27:24] 0 == 0000
	 * [23:20] 0 == 0000
	 * [19:16] 3 == 0011
	 * [15:12] e == 1110
	 * [11: 8] 8 == 1000
	 * [ 7: 4] 2 == 0010
	 * [ 3: 0] 0 == 0000
	 * MAA[14:0]#, WEA#, SRASA#, SCASA# Buffer Strengths  ==  3x
	 * MAB[14,13,10,12:11,9:0]#, WEB#, SRASB#, SCASB# Buffer Strengths == 3x
	 * MD[63:0]# Buffer Strength Control 2 == 3x
	 * MD[63:0]# Buffer Strength Control 1 == 3x
	 * MECC[7:0] Buffer Strength Control 2 == 3x
	 * MECC[7:0] Buffer Strength Control 1 == 3x
	 * CSB7# Buffer Strength == 3x	
	 * CSA7# Buffer Strength == 3x
	 * CSB6# Buffer Strength == 3x
	 * CSA6# Buffer Strength == 3x
	 * CSA5#/CSB5# Buffer Strength == 2x
	 * CSA4#/CSB4# Buffer Strength == 2x
	 * CSA3#/CSB3# Buffer Strength == 2x
	 * CSA2#/CSB2# Buffer Strength == 2x
	 * CSA1#/CSB1# Buffer Strength == 2x
	 * CSA0#/CSB0# Buffer Strength == 2x
	 * DQMA5 Buffer Strength == 2x
	 * DQMA1 Buffer Strength == 3x
	 * DQMB5 Buffer Strength == 2x
	 * DQMB1 Buffer Strength == 2x
	 * DQMA[7:6,4:2,0] Buffer Strength == 3x
	 * GCKE Buffer Strength == 1x
	 * FENA Buffer Strength == 3x
	 */
#define SET_MBSC \
	CS_WRITE_BYTE(0x69, 0xB3)	; \
	CS_WRITE_BYTE(0x6a, 0xee)	; \
	CS_WRITE_BYTE(0x6b, 0xff)	; \
	CS_WRITE_BYTE(0x6c, 0xff)	; \
	CS_WRITE_BYTE(0x6d, 0xff)	; \
	CS_WRITE_BYTE(0x6e, 0x03)
#endif


	/* 0x72 SMRAM */
	/* 1 == 0001
	 * a == 1010
	 * SMM Compatible base segment == 010 (Hardcoded value)
	 */
#define SET_SMRAM
	/* 0x73 ESMRAMC */
#define SET_ESRAMC

	/* RPS - Row Page Size Register */
	/* 0x0055
	 * [15:12] 0 == 0000
	 * [11: 8] 0 == 0000
	 * [ 7: 4] 5 == 0101
	 * [ 3: 0] 5 == 0101
	 * DRB[0] == 4KB
	 * DRB[1] == 4KB
	 * DRB[2] == 4KB
	 * DRB[3] == 4KB
	 * DRB[4] == 2KB
	 * DRB[5] == 2KB
	 * DRB[6] == 2KB
	 * DRB[7] == 2KB
	 */
#define SET_RPS \
	CS_WRITE_WORD(0x74, 0x0055)

	/* SDRAMC */
#define SET_SDRAMC \
	CS_WRITE_BYTE(0x76, CAS_NB)


	/* PGPOL - Paging Policy Register */
	/* 0xff07
	 * [15:12] f == 1111
	 * [11: 8] f == 1111
	 * [ 7: 4] 0 == 0000
	 * [ 3: 0] 7 == 0111
	 * row0 == 4banks
	 * row1 == 4banks
	 * row2 == 4banks
	 * row3 == 4banks
	 * row4 == 4banks
	 * row5 == 4banks
	 * row6 == 4banks
	 * row7 == 4banks
	 * Dram Idle Timer (DIT) == 32 clocks
	 */
#define SET_PGPOL \
	CS_WRITE_WORD(0x78, 0xff07)

	/* MBFS - Memory Buffer Frequencey Select Register */
	/* 0xffff7f					     
	 * [23:20] f == 1111				     
	 * [19:16] f == 1111				     
	 * [15:12] f == 1111				     
	 * [11: 8] f == 1111				     
	 * [ 7: 4] 7 == 0111				     
	 * [ 3: 0] f == 1111				     
	 * MAA[14:0], WEA#, SRASA#, SCASA# == 100Mhz Buffers Enabled
	 * MAB[14,13,10,12:11,9:0], WEB#, SRASB#, SCASB# == 100Mhz Buffers Enabled
	 * MD[63:0] Control 2 == 100 Mhz Buffer Enable
	 * MD[63:0] Control 1 == 100 Mhz B
	 * MECC[7:0] Control 2 == 100 Mhz B
	 *
	 */
#define SET_MBFS \
	CS_WRITE_BYTE(0xca, 0xff)	; \
	CS_WRITE_BYTE(0xcb, 0xff)	; \
	CS_WRITE_BYTE(0xcc, 0x7f)

	/* DWTC - DRAM Write Thermal Throttle Control */
#define SET_DWTC \
	CS_WRITE_BYTE(0xe0, 0xb4)	; \
	CS_WRITE_BYTE(0xe1, 0xbe)	; \
	CS_WRITE_BYTE(0xe2, 0xff)	; \
	CS_WRITE_BYTE(0xe3, 0xd7)	; \
	CS_WRITE_BYTE(0xe4, 0x97)	; \
	CS_WRITE_BYTE(0xe5, 0x3e)	; \
	CS_WRITE_BYTE(0xe6, 0x00)	; \
	CS_WRITE_BYTE(0xe7, 0x80)

	/* DRTC - DRAM Read Thermal Throttle Control */
#define SET_DRTC \
	CS_WRITE_BYTE(0xe8, 0x2c)	; \
	CS_WRITE_BYTE(0xe9, 0xd3)	; \
	CS_WRITE_BYTE(0xea, 0xf7)	; \
	CS_WRITE_BYTE(0xeb, 0xcf)	; \
	CS_WRITE_BYTE(0xec, 0x9d)	; \
	CS_WRITE_BYTE(0xed, 0x3e)	; \
	CS_WRITE_BYTE(0xee, 0x00)	; \
	CS_WRITE_BYTE(0xef, 0x00)


ram_set_registers:
	SET_NBXCFG
	SET_DRAMC
	SET_PAM
	SET_DRB
	SET_FDHC
	SET_MBSC
	SET_SMRAM
	SET_ESRAMC
	SET_RPS
	SET_SDRAMC
	SET_PGPOL
	SET_MBFS
	SET_DWTC
	SET_DRTC
	RET_LABEL(ram_set_registers)


#define DEVFN(device, function) (((device) << 3) + (function))
#define CONFIG_ADDR(bus,devfn,where) (((bus) << 16) | ((devfn) << 8) | (where))
#define PM_FUNCTION  CONFIG_ADDR(0, PIIX4_DEVFN+3, 0)

#if USE_SPD
	
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
	xor %eax,%eax
	movw $(SMBUS_IO_BASE + SMBHSTSTAT), %dx
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
	cmpb $SMBUS_MEM_DEVICE_3, %bl
	jbe 1b
	/* We couldn't find anything we must have no memory */
	jmp no_memory	
	
spd_set_dramc_out:
	testb $0x12, %al
	movl $8, %eax
	jz 1f
	movl $0x10, %eax
1:	movl %eax, %edx
	movl $0x57, %eax
	PCI_WRITE_CONFIG_BYTE
	RET_LABEL(spd_set_dramc)

#endif /* USE_SPD */

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

#if USE_SPD
spd_enable_refresh:
	/* Find the first dimm and assume the rest are the same */
	/* Load the smbus device and port int %ebx */
	movl $((12 << 8) | SMBUS_MEM_DEVICE_0), %ebx
1:	CALLSP(smbus_read_byte)
	jz 2f
	andl $0x7f, %eax
	jmp spd_enable_refresh_out

2:	addl $1, %ebx	/* increment the device */
	cmpb $SMBUS_MEM_DEVICE_3, %bl
	jbe 1b
	/* We couldn't find anything we must have no memory */
	jmp no_memory	

spd_enable_refresh_out:
	cmpb $0x06, %al		/* see if the ram refresh is a supported one */
	ja	1f
	addl $refresh_rates, %eax /* convert SPD refresh rates to 440GX refresh rates */
	movb (%eax), %cl
	jmp 2f
1:	movb $0x05, %cl	/* unknown ram refresh hard code it to something conservative */
2:	movl $0x57, %eax
	PCI_READ_CONFIG_BYTE
	andb $0xf8, %al
	orb %cl, %al
	movb %al, %dl
	movl $0x57, %eax
	PCI_WRITE_CONFIG_BYTE
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
	cmpb $SMBUS_MEM_DEVICE_3, %bl
	jbe 1b

	movl $0x7f, %eax
	PCI_WRITE_CONFIG_WORD
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
	cmpb $SMBUS_MEM_DEVICE_3, %bl
	jbe 1b

	shll $8, %esi
	orl $0x7, %esi /* 32 clocks idle time */
	movl %esi, %ecx
	movl $0x78, %eax
	PCI_WRITE_CONFIG_WORD
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
	cmpb $SMBUS_MEM_DEVICE_3, %bl
	jbe 1b

	movl %esi, %edx
	movl $0x53, %eax
	PCI_WRITE_CONFIG_BYTE
	RET_LABEL(spd_set_nbxcfg)


spd_set_sdramc:
	RET_LABEL(spd_set_sdramc)

#endif /* USE_SPD */

	

	/* PAM FDHC MBSC SMRAM ESRAMC MBFS DWTC DRTC */
ram_set_spd_registers:
#if USE_SPD
	CALL_LABEL(enable_smbus)
	CALL_LABEL(setup_smbus)
	CALL_LABEL(spd_set_drb)
	CALL_LABEL(spd_set_dramc)
	CALL_LABEL(spd_set_rps)
	CALL_LABEL(spd_set_sdramc)
	CALL_LABEL(spd_set_pgpol)
	CALL_LABEL(spd_set_nbxcfg)
#endif
	RET_LABEL(ram_set_spd_registers)

#if 1
#define HAVE_ECC_RAM_SIZE
get_ecc_ram_size_ebx:
	movl	$0x67, %eax		/* Read the RAM SIZE */
	PCI_READ_CONFIG_BYTE
	andl	$0x000000ff, %eax	/* Convert it to bytes */
	movl	%eax, %ebx
	shll	$23, %ebx
	RET_LABEL(get_ecc_ram_size_ebx)
#endif /* HAVE_ECC_RAM_SIZE */

	
/* things that are not used */
#define FIRST_NORMAL_REFERENCE()
#define SPECIAL_FINISHUP()
intel_440_out:
