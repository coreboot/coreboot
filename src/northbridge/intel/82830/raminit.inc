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
	andl	$0xffffff80, %eax		; \
	orl	$((command)), %eax ; \
	movl	%eax, %edx		; \
	movl	$REG_DRAMC, %eax	; \
	PCI_WRITE_CONFIG_DWORD

#define ENABLE_REFRESH()

#define REG_PCICMD0	0x04
#define REG_PCICMD1	0x05
	
#define REG_PCISTS0	0x06	
#define REG_PCISTS1	0x07
	
#define REG_SVID0	0x2c
#define REG_SVID1	0x2d

#define REG_SID0	0x2e
#define REG_SID1	0x2f

				
// FROM HERE ON DOWN IT NEEDS TO BE FIXED -- RGM
#define REG_MCHCFG	0x50
#define MCHCFG_CLT  	0x40	// CLT (Cpu Latency Timer) 
#define MCHCFG_LMFS_MASK 0x10	// local memory frequency mask
#define MCHCFG_LMFS_133	0x10	// 133Mhz 
#define MCHCFG_LMFS_100	0x00	// 100Mhz 

#define MCHCFG_DPCP_MASK 0x08	// dram paging policy mask
#define MCHCFG_DPCP_0 	0x00	// precharge bank at page miss
#define MCHCFG_DPCP_1 	0x08	// precharge all 

#define MCHCFG_SMFS_MASK 0x04	// memory frequency mask
#define MCHCFG_SMFS_133	0x04	// 133Mhz 
#define MCHCFG_SMFS_100	0x00	// 100Mhz 

#define REG_APCONT	0x51 	
#define REG_DRP		0x52

#define REG_DRAMC	0x7c
#define REG_DRAMT	0x53
#define DRAMT_DCT	0x10 
#define DRAMT_HACQS	0x08 	// host aperture cycle queue slot 
#define DRAMT_CL_MASK	0x04	// CAS# latency mask
#define DRAMT_CL_2	0x04	// 2 SCLK 
#define DRAMT_CL_3	0x00	// 3 SCLK 
#define DRAMT_SRCD	0x02	// RAS# latency. 0 = 3 SCLK, 1 = 2 SCLK 
#define DRAMT_SRP	0x01	// RAS# precharge. 0 = 3 SCLK, 1 = 2 SCLK 

#define REG_DRP2	0x54

#define	REG_FDHC	0x58
#define FDHC_HEN	0x80	// 0 = no hole, 1 = 15-16MB hole enable. 

#define REG_PAM0	0x59
#define REG_PAM1	0x5A
#define REG_PAM2	0x5B
#define REG_PAM3	0x5C
#define REG_PAM4	0x5D
#define REG_PAM5	0x5E
#define REG_PAM6	0x5F
#define REG_SMRAM	0x70

#define REG_MISCC	0x72
#define MISCC_BYPASS	0x2000
#define MISCC_CPCME	0x0800
#define MISCC_WPTC_MASK 0x0030
#define MISCC_WPTC_100	0x0010
#define MISCC_WPTC_133	0x0020

#define REG_BUFF_SC	0x92
#define REG_BUFF_SC2	0x94	

#define REG_SM_RCOMP	0x98

#define REG_SM0		0x9c
#define REG_SM1		0x9d
#define REG_SM2		0x9e
#define REG_SM3		0x9f			
				
/* default values for config registers */
ram_set_registers:
	CS_WRITE_BYTE(REG_PCISTS1, 0x20) // new.
	CS_WRITE_BYTE(REG_SVID0, 0x86)
	CS_WRITE_BYTE(REG_SVID1, 0x80) // new 

	CS_WRITE_BYTE(REG_SID0, 0x30)
	CS_WRITE_BYTE(REG_SID1, 0x11) // new 
		
	CS_WRITE_BYTE(REG_PCICMD1, 0x00)	// disable SERR#
	CS_WRITE_BYTE(REG_APCONT, 0x00)
	CS_WRITE_BYTE(REG_DRP2, 0x00)
	CS_WRITE_BYTE(REG_FDHC, 0x00)
	CS_WRITE_BYTE(REG_PAM0, 0x00)
	CS_WRITE_BYTE(REG_PAM1, 0x00)
	CS_WRITE_BYTE(REG_PAM2, 0x00)
	CS_WRITE_BYTE(REG_PAM3, 0x00)
	CS_WRITE_BYTE(REG_PAM4, 0x00)
	CS_WRITE_BYTE(REG_PAM5, 0x00)
	CS_WRITE_BYTE(REG_PAM6, 0x00)
	CS_WRITE_BYTE(REG_SMRAM, 0x00)

	CS_WRITE_BYTE(REG_SM0, 0x46)	
	CS_WRITE_BYTE(REG_SM1, 0x83)	
	CS_WRITE_BYTE(REG_SM2, 0xc4)	
	CS_WRITE_BYTE(REG_SM3, 0x00)	
			
	RET_LABEL(ram_set_registers)

ram_set_spd_registers:
	// TODO: SPD

	CS_WRITE_BYTE(REG_MCHCFG, 0x44)

	CS_WRITE_BYTE(0x72, 0x28)
	CS_WRITE_BYTE(0x73, 0x20)
	
	// CS_WRITE_WORD(REG_MISCC, MISCC_BYPASS | MISCC_CPCME | MISCC_WPTC_133)
						// bypass for 133Mhz.
						// write power throttle
						// 400MB/sec - 133Mhz

	// drp
	CS_WRITE_BYTE(REG_DRP, 0x0E)		// 256M, SS, DIMM0


	// dramt 
	CS_WRITE_BYTE(REG_DRAMT, 0x18)


	// buff_sc 
	// CS_WRITE_WORD(REG_BUFF_SC, 0xffdf)	// rows 0/1 - 1.7x8 load
	CS_WRITE_BYTE(0x92, 0xae)
	CS_WRITE_BYTE(0x93, 0x3e)

	// CS_WRITE_WORD(REG_BUFF_SC2, 0xfffe)	// row  0   - 2.7x8 load 
	CS_WRITE_BYTE(0x94, 0xfe)
	CS_WRITE_BYTE(0x95, 0xff)

	// SM_RCOMP 
	CS_WRITE_BYTE(0x98, 0x43)
	CS_WRITE_BYTE(0x99, 0x80)
	CS_WRITE_BYTE(0x9a, 0x43)
	CS_WRITE_BYTE(0x9b, 0x80)

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
	movl $REG_DRAMT, %eax
	PCI_READ_CONFIG_BYTE
	andl $0x4, %eax		// bit 2 ( 0 = 3clk, 1 = 2ckl ) 
	xorl $0x4, %eax		// Inverting bit 2
	shll $2, %eax		// MA4 = inv(bit 2) 
	orl  $0x2a, %eax
	shll $3, %eax		
		
	movl (%eax), %ebx	// MRS comand to sdram
	// CONSOLE_DEBUG_TX_HEX32(%eax)

	// Normal operation mode
	CONSOLE_DEBUG_TX_STRING($ram_enable_5)
	SET_RAM_COMMAND(RAM_COMMAND_NORMAL_0)	// SPD: 12 (refresh rate)

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







