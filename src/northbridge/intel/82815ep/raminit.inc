/*
	Intel 815EP MCH sdram initialization
	
	$Revision$
	$Author$
	$Date$
*/ 	
jmp intel_815_out

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

#define FIRST_NORMAL_REFERENCE()
#define SPECIAL_FINISHUP()

#define RAM_COMMAND_NORMAL	0x0		// self refresh 
#define RAM_COMMAND_NORMAL_0	0x1		// 15.6 us
#define RAM_COMMAND_NORMAL_1	0x2		// 7.8 us 
#define RAM_COMMAND_NORMAL_2	0x3		// 1.28
#define RAM_COMMAND_NOP		0x4
#define RAM_COMMAND_PRECHARGE	0x5
#define RAM_COMMAND_MRS		0x6
#define RAM_COMMAND_CBR		0x7
	
#define SET_RAM_COMMAND(command) \
	movl	$REG_DRAMT, %eax	; \
	PCI_READ_CONFIG_BYTE		; \
	andl	$0x1F, %eax		; \
	orl	$((command) << 5), %eax ; \
	movl	%eax, %edx		; \
	movl	$REG_DRAMT, %eax	; \
	PCI_WRITE_CONFIG_BYTE

#define ENABLE_REFRESH()

#define REG_PCICMD0	0x04
#define REG_PCICMD1	0x05 	
#define REG_MCHCFG	0x50
#define REG_DRP		0x52
#define REG_DRAMT	0x53
#define REG_DRP2	0x54
#define	REG_FDHC	0x58
#define REG_PAM0	0x59
#define REG_PAM1	0x5A
#define REG_PAM2	0x5B
#define REG_PAM3	0x5C
#define REG_PAM4	0x5D
#define REG_PAM5	0x5E
#define REG_PAM6	0x5F
#define REG_SMRAM	0x70
#define REG_BUFF_SC0	0x92
#define REG_BUFF_SC1	0x93	

/* default values for config registers */
ram_set_registers:
	CS_WRITE_BYTE(REG_PCICMD1, 0x00)	// disable SERR#

	CS_READ_BYTE(REG_MCHCFG) 
	TTYS0_TX_HEX32(%eax)
	// CS_WRITE_BYTE(REG_MCHCFG, 0x50)
	
	CS_WRITE_BYTE(REG_DRP, 0x00)
	CS_WRITE_BYTE(REG_DRP2, 0x00)
	CS_WRITE_BYTE(REG_DRAMT, 0x08)
	CS_WRITE_BYTE(REG_FDHC, 0x00)
	CS_WRITE_BYTE(REG_PAM0, 0x00)
	CS_WRITE_BYTE(REG_PAM1, 0x00)
	CS_WRITE_BYTE(REG_PAM2, 0x00)
	CS_WRITE_BYTE(REG_PAM3, 0x00)
	CS_WRITE_BYTE(REG_PAM4, 0x00)
	CS_WRITE_BYTE(REG_PAM5, 0x00)
	CS_WRITE_BYTE(REG_PAM6, 0x00)
	CS_WRITE_BYTE(REG_SMRAM, 0x00)
	CS_WRITE_BYTE(REG_BUFF_SC0, 0xff)
	CS_WRITE_BYTE(REG_BUFF_SC1, 0xff)	
	RET_LABEL(ram_set_registers)

ram_set_spd_registers:
	// TODO: SPD
	CS_WRITE_BYTE(REG_DRP, 0x0E)	// 256M, SS, DIMM0
	CS_WRITE_BYTE(REG_DRAMT, 0x00)  //
	
	RET_LABEL(ram_set_spd_registers)

ram_enable_1:	.string "\r\nRam Enable 1\r\n"
ram_enable_2:	.string "\r\nRam Enable 2\r\n"
ram_enable_3:	.string "\r\nRam Enable 3\r\n"
ram_enable_4:	.string "\r\nRam Enable 4\r\n"
ram_enable_5:	.string "\r\nRam Enable 5\r\n"
ram_fail:	.string "\r\n--- FAILED TO INITIALIZE \r\n"
ram_pass:	.string "\r\n--- SDRAM TEST PASSED \r\n" 

#define DO_READ(addr) \
	movl addr, %eax	; \
	movl (%eax), %ebx

#define DO_WRITE(addr, data) \
	movl addr, %eax		; \
	movl data, %ebx		; \
	movl %ebx, (%eax)
	
#define IODELAY		outb %al, $0x80		// took approximately 1us
mydelay:	
        mov $2000, %ecx
delay_loop:
	IODELAY
        decl %ecx
        jne delay_loop
	RETSP
		
enable_sdram:
	
	// Enterring NOP command enable mode  
	TTYS0_TX_STRING($ram_enable_1)
	SET_RAM_COMMAND(RAM_COMMAND_NOP)
	DO_READ($0x0)		// do read from sdram address 0 
	CALLSP(mydelay)		// wait a while (minimal 200us) 
	
		
	// Precharge all
	TTYS0_TX_STRING($ram_enable_2)	
	SET_RAM_COMMAND(RAM_COMMAND_PRECHARGE)	
	// DO_READ($0x0)	// do read from sdram address 0
	DO_READ($0x2000)

	// 8 auto refresh command / "CAS before RAS"
	TTYS0_TX_STRING($ram_enable_3)
	SET_RAM_COMMAND(RAM_COMMAND_CBR)

	mov	$8, %ecx 
1:	
	DO_READ($0x0)
	// IODELAY
	TTYS0_TX_CHAR($'.')
	dec	%ecx
	jne	1b
	
		
	// Mode register set 
	TTYS0_TX_STRING($ram_enable_4)
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
		
	movl (%eax), %ebx	// MRS comand. 
	// TTYS0_TX_HEX32(%eax)

	// Normal operation mode
	TTYS0_TX_STRING($ram_enable_5)
	SET_RAM_COMMAND(RAM_COMMAND_NORMAL_1)

	DO_READ($0x0)
	DO_WRITE($0x0, $0x0)

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
	TTYS0_TX_HEX32(%edi)

	movl 8(%esi), %edi
	TTYS0_TX_HEX32(%edi)

	movl 16(%esi), %edi
	TTYS0_TX_HEX32(%edi)
	
	cmpl $0x12345678, %edi
	je   pass
fail:
	TTYS0_TX_HEX32(%edi) 
	TTYS0_TX_STRING($ram_fail)
	jmp  doneDetection
pass:
	TTYS0_TX_HEX32(%edi) 	
	TTYS0_TX_STRING($ram_pass)
doneDetection:
#endif // 0 
		
	RET_LABEL(enable_sdram)	

intel_815_out:







