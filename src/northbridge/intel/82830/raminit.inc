/*
	Intel 82830 MCH sdram initialization
	
	$Revision$
	$Author$
	$Date$
*/ 	
jmp intel_830_out

#define CS_READ_BYTE(addr) \
	movl $addr, %eax	; \
	PCI_READ_CONFIG_BYTE
	
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

	// trashed : ax, bx, dx, sp 
#define NEWLINE	\
	CONSOLE_DEBUG_TX_CHAR($'\r')	;\
	CONSOLE_DEBUG_TX_CHAR($'\n')
	
#define PRINT_REG(reg) \
	CONSOLE_DEBUG_TX_HEX8($reg)	; \
	CONSOLE_DEBUG_TX_CHAR($':')	; \
	mov $reg, %eax		; \
	PCI_READ_CONFIG_BYTE	; \
	CONSOLE_DEBUG_TX_HEX8(%al)	; \
	NEWLINE

#define PRINT_REG_WORD(reg) \
	CONSOLE_DEBUG_TX_HEX8($reg)	; \
	CONSOLE_DEBUG_TX_CHAR($':')	; \
	mov $reg, %eax		; \
	PCI_READ_CONFIG_WORD	; \
	andl $0xffff, %eax	; \
	CONSOLE_DEBUG_TX_HEX32(%eax)	; \
	NEWLINE
	
#define FIRST_NORMAL_REFERENCE()
#define SPECIAL_FINISHUP()

#define RAM_COMMAND_NORMAL	(7<<4)		// self refresh 
#define RAM_COMMAND_NORMAL_0	((7<<4)|(0x1<<8))		// 15.6 us
#define RAM_COMMAND_NORMAL_1	((7<<4)|(0x2<<8))		// 7.8 us. 
#define RAM_COMMAND_NORMAL_2	((7<<4)|(7<<8))		// 1.28
#define RAM_COMMAND_NOP		(1<<4)
#define RAM_COMMAND_PRECHARGE	(2<<4)
#define RAM_COMMAND_MRS		(3<<4)
#define RAM_COMMAND_CBR		(6<<4)
	
#define SET_RAM_COMMAND(command) \
	movl	$REG_DRAMC, %eax	; \
	PCI_READ_CONFIG_DWORD		; \
	andl	$0xffffff8f, %eax		; \
	orl	$((command)), %eax ; \
	movl	%eax, %edx		; \
	movl	$REG_DRAMC, %eax	; \
	PCI_WRITE_CONFIG_DWORD

// current thinkpads set this to 15.6 uS.
#define ENABLE_REFRESH() \
	movl	$REG_DRAMC, %eax	; \
	PCI_READ_CONFIG_DWORD		; \
	andl	$0xfffff88f, %eax		; \
	orl	$((RAM_COMMAND_NORMAL_0)), %eax ; \
	movl	%eax, %edx		; \
	movl	$REG_DRAMC, %eax	; \
	PCI_WRITE_CONFIG_DWORD

#define REG_PCICMD0	0x04
#define REG_PCICMD1	0x05
	
#define REG_PCISTS0	0x06	
#define REG_PCISTS1	0x07
	
#define REG_SVID0	0x2c
#define REG_SVID1	0x2d

#define REG_SID0	0x2e
#define REG_SID1	0x2f

				
#define REG_RRBAR 	0x48	// IGNORED. LEAVE AT DEFAULT
#define REG_GCC0	0x50	// sixteen bits
#define REG_GCC0_VAL	0xa222
#define REG_GCC1	0x52	// 16 bits
#define REG_GCC1_VAL	0x800a
#define REG_FDHC	0x58 	// 8 bits
#define REG_FDHC_VAL	0

// thinkpad pam values. 10 11 11 00 30 11 11

#define REG_PAM0	0x59
#define REG_PAM1	0x5A
#define REG_PAM2	0x5B
#define REG_PAM3	0x5C
#define REG_PAM4	0x5D
#define REG_PAM5	0x5E
#define REG_PAM6	0x5F
#define REG_SMRAM	0x70

// on current thinkpad, one slot full, 0x2 0x4 0x4 0x4
#define REG_DRB0	0x60
#define REG_DRB1	0x61
#define REG_DRB2	0x62
#define REG_DRB3	0x63

#define REG_DRA		0x70	// 16 bits, 0xff11 on my one-ram thinkpad right now (4kb page on pop, top row EMPTY)
#define REG_DRA_VAL	0xff11
// default DRT 10 00 02 00  (0x00020010)
// Idle timer is 8 clocks, CL is 3
// Ras to cas is 3, ras precharge is 3
// i.e. as slow as it gets
#define REG_DRT		0x78	// 32 bits
#define REG_DRT_VAL	0x00020010

// on thinkpad it is 70 41 00 30 (30004170)
// rev 0, init complete, power mgmt on, all rows allowed active, 
// page close enable, refresh enabled at 15.6, normal operation

#define REG_DRAMC	0x7c
#define REG_DRAMC_VAL	0 // initially, none of these should be set initially.

#define REG_DTC		0x8c	// dram throttling
#define REG_DTC_VAL	0 	// thinkpad ignores it

// SMM sucks. Leave it off. 
#define REG_SMRAM	0x90
#define REG_SMRAM_VAL	0x2
// ignore this one. 
//#define REG_ESMRAMC	0x91
//#define REG_ESMRAMC_VAL	0x

// the buffer stuff is 1c 01 22 fc (0xfc22011c) on thinkpad

#define REG_BUFF_SC	0xec  // 32 bits
#define REG_BUFF_SC_VAL	0xfc22011c

/* default values for config registers */
ram_set_registers:
	CS_WRITE_WORD(REG_GCC0, REG_GCC0_VAL)
	CS_WRITE_WORD(REG_GCC1, REG_GCC1_VAL)
	CS_WRITE_BYTE(REG_FDHC, REG_FDHC_VAL)

	CS_WRITE_BYTE(REG_PAM0, 0x00)
	CS_WRITE_BYTE(REG_PAM1, 0x00)
	CS_WRITE_BYTE(REG_PAM2, 0x00)
	CS_WRITE_BYTE(REG_PAM3, 0x00)
	CS_WRITE_BYTE(REG_PAM4, 0x00)
	CS_WRITE_BYTE(REG_PAM5, 0x00)
	CS_WRITE_BYTE(REG_PAM6, 0x00)

	CS_WRITE_BYTE(REG_DRB0, 0x2)
	CS_WRITE_BYTE(REG_DRB1, 0x4)
	CS_WRITE_BYTE(REG_DRB2, 0x4)
	CS_WRITE_BYTE(REG_DRB3, 0x4)
	CS_WRITE_BYTE(REG_DRA, REG_DRA_VAL)
	CS_WRITE_BYTE(REG_DRT, REG_DRT_VAL)
	CS_WRITE_BYTE(REG_DTC, REG_DTC_VAL)
	CS_WRITE_BYTE(REG_DRAMC, REG_DRAMC_VAL)
	CS_WRITE_BYTE(REG_BUFF_SC, REG_BUFF_SC_VAL)

#if 0	
	CS_WRITE_BYTE(REG_SVID0, 0x86)
	CS_WRITE_BYTE(REG_SVID1, 0x80) // new 

	CS_WRITE_BYTE(REG_SID0, 0x30)
	CS_WRITE_BYTE(REG_SID1, 0x11) // new 
#endif
		
	CS_WRITE_BYTE(REG_PCICMD1, 0x00)	// disable SERR#
//	CS_WRITE_BYTE(REG_APCONT, 0x00)
	CS_WRITE_BYTE(REG_SMRAM, REG_SMRAM_VAL)

	RET_LABEL(ram_set_registers)

ram_set_spd_registers:
	// TODO: SPD
	// There is no explicit power-on on this chp ...
	// all the other non-spd setup is done at this point, so I guess this is a no-op.
	RET_LABEL(ram_set_spd_registers)

ram_enable_1:	.string "Ram Enable 1\r\n"
ram_enable_2:	.string "Ram Enable 2\r\n"
ram_enable_3:	.string "Ram Enable 3\r\n"
ram_enable_4:	.string "Ram Enable 4\r\n"
ram_enable_5:	.string "Ram Enable 5\r\n"

ram_fail:	.string "--- FAILED TO INITIALIZE \r\n"
ram_pass:	.string "--- SDRAM TEST PASSED \r\n" 

		
#define DO_READ(addr) \
	movl addr, %eax	; \
	movl (%eax), %ebx

#define DO_WRITE(addr, data) \
	movl addr, %eax		; \
	movl data, %ebx		; \
	movl %ebx, (%eax)
	
#define IODELAY		outb %al, $0x80		// took approximately 1us
#define MYDELAY(x) \
	mov x, %ecx		; \
1:	IODELAY			; \
	loop 1b 	

#define RAMREAD		DO_READ($0x0)
	
enable_sdram:
	
	// Enterring NOP command enable mode  
	CONSOLE_DEBUG_TX_STRING($ram_enable_1)
	SET_RAM_COMMAND(RAM_COMMAND_NOP)
	
	RAMREAD
	MYDELAY($400)		// wait a while (minimal 200us) 
		
	// Precharge all
	CONSOLE_DEBUG_TX_STRING($ram_enable_2)	
	SET_RAM_COMMAND(RAM_COMMAND_PRECHARGE)	
	RAMREAD

	// 8 auto refresh command "CAS before RAS"
	CONSOLE_DEBUG_TX_STRING($ram_enable_3)
	SET_RAM_COMMAND(RAM_COMMAND_CBR)

	RAMREAD
	MYDELAY($100)
	RAMREAD
	MYDELAY($100)	
	RAMREAD
	MYDELAY($100)	
	RAMREAD
	MYDELAY($100)
	RAMREAD
	MYDELAY($100)	
	RAMREAD
	MYDELAY($100)	
	RAMREAD
	MYDELAY($100)
	RAMREAD
	MYDELAY($100)	
			
	// Mode register set 
	CONSOLE_DEBUG_TX_STRING($ram_enable_4)
	SET_RAM_COMMAND(RAM_COMMAND_MRS)
	
	/* MAx[14:0] lines,
	 * MAx[2:0 ] 010 == burst mode of 4
	 * MAx[3:3 ] 1 == interleave wrap type
	 * MAx[4:4 ] == CAS# latency bit -> 0=2ns, 1=3ns 
	 * MAx[6:5 ] == 01
	 * MAx[12:7] ==	0
	 */
	// Get CAS latency
	// CAS# =2 clk -> 0x2a -> read 0x150
	// CAS# =3 clk -> 0x3a -> read 0x1D0
	// already set, leave this here as a comment.
	// everything runs slow on the thinkpad, it's all cas3
	//	movl $REG_DRAMT, %eax
	// PCI_READ_CONFIG_BYTE
	xorl	%eax, %eax	// puts all zeros into eax, meaning slow
	andl $0x4, %eax		// bit 2 ( 0 = 3clk, 1 = 2clk ) 
	xorl $0x4, %eax		// Inverting bit 2
	shll $2, %eax		// MA4 = inv(bit 2) 
	orl  $0x2a, %eax
	shll $3, %eax		
		
	movl (%eax), %ebx	// MRS comand to sdram

	// CONSOLE_DEBUG_TX_HEX32(%eax)

	// Normal operation mode
	CONSOLE_DEBUG_TX_STRING($ram_enable_5)
	ENABLE_REFRESH()			// also sets normal mode.

	DO_READ($0x0)
/* 		
	DO_WRITE($0x0, $0x0)
	DO_READ($0x0)

	CONSOLE_DEBUG_TX_HEX32(%eax)
	NEWLINE
	
	DO_WRITE($0xff, $0xff)
	DO_READ($0xff)

	CONSOLE_DEBUG_TX_HEX32(%eax)
	NEWLINE
*/
	
#if 0 
	// Now setup is done. we can write to SDRAM
	// test this.
	movl $0x0, %esi		// addr = 0 
	movl $0x0, %edi		
	movl $0x12345678, %edi	// data = 0x12345678

	// fill	
	movl %edi, (%esi)
	movl %edi, 8(%esi)
	movl %edi, 16(%esi)

	// verify.. 
	movl (%esi), %edi
	CONSOLE_DEBUG_TX_HEX32(%edi)

	movl 8(%esi), %edi
	CONSOLE_DEBUG_TX_HEX32(%edi)

	movl 16(%esi), %edi
	CONSOLE_DEBUG_TX_HEX32(%edi)
	
	cmpl $0x12345678, %edi
	je   pass
fail:
	CONSOLE_DEBUG_TX_HEX32(%edi) 
	CONSOLE_DEBUG_TX_STRING($ram_fail)
	jmp  doneDetection
pass:
	CONSOLE_DEBUG_TX_HEX32(%edi) 	
	CONSOLE_DEBUG_TX_STRING($ram_pass)
doneDetection:
#endif // 0 
		
	RET_LABEL(enable_sdram)	

intel_830_out:







