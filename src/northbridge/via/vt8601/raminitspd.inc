/* SPD ram init */
#define PM_DEVFN  CONFIG_ADDR(0, 0x3c, 0)
#define DRAM_CONFIG_PORT 0x5a
#define REGISTERED_DRAM_REGISTER $0x69
jmp raminitspd_end
/* table of settings for initial registers */
register_table: 
/* no memory clock enable -- overridden by SPD, we hope */
	.byte 0x78, 0x1
/* safe initial values ... */
	.byte 0x68, 0x0
	.byte 0x6a, 0x0	/* disable refresh */
	.byte 0x6b, 0x0
	.byte 0x6c, 0x0 /* disable ECC for start */
	.byte 0x6d, 0x37 /* as per Cindy Lee, ... */
	.byte 0x64, 0xe4 /* slowest ram setting. banks 0, 1 */
	.byte 0x65, 0xe4 /* banks 2, 3*/
	.byte 0x66, 0xe4 /* banks 4, 5 */
	.byte 0x6d, 0x5 /* 0x4 = 24 ma on ma[2:13],we#, 24ma on ras# */
	.byte 0x60, 0xff /* sdram in ALL banks. It's all we do. */
/* set these to 0xee (128 MB VC SDRAM).  In our working code we set to 
 * 0x88, but VIA code recommends 0xee. Since we are SPD, in the end, 
 * this initial setting will be over-ridden by SPD probe values. 
 */	
	.byte 0x58, 0xee
	.byte 0x59, 0xee
/* size the banks at max, they will be resized later. */
	.byte 0x5a, 0x20
	.byte 0x5b, 0x40
	.byte 0x5c, 0x60
	.byte 0x5d, 0x80
	.byte 0x5e, 0xa0
	.byte 0x5f, 0xc0
	.byte 0x56, 0xe0
	.byte 0x57, 0xff
	.byte 0x0 /* end of table */
ram_set_registers: 
	/* standard x86 loop on table until done code */
	/* assumes that: devfn is 0 (safe on anything we've seen) */
	/* which means addresses are a byte */
	/* address is first, then data */
	movl	$register_table, %esi
1: 
	movb	(%esi), %al
	testb 	%al, %al
	jz	done_ram_set_registers
	inc	%esi
	movb	(%esi), %dl
	PCI_WRITE_CONFIG_BYTE
	inc %esi
	jmp 1b

	// I forget why we need this, but we do

	mov $0xa55a5aa5, %eax
	mov %eax, 0
	mov %eax, 0x4000000

done_ram_set_registers:	RET_LABEL(ram_set_registers)
ram_set_spd_registers: 
	CALL_LABEL(enable_smbus)
	CALL_LABEL(setup_smbus)
	CALL_LABEL(spd_set_drb)
	CALL_LABEL(spd_set_dramc)
/*	CALL_LABEL(spd_set_rps)*/
/*	CALL_LABEL(spd_set_sdramc)*/
/*	CALL_LABEL(spd_set_pgpol)*/
/*	CALL_LABEL(spd_set_nbxcfg)*/
spd_set_rps_done:
spd_set_pgpol_done:
spd_set_nbxcfg_done:	

	RET_LABEL(ram_set_spd_registers)

#define SMBUS_IO_BASE 0xf00
#define REGISTERED_DRAM $0x2
#define NONREGISTERED_DRAM $0
#define REGISTER_DRAM_REGISTER $0x69

#define RAM_READ 0x0400

#define DIMM0_BASE \
	xorl %eax, %eax

#define DIMM_BASE(n) \
	movl $(0x5a + ((n) -1)), %eax	; \
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
	movl $0x6c, %eax		; \
	PCI_READ_CONFIG_BYTE		; \
	andl $0x18, %eax		; \
	orl  $(command), %eax		; \
	movl %eax, %edx			; \
	movl $0x6c, %eax		; \
	PCI_WRITE_CONFIG_BYTE

// fix me later. Have to have ram in slot 0, and we only test cas3 or 2
#define COMPUTE_CAS_MODE \
	movl $0x64, %eax	; \
	PCI_READ_CONFIG_BYTE	; \
	andl $0x20,  %eax	; \
	xorl $0x20,  %eax	; \
	shrl $1,    %eax	; \
	orl  $0x2a, %eax	; \

#define SET_RAM_MODE_REGISTER \
	SET_RAM_COMMAND(RAM_COMMAND_MRS)	; \
	COMPUTE_CAS_MODE	; \
	shll $3,    %eax	; \
	movl %eax,  %ebx	; \
	DIMMS_READ_EBX_OFFSET

#define ASSERT_RAM_COMMAND()		DIMMS_READ(RAM_READ)
#define ASSERT_MRS_RAM_COMMAND(mode)	DIMMS_READ(mode)
#define USE_SPD 1
#if ! USE_SPD
#define ENABLE_REFRESH()		CS_BIS_BYTE(0x57, 0x1)
#else /* USE_SPD */
#define ENABLE_REFRESH()		CALL_LABEL(spd_enable_refresh)
#endif

#define FIRST_NORMAL_REFERENCE() \
	movl $0x55aa55aa, %eax; \
	mov %eax, 0x0; \
	mov 0x0, %eax; 

// should also read 0x5f and write that to 0x56 and 0x57


#define SPECIAL_FINISHUP() \
	/* enable multi-page open */; \
	CS_WRITE($0x6B, $0x01)



spd_enable_refresh: 
	// just set it for now. 
	CS_WRITE($0x6A, $0x65)
	RET_LABEL(spd_enable_refresh)
raminitspd_end:
