/* 
   v1.0: 02/14/2001: Program procedure to make workable and stable register settings:
         (1) Initial registers: 0x68:0x00, 0x6a:0x00, 0x6b:0x00, 0x6c:0x00,
				0x64:0xe4, 0x65:0xe4, 0x66:0xe4, 0x50:0xfe,
				0xf8:0x22, 0xf9:0x42, 0xfb:0xb0, 0x6d:0x01,
				0x60:0xff, 0x58:0x40, 0x59:0x00, 0x5a:0x20,
				0x5b:0x40, 0x5c:0x60, 0x5d:0x80, 0x5e:0xa0,
				0x5f:0xc0, 0x56:0xe0, 0x57:0xff
	 (2) When SPD routine: Changed 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f
	 (3) After SPD, the special register setting: 
		(i) First to write: 0x56--0x57:(0x5f), 0x6a:65
		(ii) Second to write: 0x6b:0x01, 0x78:0x01, 0x58:0x80, 0x6d:0x21
  v1.1: 02/14/2001: To add:
	(1) Initial registers: 0x51:0xdf, 0x52:0xc8, 0x53:0x98, 0x64:0xe6, 0x65:0xe6, 0x66:0xe6
	(2) After SPD, the special register setting:
		(i) First to write: or 0x68:0x01-->0x68:0x41, or 0x69:0x00-->0x69:0x2c
		(i) Second to write: 0x6d:0x21-->0x6d:0x57i
  TO TEST: To change 0x64:0xe4 to 0x64:0x12, 0x65:0xe4 to 0x65:0xe6, 0x66:0xe4 to 0x66:0xe6
	Result: fail!! It will cause too many errors message. 
		SDRAM verify:
			00000000:0005ff80
			00000004:0005ff84
			00000008:00000000
			0000000c:00000004
			00000010:00000008
			00000014:0000000c
			00000018:00000010
			0000001c:00000014
			00000020:0005ff80
			00000024:0005ffa4
			.................
			.................
			Too many errors.
  v1.2: 02/14/2001: To modify:
	(1) Initial register: 0x6b:0x00--> 0x6b:0x2d
	(2) After SPD, the special register setting:
		(i) Second to modify: or 0x6b:0x2d-->0x6b:0x2f, or 0x6c:0x00-->0x08
  v1.3: 02/04/2001: To add:
	(1) Initial registers: 0x71:0x08, 0x75:0x80, 0x76:0x80, 0x79:0xf0, 0x7f:0x04
  v1.4: 02/04/2001: To modify:
	(1) After SPD, set registers:
		(1) Second to write and modify: 0x70:0xc0, or 0x71:08-->0x71:88, 0x72:ec, 0x73:0c, 0x74:0x0e,
			or 0x75:0x80-->0x75:0x81, or 0x76:0x80-->0x76:0xd6, or 0x79:0xf0-->0x79:0xf4, 0x7a:0x01
  v1.5: 02/15/2001: To modify and add:
	(1) Initial register: 0x81:0xc3
            To add ptp table(bus0, devfn08) and program: 0x1c:0xf0, 0x20:0xf0, 0x21:0xff, 0x24:0xf0, 0x25:0xff, 0x34:0x80 	
	(2) After SPD, set registers:
		(1) Second to write: 0x0d:0x08, 0xac:0x6e

*/
		
/* SPD ram init */
#define PM_DEVFN  CONFIG_ADDR(0, PM_DEVICE, 0)
#define DRAM_CONFIG_PORT 0x5a
#define REGISTERED_DRAM_REGISTER $0x69
#define LAST_SMBUS_MEM_DEVICE SMBUS_MEM_DEVICE_2

jmp raminitspd_end
/* table of settings for initial registers */
register_table: 
/* no memory clock enable -- overridden by SPD, we hope */
//	.byte 0x78, 0x1
/* safe initial values ... */
	.byte 0x68, 0x0
	.byte 0x6a, 0x0	/* disable refresh */
//	.byte 0x6b, 0x0
	.byte 0x6b, 0x2d
	.byte 0x6c, 0x0 /* disable ECC for start */
//	.byte 0x6d, 0x37 /* as per Cindy Lee, ... */
//	.byte 0x64, 0xe4 /* slowest ram setting. banks 0, 1 */
//	.byte 0x65, 0xe4 /* banks 2, 3*/
//	.byte 0x66, 0xe4 /* banks 4, 5 */
	.byte 0x64, 0xe6
	.byte 0x65, 0xe6
	.byte 0x66, 0xe6

	.byte 0x50, 0xfe
	.byte 0x51, 0xdf
	.byte 0x52, 0xc8 
	.byte 0x53, 0x98
	.byte 0xf8, 0x22
	.byte 0xf9, 0x42
	.byte 0xfb, 0xb0

/* we tried increasing the drive, but that did not help  or hurt. 
 * We will leave it at low drive for now, however. 
 */
#if 1
/* the standard BIOS goes for 0x5f here, which is very high drive. 
 * Try it out. RGM 1/26/1 */
/* LOW DRIVE */
//	.byte 0x6d, 0x5 /* 0x4 = 24 ma on ma[2:13],we#, 24ma on ras# */
	.byte 0x6d, 0x1
#else
 /* HIGH DRIVE */
	.byte 0x6d, 0x5f /* 0x4 = 24 ma on ma[2:13],we#, 24ma on ras# */
#endif
	.byte 0x60, 0xff /* sdram in ALL banks. It's all we do. */
/* set these to 0xee (128 MB VC SDRAM).  In our working code we set to 
 * 0x88, but VIA code recommends 0xee. Since we are SPD, in the end, 
 * this initial setting will be over-ridden by SPD probe values. 
 * leave at 88 for now -- deal with this mess later
 */	
//	.byte 0x58, 0x88
//	.byte 0x59, 0x88
	.byte 0x58, 0x40
	.byte 0x59, 0x00
/* size the banks at max, they will be resized later. */
	.byte 0x5a, 0x20
	.byte 0x5b, 0x40
	.byte 0x5c, 0x60
	.byte 0x5d, 0x80
	.byte 0x5e, 0xa0
	.byte 0x5f, 0xc0
	.byte 0x56, 0xe0
	.byte 0x57, 0xff

	.byte 0x71, 0x08
	.byte 0x75, 0x80
	.byte 0x76, 0x80
	.byte 0x79, 0xf0

	.byte 0x81, 0xc3
	.byte 0x0 /* end of table */

ptp_table:
	.byte 0x1c, 0xf0
	.byte 0x20, 0xf0
	.byte 0x21, 0xff
	.byte 0x24, 0xf0
	.byte 0x25, 0xff
	.byte 0x34, 0x80
	.byte 0x0

ram_set_registers:
	/* standard x86 loop on table until done code */
	/* assumes that: devfn is 0 (safe on anything we've seen) */
	/* which means addresses are a byte */
	/* address is first, then data */
	movl	$register_table, %esi
1: 
	/* zero out eax. This is a must right here. */
	xor	%eax, %eax
	movb	(%esi), %al
	testb 	%al, %al
//	jz	done_ram_set_registers
	jz 	ptp
	inc	%esi
	movb	(%esi), %dl
	PCI_WRITE_CONFIG_BYTE
	inc %esi
	jmp 1b

ptp:	movl	$ptp_table, %esi
2:
	/* zero out eax. This is a must right here. */
	xor	%eax, %eax
	movb	(%esi), %al
	testb 	%al, %al
	jz	done_ram_set_registers
	inc	%esi
	orl 	$0x0800, %eax 
	movb	(%esi), %dl
	PCI_WRITE_CONFIG_BYTE
	inc %esi
	jmp 2b
	// I forget why we need this, but we do

	mov $0xa55a5aa5, %eax
	mov %eax, 0
	mov %eax, 0x4000000
	intel_chip_post_macro(0x01)

done_ram_set_registers:	RET_LABEL(ram_set_registers)
ram_set_spd_registers: 
	CALL_LABEL(enable_smbus)
	intel_chip_post_macro(0x02)

	CALL_LABEL(setup_smbus)
	intel_chip_post_macro(0x03)

	CALL_LABEL(spd_set_drb)
	intel_chip_post_macro(0x04)

	CALL_LABEL(spd_set_dramc)
	intel_chip_post_macro(0x05)

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
 	intel_chip_post_macro(0x06) ; \
	xorl %eax, %eax	

#define DIMM_BASE(n) \
	intel_chip_post_macro(0x07) ; \
	movl $(0x5a + ((n) -1)), %eax	; \
	PCI_READ_CONFIG_BYTE		; \
	andl $0xFF, %eax		; \
	shll $23, %eax			; \

#define DIMM_READ \
	addl %ebx, %eax			; \
	movl (%eax), %edx		/* end it here -- see if that fixes */
#if 0
	xorl $0xdff8, %eax		; \
	movl (%eax), %edx
#endif

#define DIMM0_READ DIMM0_BASE   ; DIMM_READ
#define DIMM1_READ DIMM_BASE(1) ; DIMM_READ
#define DIMM2_READ DIMM_BASE(2) ; DIMM_READ
#define DIMM3_READ DIMM_BASE(3) ; DIMM_READ
#define DIMM4_READ DIMM_BASE(4) ; DIMM_READ
#define DIMM5_READ DIMM_BASE(5) ; DIMM_READ
#define DIMM6_READ DIMM_BASE(6) ; DIMM_READ
#define DIMM7_READ DIMM_BASE(7) ; DIMM_READ

#define DIMMS_READ_EBX_OFFSET \
	intel_chip_post_macro(0x08) ; \
	DIMM0_READ	; \
	intel_chip_post_macro(0x09) ; \
	DIMM1_READ	; \
	intel_chip_post_macro(0x0a) ; \
	DIMM2_READ	; \
	intel_chip_post_macro(0x0b) ; \
	DIMM3_READ	; \
	intel_chip_post_macro(0x0c) ; \
	DIMM4_READ	; \
	intel_chip_post_macro(0x0d) ; \
	DIMM5_READ	; \
	intel_chip_post_macro(0x0e) ; \
	DIMM6_READ	; \
	intel_chip_post_macro(0x0f) ; \
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
	intel_chip_post_macro(0x20) ; \
	movl $0x6c, %eax		; \
	PCI_READ_CONFIG_BYTE		; \
	andl $0x18, %eax		; \
	orl  $(command), %eax		; \
	movl %eax, %edx			; \
	movl $0x6c, %eax		; \
	PCI_WRITE_CONFIG_BYTE

// fix me later. Have to have ram in slot 0, and we only test cas3 or 2
#define COMPUTE_CAS_MODE \
	intel_chip_post_macro(0x21) ; \
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
	/* FIX ME -- set to cas3 no matter what for now */ ; \
	movl	$0x1d0, %eax; \
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
	intel_chip_post_macro(0x22) ; \
	movl $0x55aa55aa, %eax; \
	mov %eax, 0x0; \
	mov 0x0, %eax; 

// should also read 0x5f and write that to 0x56 and 0x57


#define SPECIAL_FINISHUP() \
	intel_chip_post_macro(0x23) ; \
	/* enable multi-page open */; \
	CS_READ($0x6B)        ; \
	movb %al, %dl         ; \
	orb $0x02, %dl        ; \
	movl $0x6b, %eax      ; \
	PCI_WRITE_CONFIG_BYTE ; \
	CS_READ($0x6c)	      ; \
	movb %al, %dl         ; \
	orb $0x08, %dl        ; \
	movl $0x6c, %eax      ; \
	PCI_WRITE_CONFIG_BYTE ; \
	CS_WRITE($0x78, $0x01) ; \
	CS_WRITE($0x58, $0x80) ; \
	CS_WRITE($0x6d, $0x57) ; \
	CS_WRITE($0x70, $0xc0) ; \
	CS_READ($0x71)	      ; \
	movb %al, %dl         ; \
	orb $0x80, %dl        ; \
	movl $0x71, %eax      ; \
	PCI_WRITE_CONFIG_BYTE ; \
	CS_WRITE($0x72, $0xec) ; \
	CS_WRITE($0x73, $0x0c) ; \
	CS_WRITE($0x74, $0x0e) ; \
	CS_READ($0x75)	      ; \
	movb %al, %dl         ; \
	orb $0x01, %dl        ; \
	movl $0x75, %eax      ; \
	PCI_WRITE_CONFIG_BYTE ; \
	CS_READ($0x76)	      ; \
	movb %al, %dl         ; \
	orb $0x52, %dl        ; \
	movl $0x76, %eax      ; \
	PCI_WRITE_CONFIG_BYTE ; \
	CS_READ($0x79)	      ; \
	movb %al, %dl         ; \
	orb $0x04, %dl        ; \
	movl $0x79, %eax      ; \
	PCI_WRITE_CONFIG_BYTE ; \
	CS_WRITE($0x7a, $0x01) ; \
	CS_WRITE($0x0d, $0x08) ; \
	CS_WRITE($0xac, $0x6e) 

//	CS_WRITE($0x6d, $0x21) ; \
//	CS_WRITE($0x84, $0xc0)
	
spd_enable_refresh: 
	intel_chip_post_macro(0x24) ; \
	// just set it for now. 
	CS_WRITE($0x6A, $0x65)
 
	CS_READ($0x5f)
	movb %al, %dl
	movl $0x56, %eax
	PCI_WRITE_CONFIG_BYTE 
	CS_READ($0x5f)
	movb %al, %dl
	movl $0x57, %eax
	PCI_WRITE_CONFIG_BYTE

	CS_READ($0x68)
	movb %al, %dl
	orb  $0x40, %dl
	movl $0x68, %eax
	PCI_WRITE_CONFIG_BYTE
	CS_READ($0x69)
	movb %al, %dl
	orb  $0x2c, %dl
	movl $0x69, %eax
	PCI_WRITE_CONFIG_BYTE
	RET_LABEL(spd_enable_refresh)
raminitspd_end:
