jmp intel_E7500_out

#define DEBUG_RAM_CONFIG 0


/* DDR DIMM Mode register Definitions */

#define BURST_2           (1<<0)
#define BURST_4           (2<<0)
#define BURST_8           (3<<0)

#define BURST_SEQUENTIAL  (0<<3)
#define BURST_INTERLEAVED (1<<3)

#define CAS_2_0	          (0x2<<4)
#define CAS_3_0           (0x3<<4)
#define CAS_1_5           (0x5<<4)
#define CAS_2_5           (0x6<<4)

#define MODE_NORM         (0 << 7)
#define MODE_DLL_RESET    (2 << 7)
#define MODE_TEST         (1 << 7)

#define BURST_LENGTH BURST_4
#define BURST_TYPE   BURST_INTERLEAVED
#define CAS_LATENCY  CAS_1_5

#define MRS_VALUE (MODE_NORM | CAS_LATENCY | BURST_TYPE | BURST_LENGTH)
#define EMRS_VALUE 0x000

#define MD_SHIFT 4

#define RAM_COMMAND_NONE	0x0
#define RAM_COMMAND_NOP		0x1
#define RAM_COMMAND_PRECHARGE	0x2
#define RAM_COMMAND_MRS		0x3
#define RAM_COMMAND_EMRS	0x4
#define RAM_COMMAND_CBR		0x6
#define RAM_COMMAND_NORMAL	0x7

#define RAM_CMD(command, offset) \
	movl	$(((offset) << (MD_SHIFT + 16))|((command << 4) & 0x70)), %ebx ; \
	CALLSP(do_ram_command)
	
#define RAM_NOP()		RAM_CMD(RAM_COMMAND_NOP, 0)
#define RAM_PRECHARGE()		RAM_CMD(RAM_COMMAND_PRECHARGE, 0)
#define RAM_CBR()		RAM_CMD(RAM_COMMAND_CBR, 0)
#define RAM_EMRS()		RAM_CMD(RAM_COMMAND_EMRS, EMRS_VALUE)

ram_cas_latency:
	.byte CAS_2_5, CAS_2_0, CAS_1_5, CAS_2_5
ram_mrs:
	/* Read the cas latency setting */
	movl	$0x78, %eax
	PCI_READ_CONFIG_BYTE
	/* Transform it into the form expected by SDRAM */
	shrl	$4, %eax
	andl	$3, %eax
	movb	ram_cas_latency(%eax), %al
	shll	$(16+MD_SHIFT), %eax
	orl	%eax, %ebx
	orl	$((MODE_NORM | BURST_TYPE | BURST_LENGTH) << (16+MD_SHIFT)), %ebx
	jmp	do_ram_command

#define RAM_MRS(dll_reset) \
	movl	$((dll_reset << (8+MD_SHIFT+ 16))|((RAM_COMMAND_MRS <<4)& 0x70)), %ebx ; \
	CALLSP(ram_mrs)

#define RAM_NORMAL() \
	movl	$0x7c, %eax		;\
	PCI_READ_CONFIG_BYTE		;\
	andl	$0x8F, %eax		;\
	orb	$(RAM_COMMAND_NORMAL << 4), %al	;\
	movl	%eax, %edx		;\
	movl	$0x7c, %eax		;\
	PCI_WRITE_CONFIG_BYTE

#define RAM_RESET_DDR_PTR() \
	movl	$0x88, %eax		;\
	PCI_READ_CONFIG_BYTE		;\
	orb	$(1 << 4), %al		;\
	movl	%eax, %edx		;\
	movl	$0x88, %eax		;\
	PCI_WRITE_CONFIG_BYTE		;\
	movl	$0x88, %eax		;\
	PCI_READ_CONFIG_BYTE		;\
	andb	$~(1 << 4), %al		;\
	movl	%eax, %edx		;\
	movl	$0x88, %eax		;\
	PCI_WRITE_CONFIG_BYTE

do_ram_command:
#if DEBUG_RAM_CONFIG >= 2
	movl	%esp, %esi
	movl	%ebx, %edi
	CONSOLE_DEBUG_TX_CHAR($'P')
	CONSOLE_DEBUG_TX_CHAR($':')
	CONSOLE_DEBUG_TX_HEX8(%bl)
	CONSOLE_DEBUG_TX_CHAR($'\r')
	CONSOLE_DEBUG_TX_CHAR($'\n')
	movl	%edi, %ebx
	movl	%esi, %esp
#endif
	/* %ecx - initial address to read from */
	/* Compute the offset */
	movl	%ebx, %ecx
	shrl	$16, %ecx

1:
	/* Set the ram command */
	movl	$0x7c, %eax
	PCI_READ_CONFIG_BYTE
	andl	$0x8F, %eax
	orb	%bl, %al
	movl	%eax, %edx
	movl	$0x7c, %eax
	PCI_WRITE_CONFIG_BYTE

	/* Assert the command to the memory */
#if DEBUG_RAM_CONFIG  >= 2
	movl	%esp, %esi
	movl	%ebx, %edi
	CONSOLE_DEBUG_TX_CHAR($'R')
	CONSOLE_DEBUG_TX_CHAR($':')
	CONSOLE_DEBUG_TX_HEX32(%ecx)
	CONSOLE_DEBUG_TX_CHAR($'\r')
	CONSOLE_DEBUG_TX_CHAR($'\n')
	movl	%edi, %ebx
	movl	%esi, %esp
#endif
	movl	(%ecx), %eax

	/* Go to the next base address */
	addl	$0x04000000, %ecx

	/* Increment the counter */
	incb	%bh

	/* See if we are done */
	cmpb	$8, %bh
	jb	1b

3:	/* The command has been sent to all dimms so get out */
	RETSP

#define ENABLE_REFRESH() \
	movl	$0x7c, %eax		;\
	PCI_READ_CONFIG_DWORD		;\
	orl	$(1 << 29), %eax	;\
	movl	%eax, %ecx		;\
	mov	$0x7c, %eax		;\
	PCI_WRITE_CONFIG_DWORD

	/*
	 * Table:	constant_register_values
	 */
	.p2align 3
constant_register_values:
	/* SVID - Subsystem Vendor Identification Register
	 * 0x2c - 0x2d
	 * [15:00] Subsytem Vendor ID (Indicates system board vendor)
	 */
	/* SID - Subsystem Identification Register
	 * 0x2e - 0x2f
	 * [15:00] Subsystem ID
	 */
	.long 0x2c, 0, (0x15d9 << 0) | (0x3480 << 16)

	/* Undocumented
	 * 0x80 - 0x80
	 * This register has something to do with CAS latencies,
	 * possibily this is the real chipset control.
	 * At 0x00 CAS latency 1.5 works.
	 * At 0x06 CAS latency 2.5 works.
	 * At 0x01 CAS latency 2.0 works.
	 */
#if CAS_LATENCY == CAS_2_5
	.long 0x80, 0xfffffe00, 0x06 /* Intel E7500 recommended */
#elif CAS_LATENCY == CAS_2_0
	.long 0x80, 0xfffffe00, 0x0d /* values for register 0x80 */
#elif CAS_LATENCY == CAS_1_5
	.long 0x80, 0xfffffe00, 0x05
#endif

	/* Enable periodic memory recalibration */
	.long 0x88, 0xffffff00, 0x80

	/* FDHC - Fixed DRAM Hole Control
	 * 0x58
	 * [7:7] Hole_Enable
	 *       0 == No memory Hole
	 *       1 == Memory Hole from 15MB to 16MB
	 * [6:0] Reserved
	 *
	 * PAM - Programmable Attribute Map
	 * 0x59 [1:0] Reserved
	 * 0x59 [5:4] 0xF0000 - 0xFFFFF
	 * 0x5A [1:0] 0xC0000 - 0xC3FFF
	 * 0x5A [5:4] 0xC4000 - 0xC7FFF
	 * 0x5B [1:0] 0xC8000 - 0xCBFFF
	 * 0x5B [5:4] 0xCC000 - 0xCFFFF
	 * 0x5C [1:0] 0xD0000 - 0xD3FFF
	 * 0x5C [5:4] 0xD4000 - 0xD7FFF
	 * 0x5D [1:0] 0xD8000 - 0xDBFFF
	 * 0x5D [5:4] 0xDC000 - 0xDFFFF
	 * 0x5E [1:0] 0xE0000 - 0xE3FFF
	 * 0x5E [5:4] 0xE4000 - 0xE7FFF
	 * 0x5F [1:0] 0xE8000 - 0xEBFFF
	 * 0x5F [5:4] 0xEC000 - 0xEFFFF
	 *       00 == DRAM Disabled (All Access go to memory mapped I/O space)
	 *       01 == Read Only (Reads to DRAM, Writes to memory mapped I/O space)
	 *       10 == Write Only (Writes to DRAM, Reads to memory mapped I/O space)
	 *       11 == Normal (All Access go to DRAM)
	 */
	.long 0x58, 0xcccccf7f, (0x00 << 0) | (0x30 << 8) | (0x33 << 16) | (0x33 << 24)
	.long 0x5C, 0xcccccccc, (0x33 << 0) | (0x33 << 8) | (0x33 << 16) | (0x33 << 24)

	/* DRB - DRAM Row Boundary Registers
	 * 0x60 - 0x6F
	 *     An array of 8 byte registers, which hold the ending
	 *     memory address assigned  to each pair of DIMMS, in 64MB 
	 *     granularity.   
	 */
	/* Conservatively say each row has 64MB of ram, we will fix this up later */
	.long 0x60, 0x00000000, (0x01 << 0) | (0x02 << 8) | (0x03 << 16) | (0x04 << 24)
	.long 0x64, 0x00000000, (0x05 << 0) | (0x06 << 8) | (0x07 << 16) | (0x08 << 24)
	.long 0x68, 0xffffffff, 0
	.long 0x6C, 0xffffffff, 0

	/* DRA - DRAM Row Attribute Register 
	 * 0x70 Row 0,1
	 * 0x71 Row 2,3
 	 * 0x72 Row 4,5
	 * 0x73 Row 6,7
	 * [7:7] Device width for Odd numbered rows
	 *       0 == 8 bits wide x8
	 *       1 == 4 bits wide x4
	 * [6:4] Row Attributes for Odd numbered rows
	 *       010 == 8KB
	 *       011 == 16KB
	 *       100 == 32KB
	 *       101 == 64KB
	 *       Others == Reserved
	 * [3:3] Device width for Even numbered rows
	 *       0 == 8 bits wide x8
	 *       1 == 4 bits wide x4
	 * [2:0] Row Attributes for Even numbered rows
	 *       010 == 8KB
	 *       011 == 16KB
	 *       100 == 32KB
	 *       101 == 64KB (This page size appears broken)
	 *       Others == Reserved
	 */
	.long 0x70, 0x00000000, \
		(((0<<3)|(0<<0))<< 0) | \
		(((0<<3)|(0<<0))<< 4) | \
		(((0<<3)|(0<<0))<< 8) | \
		(((0<<3)|(0<<0))<<12) | \
		(((0<<3)|(0<<0))<<16) | \
		(((0<<3)|(0<<0))<<20) | \
		(((0<<3)|(0<<0))<<24) | \
		(((0<<3)|(0<<0))<<28)
	.long 0x74, 0xffffffff, 0

	/* DRT - DRAM Time Register
	 * 0x78
	 * [31:30] Reserved
	 * [29:29] Back to Back Write-Read Turn Around
	 *         0 == 3 clocks between WR-RD commands
	 *         1 == 2 clocks between WR-RD commands
	 * [28:28] Back to Back Read-Write Turn Around
	 *         0 == 5 clocks between RD-WR commands
	 *         1 == 4 clocks between RD-WR commands
	 * [27:27] Back to Back Read Turn Around
	 *         0 == 4 clocks between RD commands
	 *         1 == 3 clocks between RD commands
	 * [26:24] Read Delay (tRD)
	 *         000 == 7 clocks
	 *         001 == 6 clocks
	 *         010 == 5 clocks
	 *         Others == Reserved
	 * [23:11] Reserved
	 * [10:09] Active to Precharge (tRAS)
	 *         00 == 7 clocks
	 *         01 == 6 clocks
	 *         10 == 5 clocks
	 *         11 == Reserved
	 * [08:06] Reserved
	 * [05:04] Cas Latency (tCL)
	 *         00 == 2.5 Clocks
	 *         01 == 2.0 Clocks
	 *         10 == 1.5 Clocks
	 *         11 == Reserved
	 * [03:03] Write Ras# to Cas# Delay (tRCD)
	 *         0 == 3 DRAM Clocks
	 *         1 == 2 DRAM Clocks
	 * [02:02] Reserved
 	 * [01:01] Read RAS# to CAS# Delay (tRCD)
	 *         0 == 3 DRAM Clocks
	 *         1 == 2 DRAM Clocks
	 * [00:00] DRAM RAS# to Precharge (tRP)
	 *         0 == 3 DRAM Clocks
	 *         1 == 2 DRAM Clocks
	 */
#define DRT_CAS_2_5 (0<<4)
#define DRT_CAS_2_0 (1<<4)
#define DRT_CAS_1_5 (2<<4)
#define DRT_CAS_MASK (3<<4)

#if CAS_LATENCY == CAS_2_5
#define DRT_CL DRT_CAS_2_5
#elif CAS_LATENCY == CAS_2_0
#define DRT_CL DRT_CAS_2_0
#elif CAS_LATENCY == CAS_1_5
#define DRT_CL DRT_CAS_1_5
#endif

	/* Most aggressive settings possible */
	.long 0x78, 0xc0fff8c0, (1<<29)|(1<<28)|(1<<27)|(2<<24)|(2<<9)|DRT_CL|(1<<3)|(3<<1)|(1<<0)

	/* FIXME why was I attempting to set a reserved bit? */
	/* 0x0100040f */

	/* DRC - DRAM Contoller Mode Register
	 * 0x7c
	 * [31:30] Reserved
	 * [29:29] Initialization Complete
	 *         0 == Not Complete
	 *         1 == Complete
	 * [28:22] Reserved
	 * [21:20] DRAM Data Integrity Mode
	 *         00 == Disabled, no ECC
	 *         01 == Reserved
	 *         10 == Error checking, using chip-kill, with correction
	 *         11 == Reserved
	 * [19:18] Reserved
	 *         Must equal 01
	 * [17:17] Reserved
	 * [16:16] Command Per Clock - Address/Control Assertion Rule (CPC)
	 *         0 == 2n Rule
	 *         1 == 1n rule
	 * [15:10] Reserved
	 * [09:08] Refresh mode select
	 *         00 == Refresh disabled
	 *         01 == Refresh interval 15.6 usec
	 *         10 == Refresh interval 7.8 usec
	 *         11 == Refresh interval 64 usec
	 * [07:07] Reserved
	 * [06:04] Mode Select (SMS)
	 *         000 == Self Refresh Mode
	 *         001 == NOP Command
	 *         010 == All Banks Precharge
	 *         011 == Mode Register Set
 	 *         100 == Extended Mode Register Set
	 *         101 == Reserved
	 *         110 == CBR Refresh
	 *         111 == Normal Operation
	 * [03:00] Reserved
	 */
	.long 0x7c, 0xffcefcff, (2 << 20)|(1 << 16)| (0 << 8)
	/* CLOCK_DIS - CK/CK# Disable Register
	 * 0x8C
	 * [7:4] Reserved
	 * [3:3] CK3
	 *       0 == Enable
	 *       1 == Disable
	 * [2:2] CK2
	 *       0 == Enable
	 *       1 == Disable
	 * [1:1] CK1
	 *       0 == Enable
	 *       1 == Disable
	 * [0:0] CK0
	 *       0 == Enable
	 *       1 == Disable
	 */
	.long 0x8C, 0xfffffff0, 0xf

	/* TOLM - Top of Low Memory Register
	 * 0xC4 - 0xC5
	 * [15:11] Top of low memory (TOLM)
	 *         The address below 4GB that should be treated as RAM,
	 *         on a 128MB granularity.
	 * [10:00] Reserved
	 */
	/* REMAPBASE - Remap Base Address Regsiter
	 * 0xC6 - 0xC7
	 * [15:10] Reserved
	 * [09:00] Remap Base Address [35:26] 64M aligned
	 *         Bits [25:0] are assumed to be 0.
	 */
	.long 0xc4, 0xfc0007ff, (0x2000 << 0) | (0x3ff << 16)
	/* REMAPLIMIT - Remap Limit Address Register
	 * 0xC8 - 0xC9
	 * [15:10] Reserved
	 * [09:00] Remap Limit Address [35:26] 64M aligned
	 * When remaplimit < remapbase this register is disabled.
	 */
	.long 0xc8, 0xfffffc00, 0
	/* DVNP - Device Not Present Register
	 * 0xE0 - 0xE1
	 * [15:05] Reserved
 	 * [04:04] Device 4 Function 1 Present
	 *         0 == Present
	 *         1 == Absent
 	 * [03:03] Device 3 Function 1 Present
	 *         0 == Present
	 *         1 == Absent
 	 * [02:02] Device 2 Function 1 Present
	 *         0 == Present
	 *         1 == Absent
 	 * [01:01] Reserved
 	 * [00:00] Device 0 Function 1 Present
	 *         0 == Present
	 *         1 == Absent
	 */
	.long 0xe0, 0xffffffe2, (1<<4)|(1<<3)|(1<<2)|(0<<0)

constant_register_values_end:

	/*
	 * Routine:	ram_set_registers
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	%eax, %ebx, %ecx, %edx, %esi, %eflags
	 * Effects:	Do basic ram setup that does not depend on serial
	 *		presence detect information.
	 *		This sets PCI configuration registers to known good
	 *		values based on the table: 
	 *			constant_register_values
	 *		Which are a triple of configuration regiser, mask, and value.
	 *		
	 */
	/* DDR RECOMP/SCOMP table */
	.p2align 3
ddr_rcomp_scomp_table:
	/* register, function 2, function 3 */
	.long 0x054, 0x000000000, 0x000000000
	.long 0x058, 0x076433221, 0x076433221
	.long 0x05c, 0x0fedca988, 0x0fedca988
	.long 0x060, 0x0ffffffff, 0x0ffffffff
	.long 0x064, 0x000000000, 0x000000000
	.long 0x068, 0x076433221, 0x076433221
	.long 0x06c, 0x0fedca988, 0x0fedca988
	.long 0x070, 0x0ffffffff, 0x0ffffffff
	.long 0x084, 0x000000000, 0x099999999
	.long 0x088, 0x076433221, 0x099999999
	.long 0x08c, 0x0fedca988, 0x099999999
	.long 0x090, 0x0ffffffff, 0x099999999
	.long 0x094, 0x000000000, 0x0aaaaaaaa
	.long 0x098, 0x076433221, 0x0aaaaaaaa
	.long 0x09c, 0x0fedca988, 0x0aaaaaaaa
	.long 0x0a0, 0x0ffffffff, 0x0aaaaaaaa
	.long 0x0b4, 0x000000000, 0x000000000
	.long 0x0b8, 0x076433221, 0x076433221
	.long 0x0bc, 0x0fedca988, 0x0fedca988
	.long 0x0c0, 0x0ffffffff, 0x0ffffffff
	.long 0x0c4, 0x000000000, 0x000000000
	.long 0x0c8, 0x076433221, 0x076433221
	.long 0x0cc, 0x0fedca988, 0x0fedca988
	.long 0x0d0, 0x0ffffffff, 0x0ffffffff
ddr_rcomp_scomp_table_end:

	.p2align 3
ram_set_registers:
	CALL_LABEL(ram_set_rcomp_regs)
	CALL_LABEL(ram_set_d0f0_regs)
	RET_LABEL(ram_set_registers)
ram_set_rcomp_regs:
	movl	$0x0e0, %eax	/*enable device 0 functions 2 & 3 */
	PCI_READ_CONFIG_BYTE
	andb	$~(1<<1), %al
	movb	%al, %dl	/* Put data in dl */
	movl	$0x0e0, %eax	/* write to offset e0 */
	PCI_WRITE_CONFIG_BYTE

	/* load the tables in device 0 functions 2 and 3 */
	movl	$ddr_rcomp_scomp_table, %ebx
1:	movl	0(%ebx), %eax	/* get the register address */
	orl	$0x200, %eax	/* set to function 2 */
	movl	4(%ebx), %ecx	/* get the data for function 2 */
	PCI_WRITE_CONFIG_DWORD
	movl	0(%ebx), %eax	/* get the register address */
	orl	$0x300, %eax	/* set to function 3 */
	movl	8(%ebx), %ecx	/* get the data for function 3 */
	PCI_WRITE_CONFIG_DWORD
	addl	$12, %ebx
	cmpl	$ddr_rcomp_scomp_table_end, %ebx
	jb	1b

	movl	$0x088, %eax	/* RCOMP evaluation phase */
	PCI_READ_CONFIG_BYTE
	andb	$~(1<<5), %al
	movb	%al, %dl	/* Put data in dl */
	movb	%al, %cl	/* save the byte */
	movl	$0x088, %eax	/* write to offset 88 */
	PCI_WRITE_CONFIG_BYTE
	orb	$(1<<5), %cl
	movb	%cl, %dl	/* Put data in dl */
	movl	$0x088, %eax	/* write to offset 88 */
	PCI_WRITE_CONFIG_BYTE
2:	movl	$0x088, %eax	/* poll for RCOMP evaluation complete */
	PCI_READ_CONFIG_BYTE
	andb	$(1<<5), %al
	jnz	2b
	movl	$0x088, %eax	/* RCOMP/SCOMP register update */
	PCI_READ_CONFIG_BYTE
	andb	$~(1<<6), %al
	movb	%al, %dl	/* Put data in dl */
	movb	%al, %cl	/* save the byte */
	movl	$0x088, %eax	/* write to offset 88 */
	PCI_WRITE_CONFIG_BYTE
	orb	$(1<<6), %cl
	movb	%cl, %dl	/* Put data in dl */
	movl	$0x088, %eax	/* write to offset 88 */
	PCI_WRITE_CONFIG_BYTE
3:	movl	$0x088,%eax	/* poll for RCOMP/SCOMP update complete */
	PCI_READ_CONFIG_BYTE
	andb	$(1<<6), %al
	jnz	3b

	/* place the command clock drive strength comp in static override */
	movl	$0x0380, %eax	/* read function 3 offset 80 */
	PCI_READ_CONFIG_DWORD
    /* not sure this next section is correct, documentation on device 0
	function 3, register 80 is lacking. */
	movl	%eax, %ecx

	andl	$0xff01ffff, %ecx
	shrl	$7, %eax
	andl	$0x00fe0000, %eax
	orl	%eax, %ecx
	movl	$0x0380, %eax
	PCI_WRITE_CONFIG_DWORD	/* Set pull up values */
	movl	%ecx, %eax
	andl	$0xffffff01, %ecx
	shrl	$7, %eax
	andl	$0x000000fe, %eax
	orl	%eax, %ecx
	movl	$0x0380, %eax
	PCI_WRITE_CONFIG_DWORD	/* Set pull down values */
	orl	$0x80008000, %ecx
	movl	$0x0380, %eax
	PCI_WRITE_CONFIG_DWORD	/* enable mode overide */

	movl	$0x0088, %eax	/* enable periodic compensaion */
	PCI_READ_CONFIG_BYTE
	orb	$(1<<7), %al
	mov	%al, %dl
	movl	$0x0088, %eax	
	PCI_WRITE_CONFIG_BYTE
	RET_LABEL(ram_set_rcomp_regs)

ram_set_d0f0_regs:
#if DEBUG_RAM_CONFIG
	CALLSP(dumpnorth)
#endif
	movl	$constant_register_values, %ebx
	jmp	ram_set_one_register_start
ram_set_one_register:
#if DEBUG_RAM_CONFIG
	movl	%ebx, %esi
	CONSOLE_DEBUG_TX_CHAR($'C')
	CONSOLE_DEBUG_TX_CHAR($':')
	movl	0(%esi), %eax
	CONSOLE_DEBUG_TX_HEX32(%eax)
	CONSOLE_DEBUG_TX_CHAR($':')
	movl	4(%esi), %eax
	CONSOLE_DEBUG_TX_HEX32(%eax)
	CONSOLE_DEBUG_TX_CHAR($':')
	movl	8(%esi), %eax
	CONSOLE_DEBUG_TX_HEX32(%eax)
	CONSOLE_DEBUG_TX_CHAR($'\r')
	CONSOLE_DEBUG_TX_CHAR($'\n')
	movl	%esi, %ebx
#endif /* DEBUG_RAM_CONFIG */
	movl	0(%ebx), %eax	/* Read the original value to preserve the reserved bits */
	PCI_READ_CONFIG_DWORD
	movl	4(%ebx), %edx	/* Reserved bits mask */
	andl	%edx, %eax	/* Preserve only the reserved bits */
	movl	8(%ebx), %ecx	/* Read the new values into %ecx */
	notl	%edx
	andl	%edx, %ecx	/* Keep only the unreserved bits */
	orl	%eax, %ecx	/* Put the two sets of bits together */
	movl	0(%ebx), %eax	/* Refetch the address to write */
	PCI_WRITE_CONFIG_DWORD
	addl	$12, %ebx
ram_set_one_register_start:
	cmpl	$constant_register_values_end, %ebx
	jb	ram_set_one_register
#if DEBUG_RAM_CONFIG
	CALLSP(dumpnorth)
#endif
	RET_LABEL(ram_set_registers)


	/*
	 * Routine:	sdram_spd_get_page_size
	 * Arguments:	%bl SMBUS_MEM_DEVICE
	 * Results:	
	 *		%edi log base 2 page size of DIMM side 1 in bits
	 *		%esi log base 2 page size of DIMM side 2 in bits
	 *
	 * Preserved:	%ebx (except %bh), %ebp
	 *
	 * Trashed:	%eax, %bh, %ecx, %edx, %esp, %eflags
	 * Used:	%eax, %ebx, %ecx, %edx, %esi, %edi, %esp, %eflags
	 *
	 * Effects:	Uses serial presence detect to set %edi & %esi
	 *		to the page size of a dimm.
	 * Notes:
	 *              %bl SMBUS_MEM_DEVICE
	 *              %edi holds the page size for the first side of the DIMM.
	 *		%esi holds the page size for the second side of the DIMM.
	 *		     memory size is represent as a power of 2.
	 *
	 *		This routine may be worth moving into generic code somewhere.
	 */
sdram_spd_get_page_size:
	xorl	%edi, %edi	
	xorl	%esi, %esi
	
	movb	$4, %bh /* columns */
	CALLSP(smbus_read_byte)
	jz	sdram_spd_get_page_size_out
	andl	$0xf, %eax
	addl	%eax, %edi
	
	/* Get the module data width and convert it to a power of two */
	movb	$7, %bh		/* (high byte) */
	CALLSP(smbus_read_byte)
	jz	sdram_spd_get_page_size_out
	andl	$0xff, %eax
	movl	%eax, %ecx
	shll	$8, %ecx

	movb	$6, %bh		/* (low byte) */
	CALLSP(smbus_read_byte)
	jz	sdram_spd_get_page_size_out
	andl	$0xff, %eax
	orl	%eax, %ecx
	bsrl	%ecx, %eax	/* compute cheap log base 2 */
	addl	%eax, %edi

	/* side two */
	movb	$5, %bh		/* number of physical banks */
	CALLSP(smbus_read_byte)
	jz	sdram_spd_get_page_size_out
	cmp	$1, %al
	jbe	sdram_spd_get_page_size_out

	/* Start with the symmetrical case */
	movl	%edi, %esi

	movb	$4, %bh		/* columns */
	CALLSP(smbus_read_byte)
	jz	sdram_spd_get_page_size_out
	testb	$0xf0, %al
	jz	sdram_spd_get_page_size_out
	movl	%eax, %ecx
	andl	$0xf, %ecx
	subl	%ecx, %esi	/* Subtract out columns on side 1 */
	shrl	$4, %eax
	andl	$0xf, %eax
	addl	%eax, %esi	/* Add in columns on side 2 */
sdram_spd_get_page_size_out:
	RET_LABEL(sdram_spd_get_page_size)


	/*
	 * Routine:	sdram_spd_get_width
	 * Arguments:	%bl SMBUS_MEM_DEVICE
	 * Results:	
	 *		%edi width of SDRAM chips on DIMM side 1 in bits
	 *		%esi width of SDRAM chips on DIMM side 2 in bits
	 *
	 * Preserved:	%ebx (except %bh), %ebp
	 *
	 * Trashed:	%eax, %bh, %ecx, %edx, %esp, %eflags
	 * Used:	%eax, %ebx, %ecx, %edx, %esi, %edi, %esp, %eflags
	 *
	 * Effects:	Uses serial presence detect to set %edi & %esi
	 *		to the width of a dimm.
	 * Notes:
	 *              %bl SMBUS_MEM_DEVICE
	 *              %edi holds the width for the first side of the DIMM.
	 *		%esi holds the width for the second side of the DIMM.
	 *		     memory size is represent as a power of 2.
	 *
	 *		This routine may be worth moving into generic code somewhere.
	 */
sdram_spd_get_width:
	xorl	%edi, %edi	
	xorl	%esi, %esi
	
	movb	$13, %bh /* sdram width */
	CALLSP(smbus_read_byte)
	jz	sdram_spd_get_width_out
	movl	%eax, %ecx
	andl	$0x7f, %eax
	addl	%eax, %edi
	
	/* side two */
	movb	$5, %bh		/* number of physical banks */
	CALLSP(smbus_read_byte)
	jz	sdram_spd_get_width_out
	cmp	$1, %al
	jbe	sdram_spd_get_width_out

	/* Start with the symmetrical case */
	movl	%edi, %esi
	
	/* See if I need to double the width */
	testl	$0x80, %ecx
	jz	sdram_spd_get_width_out

	/* Double the width on side 2 */
	addl	%esi, %esi
sdram_spd_get_width_out:
	RET_LABEL(sdram_spd_get_width)

	
	/*
	 * Routine:	sdram_spd_get_dimm_size
	 * Arguments:	%bl SMBUS_MEM_DEVICE
	 * Results:	
	 *		%edi log base 2 size of DIMM side 1 in bits
	 *		%esi log base 2 size of DIMM side 2 in bits
	 *
	 * Preserved:	%ebx (except %bh), %ebp
	 *
	 * Trashed:	%eax, %bh, %ecx, %edx, %esp, %eflags
	 * Used:	%eax, %ebx, %ecx, %edx, %esi, %edi, %esp, %eflags
	 *
	 * Effects:	Uses serial presence detect to set %edi & %esi
	 *		the size of a dimm.
	 * Notes:
	 *              %bl SMBUS_MEM_DEVICE
	 *              %edi holds the memory size for the first side of the DIMM.
	 *		%esi holds the memory size for the second side of the DIMM.
	 *		     memory size is represent as a power of 2.
	 *
	 *		This routine may be worth moving into generic code somewhere.
	 */

sdram_spd_get_dimm_size:
	xorl	%edi, %edi
	xorl	%esi, %esi

	/* Note it might be easier to use byte 31 here, it has the DIMM size as
	 * a multiple of 4MB.  The way we do it now we can size both
	 * sides of an assymetric dimm.
	 */
	movb	$3, %bh		/* rows */
	CALLSP(smbus_read_byte)
	jz	sdram_spd_get_dimm_size_out
	andl	$0xf, %eax
	addl	%eax, %edi

	movb	$4, %bh		/* columns */
	CALLSP(smbus_read_byte)
	jz	sdram_spd_get_dimm_size_out
	andl	$0xf, %eax
	addl	%eax, %edi

	movb	$17, %bh	/* banks */
	CALLSP(smbus_read_byte)
	jz	sdram_spd_get_dimm_size_out
	andl	$0xff, %eax
	bsrl	%eax, %ecx	/* compute cheap log base 2 */
	addl	%ecx, %edi

	/* Get the module data width and convert it to a power of two */
	movb	$7, %bh		/* (high byte) */
	CALLSP(smbus_read_byte)
	jz	sdram_spd_get_dimm_size_out
	andl	$0xff, %eax
	movl	%eax, %ecx
	shll	$8, %ecx

	movb	$6, %bh		/* (low byte) */
	CALLSP(smbus_read_byte)
	jz	sdram_spd_get_dimm_size_out
	andl	$0xff, %eax
	orl	%eax, %ecx
	bsrl	%ecx, %eax	/* compute cheap log base 2 */
	addl	%eax, %edi

	/* side two */
	movb	$5, %bh		/* number of physical banks */
	CALLSP(smbus_read_byte)
	jz	sdram_spd_get_dimm_size_out
	cmp	$1, %al
	jbe	sdram_spd_get_dimm_size_out

	/* Start with the symmetrical case */
	movl	%edi, %esi

	movb	$3, %bh		/* rows */
	CALLSP(smbus_read_byte)
	jz	sdram_spd_get_dimm_size_out
	testb	$0xf0, %al
	jz	sdram_spd_get_dimm_size_out
	movl	%eax, %ecx
	andl	$0xf, %ecx
	subl	%ecx, %esi	/* Subtract out rows on side 1 */
	shrl	$4, %eax
	andl	$0xf, %eax
	addl	%eax, %esi	/* Add in rows on side 2 */

	movb	$4, %bh		/* columns */
	CALLSP(smbus_read_byte)
	jz	sdram_spd_get_dimm_size_out
	movl	%eax, %ecx
	andl	$0xf, %ecx
	subl	%ecx, %esi	/* Subtract out columns on side 1 */
	shrl	$4, %eax
	andl	$0xf, %eax
	addl	%eax, %esi	/* Add in columns on side 2 */

sdram_spd_get_dimm_size_out:
	RET_LABEL(sdram_spd_get_dimm_size)



	/*
	 * This is a place holder fill this out
	 * Routine:	spd_set_row_attributes 
	 * Arguments:	%bl SMBUS_MEM_DEVICE
	 * Results:	
	 *		%edi log base 2 size of DIMM side 1 in bits
	 *		%esi log base 2 size of DIMM side 2 in bits
	 *
	 * Preserved:	%ebx (except %bh), %ebp
	 *
	 * Trashed:	%eax, %bh, %ecx, %edx, %esp, %eflags
	 * Used:	%eax, %ebx, %ecx, %edx, %esi, %edi, %esp, %eflags
	 *
	 * Effects:	Uses serial presence detect to set %edi & %esi
	 *		the size of a dimm.
	 * Notes:
	 *              %bl SMBUS_MEM_DEVICE
	 *              %edi holds the memory size for the first side of the DIMM.
	 *		%esi holds the memory size for the second side of the DIMM.
	 *		     memory size is represent as a power of 2.
	 *
	 *		This routine may be worth moving into generic code somewhere.
	 */
spd_set_row_attributes:
	movl	$(SMBUS_MEM_DEVICE_START), %ebx
	xorl	%ebp, %ebp
spd_get_row_attributes:
	CALL_LABEL(sdram_spd_get_page_size)

	/* Test to see if the dimm is present */
	testl	%edi, %edi
	jz	spd_get_row_attributes_dimm_width

	/* Test for a valid dimm width */
	cmpl	$15, %edi
	jb	unsupported_page_size
	cmpl	$18, %edi
	ja	unsupported_page_size

	 /* double because I have 2 channels */
	addl	$1, %edi

	/* Convert to the format needed for the DRA register */
	subl	$14, %edi

	/* Place in the %ebp the dra place holder */
	movb	%bl, %cl
	subb	$(SMBUS_MEM_DEVICE_START), %cl
	shlb	$3, %cl
	shll	%cl, %edi
	orl	%edi, %ebp

	/* Test to see if the second side is present */
	testl	%esi, %esi
	jz	spd_get_row_attributes_dimm_width

	/* Test for a valid dimm width */
	cmpl	$15, %esi
	jb	unsupported_page_size
	cmpl	$18, %esi
	ja	unsupported_page_size

	 /* double because I have 2 channels */
	addl	$1, %esi

	/* Convert to the format needed for the DRA register */
	subl	$14, %esi

	/* Place in the %ebp the dra place holder */
	movb	%bl, %cl
	subb	$(SMBUS_MEM_DEVICE_START), %cl
	shlb	$3, %cl
	addb	$4, %cl
	shll	%cl, %esi
	orl	%esi, %ebp

spd_get_row_attributes_dimm_width:
	/* Now add the SDRAM chip width to the DRA */
	CALL_LABEL(sdram_spd_get_width)
	testl	%edi, %edi
	jz	spd_get_row_attributes_next_dimm
	cmpl	$4, %edi
	jne	1f

	/* Enable an x4 device */
	movl	$0x08, %edi
	movb	%bl, %cl
	subb	$(SMBUS_MEM_DEVICE_START), %cl
	shlb	$3, %cl
	shll	%cl, %edi
	orl	%edi, %ebp
1:
	testl	%esi, %esi
	jz	spd_get_row_attributes_next_dimm
	cmpl	$4, %esi
	jne	spd_get_row_attributes_next_dimm

	/* Enable an x4 device */
	movl	$0x08, %esi
	movb	%bl, %cl
	subb	$(SMBUS_MEM_DEVICE_START), %cl
	shlb	$3, %cl
	addb	$4, %cl
	shll	%cl, %esi
	orl	%esi, %ebp
	
spd_get_row_attributes_next_dimm:
	/* go to the next DIMM */
	addb	$(SMBUS_MEM_DEVICE_INC), %bl /* increment the smbus device */
	cmpb	$SMBUS_MEM_DEVICE_END, %bl
	jbe	spd_get_row_attributes

	/* Write the new row attributes register */
	movl	$0x70, %eax
	movl	%ebp, %ecx
	PCI_WRITE_CONFIG_DWORD

	RET_LABEL(spd_set_row_attributes)

	/*
	 * Routine:	sdram_read_paired_byte
	 * Arguments:	%esp return address
	 *		%bl device on the smbus to read from
	 *              %bh address on the smbus to read
	 * Results:	
	 *		zf clear
	 *		byte read in %al
	 *	On Error:
	 *		zf set
	 *		%eax trashed
	 *
	 * Preserved:	%ebx, %esi, %edi
	 *
	 * Trashed:	%eax, %ecx, %edx, %ebp, %esp, %eflags
	 * Used:	%eax, %ebx, %ecx, %edx, %esp, %eflags
	 *
	 * Effects:	Reads two spd bytes from both ram channesl
	 *              and errors if they are not equal.
	 *		It then returns the equal result.
	 */
spd_read_paired_byte:
	movl	%esp, %ebp
	CALLSP(smbus_read_byte)
	setnz	%cl
	movb	%al, %ch
	addb	$(SMBUS_MEM_CHANNEL_OFF), %bl
	CALLSP(smbus_read_byte)
	movb	%ch, %ah
	setnz	%ch
	subb	$(SMBUS_MEM_CHANNEL_OFF), %bl

	/* See if dimms on both sides are equally present */	
	cmp	%cl, %ch
	jne	sdram_presence_mismatch

	/* Leave if I have no data */
	testb	%cl, %cl
	jz	spd_verify_byte_out

	/* Verify the data is identical */
	cmp	%ah, %al
	jne	sdram_value_mismatch

	/* Clear the zero flag */
	testb	%cl, %cl
spd_verify_byte_out:
	movl	%ebp, %esp
	RETSP

	/*
	 * Routine:	spd_verify_dimms
	 * Arguments:	none
	 * Results:	none
	 * Preserved:	none
	 * Trashed:	%eax, %ebx, %ecx, %edx, %ebp, %esi, %edi, %esp, %eflags
	 * Used:	%eax, %ebx, %ecx, %edx, %ebp, %esi, %edi, %esp, %eflags
	 *
	 * Effects:	
	 *		- Verify all interesting spd information
	 *		  matches for both dimm channels.
	 *		- Additional error checks that can be easily done
	 *		  here are computed as well, so I don't need to
	 *		  worry about them later.
	 */
spd_verify_dimms:
	movl	$(SMBUS_MEM_DEVICE_START), %ebx
spd_verify_dimm:
	/* Verify this is DDR SDRAM */
	movb	$2, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_verify_next_dimm
	cmpb	$7, %al
	jne	invalid_dimm_type

	/* Verify the row addresses */
	movb	$3, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data
	testb	$0x0f, %al
	jz	spd_invalid_data
	
	/* Column addresses */
	movb	$4, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data
	testb	$0xf, %al
	jz	spd_invalid_data

	/* Physical Banks */
	movb	$5, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data
	cmp	$1, %al
	jb	spd_invalid_data
	cmp	$2, %al
	ja	spd_invalid_data

	/* Module Data Width */
	movb	$7, %bh
	CALLSP(spd_read_paired_byte)	
	jz	spd_missing_data
	cmpb	$0, %al
	jne	spd_invalid_data

	movb	$6, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data
	cmpb	$64, %al
	je	1f
	cmpb	$72, %al
	je	1f
	jmp	spd_unsupported_data
1:

	/* Cycle time at highest CAS latency CL=X */
	movb	$9, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data

	/* SDRAM type */
	movb	$11, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data

	/* Refresh Interval */
	movb	$12, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data
	
	/* SDRAM Width */
	movb	$13, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data
	andb	$0x7f, %al
	cmpb	$4, %al
	je	1f
	cmpb	$8, %al
	je	1f
	jmp	spd_unsupported_data
1:

	/* Back-to-Back Random Column Accesses */
	movb	$15, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data
	testb	%al, %al
	jz	spd_invalid_data
	cmpb	$4, %al
	ja	spd_unsupported_data

	/* Burst Lengths */
	movb	$16, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data
	testb	$(1<<2), %al
	jz	spd_unsupported_data

	/* Logical Banks */
	movb	$17, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data
	testb	%al, %al
	jz	spd_invalid_data
	
	/* Supported CAS Latencies */
	movb	$18, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data
	testb	$(1 << 1), %al /* CL 1.5 */
	jnz	1f
	testb	$(1 << 2), %al /* CL 2.0 */
	jnz	1f
	testb	$(1 << 3), %al /* CL 2.5 */
	jnz	1f
	jmp	spd_unsupported_data
1:
	
	/* Cycle time at Cas Latency (CLX - 0.5) */
	movb	$23, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data
	
	/* Cycle time at Cas Latency (CLX - 1.0) */
	movb	$26, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data
	
	/* tRP Row precharge time */
	movb	$27, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data
	testb	$0xfc, %al
	jz	spd_invalid_data
	

	/* tRCD RAS to CAS */
	movb	$29, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data
	testb	$0xfc, %al
	jz	spd_invalid_data
	
	/* tRAS Activate to Precharge */
	movb	$30, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data
	testb	%al, %al
	jz	spd_invalid_data

	/* Module Bank Density */
	movb	$31, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data
	testb	$(1<<2), %al		/* 16MB */
	jnz	spd_unsupported_data
	testb	$(1<<3), %al
	jnz	spd_unsupported_data	/* 32MB */
	
	/* Address and Command Hold Time After Clock */
	movb	$33, %bh
	CALLSP(spd_read_paired_byte)
	jz	spd_missing_data

spd_verify_next_dimm:
	/* go to the next DIMM */
	addb	$(SMBUS_MEM_DEVICE_INC), %bl /* increment the smbus device */
	cmpb	$SMBUS_MEM_DEVICE_END, %bl
	jbe	spd_verify_dimm
spd_verify_dimms_out:
	RET_LABEL(spd_verify_dimms)

	.section ".rom.data"
spd_pre_init:	.string "Reading SPD data...\r\n"
spd_pre_set:	.string "setting based on SPD data...\r\n"
spd_post_init:	.string "done\r\n"
	.previous


refresh_rate_rank:
	/* Refresh rates ordered from most conservative (lowest)
	 * to most agressive (highest)
	 * disabled 0 -> rank 3 
	 * 15.6usec 1 -> rank 1
	 * 7.8 usec 2 -> rank 0
	 * 64usec   3 -> rank 2
	 */
	.byte 3, 1, 0, 2
refresh_rate_index:
	/* Map the spd refresh rates to memory controller settings 
	 * 15.625us -> 15.6us
 	 * 3.9us    -> err
	 * 7.8us    -> 7.8us
	 * 31.3s    -> 15.6us
	 * 62.5us   -> 15.6us
	 * 125us    -> 64us
	 */
	.byte 1, 0xff, 2, 1, 1, 3
#define MAX_SPD_REFRESH_RATE 5

spd_set_dram_controller_mode:
	/* Walk through all dimms and find the interesection of the support
	 * for ecc sdram and refresh rates
	 */
	movl	$(SMBUS_MEM_DEVICE_START), %ebx
	/* Read the inititial state */
	movl	$0x7c, %eax
	PCI_READ_CONFIG_DWORD
	movl	%eax, %esi

#if USE_OPTION_TABLE == 1
	/* Test if ECC cmos option is enabled */
        movb    $RTC_BOOT_BYTE, %al
        outb    %al, $0x70
        inb     $0x71, %al
        testb   $(1<<2), %al
	jnz	1f
	/* Clear the ecc enable */
	andl	$~(3 << 20), %esi
1:
#endif
spd_get_dram_controller_mode:

	/* Test to see if I have ecc sdram */
	movb	$11, %bh		/* SDRAM type */
	CALLSP(smbus_read_byte)
	jz	spd_set_dram_controller_mode_next_dimm
	cmpb	$2, %al
	je	spd_ecc
	/* Clear the ecc enable */
	andl	$~(3 << 20), %esi
spd_ecc:
	movb	$12, %bh		/* SDRAM refresh rate */
	CALLSP(smbus_read_byte)
	jz	spd_set_dram_controller_mode_next_dimm
	andl	$0x7f, %eax
	cmpb	$MAX_SPD_REFRESH_RATE, %al
	ja	unsupported_refresh_rate
	cmpb	$0xff, %al
	je	unsupported_refresh_rate
	xorl	%ecx, %ecx
	movb	refresh_rate_index(%eax, 1), %cl

	/* Isolate the old refresh rate setting */
	movl	%esi, %edx
	shrl	$8, %edx
	andl	$3, %edx

	/* Load the refresh rate ranks */
	movb	refresh_rate_rank(%edx), %dh
	movb	refresh_rate_rank(%ecx), %dl
	
	/* See if the new refresh rate is more conservative than the old
	 * refresh rate setting. (Lower ranks are more conservative)
	 */
	cmpb	%dh, %dl
	jae	1f
	/* Clear the old refresh rate */
	andl	$~(3<<8), %esi
	/* Move in the new refresh rate */
	shll	$8, %ecx
	orl	%ecx, %esi
1:
	
	movb	$33, %bh	/* Address and command hold time after clock */
	CALLSP(smbus_read_byte)
	jz	spd_set_dram_controller_mode_next_dimm
	cmpb	$0xa0, %al	/* At 133Mhz this constant should be 0x75 */
	jb	1f
	andl	$~(1<<16), %esi	/* Use two clock cyles instead of one */
1:	
	
spd_set_dram_controller_mode_next_dimm:
	/* go to the next DIMM */
	addb	$(SMBUS_MEM_DEVICE_INC), %bl /* increment the smbus device */
	cmpb	$SMBUS_MEM_DEVICE_END, %bl
	jbe	spd_get_dram_controller_mode

	/* Now write the controller mode */
	movl	$0x7c, %eax
	movl	%esi, %ecx
	PCI_WRITE_CONFIG_DWORD

	RET_LABEL(spd_set_dram_controller_mode)


spd_enable_clocks:
	/* Walk through all dimms and enable clocks for those that are present.
	 */
	movl	$(SMBUS_MEM_DEVICE_START), %ebx
	/* Read the inititial state */
	movl	$0x8c, %eax
	PCI_READ_CONFIG_DWORD
	movl	%eax, %esi
spd_get_clocks:
	/* Read any spd byte to see if the dimm is present */
	movb	$5, %bh		/* Physical Banks */
	CALLSP(smbus_read_byte)
	jz	spd_enable_clocks_next_dimm

	/* Find the index of the dimm */
	movb	%bl, %cl
	subb	$(SMBUS_MEM_DEVICE_START), %cl
	/* Now construct the mask to clear the enable bit */
	movl	$1, %eax
	shll	%cl, %eax
	notl	%eax
	andl	%eax, %esi
	
spd_enable_clocks_next_dimm:	
	/* go to the next DIMM */
	addb	$(SMBUS_MEM_DEVICE_INC), %bl /* increment the smbus device */
	cmpb	$SMBUS_MEM_DEVICE_END, %bl
	jbe	spd_get_clocks

	movl	$0x8c, %eax
	movl	%esi, %ecx
	PCI_WRITE_CONFIG_DWORD

	/* If we have no dimm modules bail out */
	andl	$0xf, %esi
	cmpl	$0xf, %esi
	je	no_memory
	RET_LABEL(spd_enable_clocks)

cas_latency_80:
	/* For cas latency 2.0 0x01 works and until I see a large test sample
	 * I am not prepared to change this value, to the intel recommended value
	 * of 0x0d.  Eric Biederman
 	 */
	.byte 0x05, 0x0d, 0x06 
cas_latency_78:
	.byte DRT_CAS_1_5, DRT_CAS_2_0, DRT_CAS_2_5

spd_set_cas_latency:
	/* Walk through all dimms and find the interesection of the
	 * supported cas latencies.
	 */
	movl	$(SMBUS_MEM_DEVICE_START), %ebx
	/* Initially allow cas latencies 2.5, 2.0, 1.5
	 * which the chipset supports.
	 */	
	movl	$((1 << 3)|(1 << 2)|(1 <<1)), %esi
spd_get_cas_latencies:
	movb	$18, %bh
	CALLSP(smbus_read_byte)
	jz	spd_set_cas_latency_next_dimm

	/* Find the highest supported cas latency */
	bsrl	%eax, %ecx
	movl	$1, %edi
	shll	%cl, %edi

	/* Remember the supported cas latencies */
	movl	%eax, %ecx
	
	/* Verify each cas latency at 100Mhz */
	/* Verify slowest/highest CAS latency */
	movb	$9, %bh
	CALLSP(smbus_read_byte)
	jz	spd_set_cas_latency_next_dimm

	cmpb	$0xa0, %al
	jbe	1f
	/* The bus is too fast so we cannot support this case latency */
	notl	%edi
	andl	%edi, %ecx
	notl	%edi
1:

	/* Verify the highest CAS latency - 0.5 clocks */
	shrl	$1, %edi
	testl	%edi,%edi
	jz	1f
	
	movb	$23, %bh
	CALLSP(smbus_read_byte)
	jz	spd_set_cas_latency_next_dimm

	cmpb	$0xa0, %al
	jbe	1f
	/* The bus is too fast so we cannot support this cas latency */
	notl	%edi
	andl	%edi, %ecx
	notl	%edi
1:	

	/* Verify the highest CAS latency - 1.0 clocks */
	shrl	$1, %edi
	testl	%edi, %edi
	jz	1f

	movb	$25, %bh
	CALLSP(smbus_read_byte)
	jz	spd_set_cas_latency_next_dimm

	movb	$0xa0, %al
	jbe	1f
	/* The bus is too fast so we cannot support this cas latency */
	notl	%edi
	andl	%edi, %ecx
	notl	%edi
1:	

	/* Now find which cas latencies are supported for the bus */
	andl	%ecx, %esi
spd_set_cas_latency_next_dimm:	
	/* go to the next DIMM */
	addb	$(SMBUS_MEM_DEVICE_INC), %bl /* increment the smbus device */
	cmpb	$SMBUS_MEM_DEVICE_END, %bl
	jbe	spd_get_cas_latencies

	/* After all of the arduous calculation setup with the fastest
	 * cas latency I can use.
	 */
	bsfl	%esi, %eax
	jz	inconsistent_cas_latencies
	subl	$1, %eax
	movl	%eax, %ecx

	movl	$0x78, %eax
	PCI_READ_CONFIG_BYTE
	andb	$~(DRT_CAS_MASK), %al
	orb	cas_latency_78(%ecx), %al
	movb	%al, %dl
	movl	$0x78, %eax
	PCI_WRITE_CONFIG_BYTE

	movl	$0x88, %eax	/* set master DLL reset */
	PCI_READ_CONFIG_DWORD
	orl	$(1<<26), %eax
	movl	%ecx, %ebx
	movl	%eax, %ecx
	movl	$0x88, %eax
	PCI_WRITE_CONFIG_DWORD
	andl	$0xfc00ffff, %ecx	/* set DQS delays to 2.2ns */
	orl	$0x02310000, %ecx
	movl	$0x88, %eax
	PCI_WRITE_CONFIG_DWORD
	movl	%ebx, %ecx
	movl	$0x80, %eax
	movb	cas_latency_80(%ecx), %dl
	PCI_WRITE_CONFIG_BYTE
	movl	$0x88, %eax	/* reset master DLL reset */
	PCI_READ_CONFIG_DWORD
	andl	$~(1<<26), %eax
	movl	%eax, %ecx
	movl	$0x88, %eax
	PCI_WRITE_CONFIG_DWORD
	
	RET_LABEL(spd_set_cas_latency)

spd_set_dram_timing:
	/* Walk through all dimms and find the interesection of the
	 * supported dram timings.
	 */
	movl	$(SMBUS_MEM_DEVICE_START), %ebx
	/* Read the inititial state */
	movl	$0x78, %eax
	PCI_READ_CONFIG_DWORD
	movl	%eax, %esi
spd_get_dram_timing:

	/* 
	 * tRP 
	 */
	movb	$27, %bh
	CALLSP(smbus_read_byte)
	jz	spd_set_dram_timing_next_dimm
	cmpb	$(20<<2), %al
	jbe	1f
	/* At 100Mhz if row precharge time is above than 20ns than we
	 * need 3 clocks not 2 clocks.
	 */ 
	andl	$~(1<<0), %esi
1:
	/* 
	 * tRCD 
	 */
	movb	$29, %bh
	CALLSP(smbus_read_byte)
	jz	spd_set_dram_timing_next_dimm
	cmpb	$(20<<2), %al
	jbe	1f
	/* At 100Mhz if the Minimum ras to cas delay is about 20ns we
	 * need 3 clocks not 2 clocks.
	 */
	andl	$~((1<<3)|(1<<1)), %esi
1:	

	/* 
	 * tRAS 
	 */
	movb	$30, %bh
	CALLSP(smbus_read_byte)
	jz	spd_set_dram_timing_next_dimm
	/* Convert tRAS from ns to 100Mhz clock cycles */
	movb	$10, %dl
	addb	%dl, %al	/* Make certain we round up */
	subb	$1, %al
	andl	$0xff, %eax	/* Clear the upper bits of eax */
	divb	%dl, %al

	/* Don't even process small timings */
	cmpb	$5, %al
	jbe	1f

	/* Die if the value is to large */
	cmpb	$7, %al
	ja	unsupported_rcd
	
	/* Convert to clocks - 5 */
	subb	$5, %al

	/* Convert the existing value into clocks - 5 */
	movl	%esi, %ecx
	shrl	$9, %ecx
	andl	$3, %ecx
	notl	%ecx
	subl	$1, %ecx

	/* See if we need a slower timing */
	cmpb	%al, %cl
	jbe	1f

	/* O.k. put in our slower timing */
	movb	%al, %cl
	addb	$1, %cl
	notl	%ecx
	shll	$9, %ecx
	andl	$(3<<9), %esi
	orl	%ecx, %esi
	
1:	
	/* 
	 * tRD 
	 */
	/* Set to a 6 clock read delay */
	andl	$~(7<<24), %esi
	orl	$(1<<24), %esi   

	/*
	 * Back to Back Read Turn Around
	 */
	/* Set to a 4 clock back to back read turn around */
	andl	$~(1<<27), %esi
	/*
	 * Back to Back Read-Write Turn Around
	 */
	/* Set to a 5 clock back to back read to write turn around */
	andl	$~(1<<28), %esi
	/*
	 * Back to Back Write-Read Turn Around
	 */
	/* Set to a 3 clock back to back write to read turn around */
	andl	$~(1<<29), %esi
			
spd_set_dram_timing_next_dimm:
	/* go to the next DIMM */
	addb	$(SMBUS_MEM_DEVICE_INC), %bl /* increment the smbus device */
	cmpb	$SMBUS_MEM_DEVICE_END, %bl
	jbe	spd_get_dram_timing

	movl	$0x78, %eax
	movl	%esi, %ecx
	PCI_WRITE_CONFIG_DWORD
	
	RET_LABEL(spd_set_dram_timing)


ram_set_spd_registers:
	CONSOLE_INFO_TX_STRING($spd_pre_init)
	CALL_LABEL(spd_enable_clocks)
	CALL_LABEL(spd_verify_dimms)
	CONSOLE_INFO_TX_STRING($spd_pre_set)
	CALL_LABEL(spd_set_row_attributes)
	CALL_LABEL(spd_set_dram_controller_mode)
	CALL_LABEL(spd_set_cas_latency)
	CALL_LABEL(spd_set_dram_timing)
	CONSOLE_INFO_TX_STRING($spd_post_init)
	RET_LABEL(ram_set_spd_registers)

/* LAST_DRB_SLOT is a constant for any E7500 board */
#define LAST_DRB_SLOT 0x67

spd_set_ram_size:
	movl	$(SMBUS_MEM_DEVICE_START), %ebx
	xorl	%ebp, %ebp
	/* Read the necessary SPD bytes for the first dimm */
spd_get_dimm_size:
	CALL_LABEL(sdram_spd_get_dimm_size)

	/* Since I have 2 identical channels double the sizes */
	incl	%edi
	incl	%esi

	/* Convert bits to multiples of 64MB */
	subl	$29, %edi
	jb	1f
	movl	%edi, %ecx
	movl	$1, %eax
	shll	%cl, %eax
	addl	%eax, %ebp
1:	
	/* Write the size of side 1 of the dimm */
	xorl	%eax, %eax
	movb	%bl, %al
	subl	$(SMBUS_MEM_DEVICE_START), %eax
	addl	%eax, %eax
	addl	$0x60, %eax
	movl	%ebp, %edx
	PCI_WRITE_CONFIG_BYTE

	/* Convert bits to multiples of 64MB */
	subl	$29, %esi
	jb	1f
	movl	%esi, %ecx
	movl	$1, %eax
	shll	%cl, %eax
	addl	%eax, %ebp
1:	
	/* Write the size of side 2 of the dimm */
	xorl	%eax, %eax
	movb	%bl, %al
	subl	$(SMBUS_MEM_DEVICE_START), %eax
	addl	%eax, %eax
	addl	$0x61, %eax
	movl	%ebp, %edx
	PCI_WRITE_CONFIG_BYTE

	/* go to the next DIMM */
	addb	$(SMBUS_MEM_DEVICE_INC), %bl /* increment the smbus device */
	cmpb	$SMBUS_MEM_DEVICE_END, %bl
	jbe	spd_get_dimm_size

	/* now, fill in DRBs where no physical slot exists */
	movl	$(SMBUS_MEM_DEVICE_END - SMBUS_MEM_DEVICE_START), %ecx
	addl	%ecx, %ecx
	addl	$0x61, %ecx
1:
	incl	%ecx
	cmpl	$LAST_DRB_SLOT, %ecx
	ja	spd_set_ram_size_computed
	movl	%ebp, %edx
	movl	%ecx, %eax
	PCI_WRITE_CONFIG_BYTE
	jmp	1b
	
spd_set_ram_size_computed:
	/* For now hardset everything at 128MB boundaries */
	/* %ebp has the ram size in multiples of 64MB */
	cmpl	$0, %ebp	/* test if there is no mem - smbus went bad */
	jz	no_memory_bad_smbus
	cmpl	$0x30, %ebp
	jae	spd_set_large_ram_size
spd_set_small_ram_size:
	/* I should really adjust all of this in C after I have resources
	 * to all of the pcie devices.
	 */

	/* Round up to 128M granularity */
	addl	$1, %ebp
	andl	$0xfe, %ebp
	shll	$10, %ebp
	movl	$0xC4, %eax
	movl	%ebp, %ecx
	PCI_WRITE_CONFIG_WORD
	jmp	spd_set_ram_size_out
spd_set_large_ram_size:
	/* FIXME will this work with 3.5G of ram? */
	/* Put TOLM at 3G */
	movl	$0xC4, %eax
	movl	$0xc000, %ecx
	PCI_WRITE_CONFIG_WORD
	/* Hard code a 1G remap window, right after the ram */
	cmp	$0x40, %ebp
	jae	1f
	movl	$0x40, %ebp  /* Ensure we are over 4G */
1:
	movl	$0xC6, %eax
	movl	%ebp, %ecx
	PCI_WRITE_CONFIG_WORD
	movl	$0xC8, %eax
	movl	%ebp, %edx
	addl	$0x10, %ecx
	PCI_WRITE_CONFIG_WORD
	jmp	spd_set_ram_size_out

spd_set_ram_size_out:
	RET_LABEL(spd_set_ram_size)

	/* I have finally seen ram bad enough to cause LinuxBIOS
	 * to die in mysterious ways, before booting up far
	 * enough to run a memory tester.  This code attempts
	 * to catch this blatantly bad ram, with a spot check.
	 * For most cases you should boot all of the way up 
	 * and run a memory tester.  
	 */
	/* Ensure I read/write each stick of bank of memory &&
	 * that I do more than 1000 bytes to avoid the northbridge cache.
	 * Only 64M of each side of each DIMM is currently mapped,
	 * so we can handle > 4GB of ram here.
	 */
bank_msg:	.asciz "Bank "
side_msg:	.asciz " Side "
verify_ram:
	xorl	%ecx, %ecx
	/* Check to see if the RAM is present,
	 * in the specified bank and side.
	 */
1:	movl	%ecx, %ebx
	shrl	$1, %ebx
	addl	$((5<<8) | SMBUS_MEM_DEVICE_START), %ebx
	CALLSP(smbus_read_byte)
	jz	5f
	testl	$1, %ecx
	jz	2f
	cmpb	$2, %al
	jne	5f

	/* Display the bank and side we are spot checking.
	 */
2:	CONSOLE_INFO_TX_STRING($bank_msg)
	movl	%ecx, %ebx
	shrl	$1, %ebx
	incl	%ebx
	CONSOLE_INFO_TX_HEX8(%bl)
	CONSOLE_INFO_TX_STRING($side_msg)
	movl	%ecx, %ebx
	andl	$1, %ebx
	CONSOLE_INFO_TX_HEX8(%bl)

	/* Compute the memory address to spot check. */
	movl	%ecx, %ebx
	xorl	%eax, %eax
3:	testl	%ebx, %ebx
	jz	4f
	addl	$0x04000000, %eax
	decl	%ebx
	jmp	3b
4:
	/* Spot check 512K of RAM */
	movl	%eax, %ebx
	addl	$0x0007ffff, %ebx
	CALLSP(spot_check)
5:	
	/* Now find the next bank and side to spot check */
	incl	%ecx
	cmpl	$((SMBUS_MEM_DEVICE_END - SMBUS_MEM_DEVICE_START)<<1), %ecx
	jb	1b
	RET_LABEL(verify_ram)
	

ram_postinit:
#if DEBUG_RAM_CONFIG 
	CALLSP(dumpnorth)
#endif
	/* Include a test to verify that memory is more or less working o.k. 
  	 * This test is to catch programming errors and hardware that is out of
	 * spec, not a test to see if the memory dimms are working 100%
	 */
	CALL_LABEL(verify_ram)
	CALL_LABEL(spd_set_ram_size)
	RET_LABEL(ram_postinit)


#define FIRST_NORMAL_REFERENCE() CALL_LABEL(ram_postinit)

#define SPECIAL_FINISHUP()   CALL_LABEL(dram_finish)

	.section ".rom.data"
ecc_pre_init:	.string "Initializing ECC state...\r\n"
ecc_post_init:	.string "ECC state initialized.\r\n"
	.previous
dram_finish:
	/* Test to see if ECC support is enabled */
	movl	$0x7c, %eax
	PCI_READ_CONFIG_DWORD
	shrl	$20, %eax
	andl	$3, %eax
	cmpb	$2, %al
	jne	noecc_init

	CONSOLE_INFO_TX_STRING($ecc_pre_init)
	/* Initialize ECC bits */
	movl	$0x52, %eax
	movl	$0x00, %edx
	PCI_WRITE_CONFIG_BYTE
	movl	$0x52, %eax
	movl	$0x01, %edx
	PCI_WRITE_CONFIG_BYTE
1:	movl	$0x52, %eax
	PCI_READ_CONFIG_BYTE
	andl	$0x08, %eax
	jz	1b
	andl	$0xfffffffe, %eax
	movl	%eax, %edx
	movl	$0x52, %eax
	PCI_WRITE_CONFIG_BYTE

	CONSOLE_INFO_TX_STRING($ecc_post_init)

	/* Clear the ECC error bits */
	movl    $0x0180, %eax  /* dev 0, function 1, offset 80 */
	movl    $0x03, %edx
	PCI_WRITE_CONFIG_BYTE
	movl    $0x0182, %eax  /* dev 0, function 1, offset 82 */
	movl    $0x03, %edx
	PCI_WRITE_CONFIG_BYTE

	movl    $0x0140, %eax  /* clear dev 0, function 1, offset 40 */
	movl    $(1<<18), %edx /* bit 18 by writing a 1 to it */
	PCI_WRITE_CONFIG_DWORD
	movl    $0x0144, %eax  /* clear dev 0, function 1, offset 44 */
	movl    $(1<<18), %edx /* bit 18 by writing a 1 to it */
	PCI_WRITE_CONFIG_DWORD

	movl	$0x52, %eax
	movl	$0x0d, %edx
	PCI_WRITE_CONFIG_BYTE

noecc_init:

#if DEBUG_RAM_CONFIG 
	CALLSP(dumpnorth)
#endif
	RET_LABEL(dram_finish)

#define ERRFUNC(x, str) \
	.section ".rom.data"			;\
x##_str:					;\
	.string str				;\
	.ascii str				;\
	.previous				;\
x:						;\
	movl	$x##_str, %esi			;\
	jmp mem_err


ERRFUNC(invalid_dimm_type,          "Invalid dimm type")
ERRFUNC(spd_missing_data,           "Missing sdram spd data")
ERRFUNC(spd_invalid_data,           "Invalid sdram spd data")
ERRFUNC(spd_unsupported_data,       "Unsupported sdram spd value")
ERRFUNC(unsupported_page_size,      "Unsupported page size")
ERRFUNC(sdram_presence_mismatch,    "DIMM presence mismatch")
ERRFUNC(sdram_value_mismatch,       "spd data does not match")
ERRFUNC(unsupported_refresh_rate,   "Unsuported spd refresh rate")
ERRFUNC(inconsistent_cas_latencies, "No cas latency supported by all dimms")
ERRFUNC(unsupported_rcd,            "Unsupported ras to cas delay")
#undef ERRFUNC

.section ".rom.data"
mem_err_err:	.string "ERROR: "
mem_err_pair:	.string " on dimm pair "
mem_err_byte:	.string " spd byte "
.previous
mem_err:
	movl	%ebx, %edi
	CONSOLE_ERR_TX_STRING($mem_err_err)
	CONSOLE_ERR_TX_STRING(%esi)
	CONSOLE_ERR_TX_STRING($mem_err_pair)
	movl	%edi, %ebx
	subb	$(SMBUS_MEM_DEVICE_START), %bl
	CONSOLE_ERR_TX_HEX8(%bl)
	CONSOLE_ERR_TX_STRING($mem_err_byte)
	movl	%edi, %ebx
	CONSOLE_ERR_TX_HEX8(%bh)
	jmp	mem_stop

intel_E7500_out:
