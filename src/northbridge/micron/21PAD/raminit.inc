/************************************************** 
 * FILE: src/northbridge/micron/21PAD/raminit.inc
 * NAME: Suravee Suthikulpanit
 * 5/2002
 *
 *This is the DDR RAM initialization for the 
 * RLX Blade.  
 *	- NorthBridge: Micron 21PAD 
 *	- SouthBridge: Acer M1535D (SMB is on M7101)
 * STEPS:
 *	1. Use SPD to determine the memory
 * 	2. Configure Memory Clock
 * 	3. Initialize DDR SDRAM
 * 	4. Adjust Read Data Strobes
 * 	5. Set timeing parameter
 * 	6. Memory Sizing
***************************************************/

jmp start_raminit

// This is for the dumpnorth sub-routine.
//#define NORTHBRIDGE_DEVFN 0xc000
#define DEBUG_MEM_INIT
//#define HARD_CODED
#define SMBUS_IO_BASE 0x5000
#define SMBHSTSTAT 0x0
#define SMBHSTTYPE 0x1
#define SMBHSTPORT 0x2
#define SMBHSTADDR 0x3
#define SMBHSTDAT0 0x4
#define SMBHSTDAT1 0x5
#define SMBHSTBLK  0x6
#define SMBHSTCMD  0x7

#define DEVFN(device, function) (((device) << 3) + (function))
#define CONFIG_ADDR(bus,devfn,where) (((bus) << 16) | ((devfn) << 8) | (where))

#define M7101_DEVFN (DEVFN(0x11,0))
#define M7101_REG(reg) (CONFIG_ADDR(0,M7101_DEVFN,reg))

#define _21PAD0_DEVFN (DEVFN(0x18,0))
#define _21PAD0_REG(reg) (CONFIG_ADDR(0,_21PAD0_DEVFN,reg))

#define _21PAD1_DEVFN (DEVFN(0x018,1))
#define _21PAD1_REG(reg) (CONFIG_ADDR(0,_21PAD1_DEVFN,reg))

#define loop200 $0x5000
#define loop100 $0x2500

#define PATTERN 0x55

Mem_Typing:			.string "Mem Typing\r\n"
Config_Mem_Clock:		.string "Config Mem Clock\r\n"
Init_DDR_SDRAM:			.string "Init DDR SDRAM\r\n"
Adjust_Read_Data_Strobes:	.string "Adjust Read Data Strobes\r\n"
Memory_Refresh:			.string "Memory Refresh\r\n"
Memory_Sizing:			.string "Memory Sizing\r\n"
RAM_not_found:			.string "RAM not found\r\n"
DEBUG1:				.string "DEBUG1\r\n"
DEBUG2:				.string "DEBUG2\r\n"
SMB_BASE:			.string "SMB Base address = "
SMB_REGISTER:			.string "SMB Register "
PRINT_SMB:			.string "Print SMB\r\n"
RAM_FOUND:			.string "RAM_FOUND\r\n"
DONE_CHECK_BUFFERED:		.string "DONE_CHECK_BUFFERED\r\n"
DONE_CHECK_WIDTH:		.string "DONE_CHECK_WIDTH\r\n"
DONE_SIDE1:			.string "DONE_SIDE1\r\n"
DONE_SIDE2:			.string "DONE_SIDE2\r\n"
DONE_FOR_NOW:			.string "DONE FOR NOW\r\n"
fail_write_read_verify:		.string "FAIL WRITE/READ/VERIFY\r\n"
pass_write_read_verify:		.string "PASS WRITE/READ/VERIFY\r\n"

/**************************************
 * Macro: 	CS_READ/WRITE_BYTE
 * Trashed: 	eax,edx
 * Result:	edx for read
 * Description:	
 *		- These macros acces the PCI device
 *              register.
 **************************************/ 

#define CS_WRITE_BYTE(addr, byte) \
        movl $addr, %eax        ; \
        movl $byte, %edx        ; \
        PCI_WRITE_CONFIG_BYTE

#define CS_READ_BYTE(addr) \
        movl $addr, %eax        ; \
        PCI_READ_CONFIG_BYTE

/***************************************
 * Macro:	SET_SMBUS
 * Trashed:	al, dx
 * Description: 
 * 		- This set the corresponding SMB IO Registers. 
***************************************/ 

#define SET_SMBUS(reg,value) \
	movw 	$SMBUS_IO_BASE, %dx	;\
	orb 	$reg, %dl		;\
	movb 	value, %al		;\
	outb 	%al, %dx
	
/***************************************
 * Macro:	GET_SMBUS
 * Trashed:	al,dx
 * result:	al
 * Description: 
 *		- This get the corresponding 
 * 		SMB IO Registers,and put it into %al. 
***************************************/
 
#define GET_SMBUS(reg)	\
	movw	$SMBUS_IO_BASE, %dx	;\
	orb 	$reg, %dl		;\
	xorb	%al, %al		;\
	inb	%dx, %al
		
#ifdef DEBUG_MEM_INIT

/***************************************
 * Sub-Routine:	PRINT_SMBUS_REG
 * Trashed:	eax,ecx,edx,esp
 * result:	none	
 * Description:	
 *		- This routine print all the register value
 *		of the SMBUS. This Sub-Routine is for debuging
 * 		only.  The code will stop at this routine.
 *		because it modifies the esp register.  
***************************************/
 
print_smbus_reg:
	CONSOLE_INFO_TX_STRING($PRINT_SMB)	
	xorl 	%ecx, %ecx
	xorl 	%eax, %eax
	xorl 	%edx, %edx
	//GET_SMBUS(SMBUS_IO_BASE)
	movw	$SMBUS_IO_BASE, %cx

	CONSOLE_INFO_TX_STRING($SMB_BASE)	
	CONSOLE_INFO_TX_HEX32(%ecx)
	CONSOLE_INFO_TX_CHAR($'\r')
	CONSOLE_INFO_TX_CHAR($'\n')
print_next:
	CONSOLE_INFO_TX_STRING($SMB_REGISTER)	
	CONSOLE_INFO_TX_HEX32(%ecx)
	CONSOLE_INFO_TX_CHAR($'-')	
	
	movw	%cx,  %dx
	inb	%dx,  %al
	
	CONSOLE_INFO_TX_HEX8(%al)	
	CONSOLE_INFO_TX_CHAR($'\r')
	CONSOLE_INFO_TX_CHAR($'\n')	
	 
	incb	%cl
	cmp	$0x08, %cl
	jne	print_next
	
	RETSP
#endif

/***************************************
 * Sub-Routine:	read_spd
 * arguements:	bl = the spd offset
 * results: 	al = the read value from SPD
 * Trashed:	eax,edx
 * Description: - This setup the read command and read the spd data
 *		from SMB
***************************************/ 

read_spd:
	// Reset statemachine of SMB
	//SET_SMBUS(SMBHSTTYPE, $0x4)
	
wait_busy_bus1:
	GET_SMBUS(SMBHSTSTAT)
	testb	$0x04, %al
	jz	wait_busy_bus1
 
	SET_SMBUS(SMBHSTSTAT, $0xFF)	// Reset status reg
	SET_SMBUS(SMBHSTADDR, $0xA1)	// Device Address for DRAM SPD/READ
	SET_SMBUS(SMBHSTTYPE, $0x20)	// Read/write byte type
	SET_SMBUS(SMBHSTCMD, %bl)	// Offset
	SET_SMBUS(SMBHSTPORT, $0xFF)	// Start Cycle
	
wait_busy_host:
	GET_SMBUS(SMBHSTSTAT)
	testb	$0x8, %al
	jnz	wait_busy_host
	
wait_busy_bus2:
	GET_SMBUS(SMBHSTSTAT)
	testb	$0x04, %al
	jz	wait_busy_bus2

check_complete:
	GET_SMBUS(SMBHSTSTAT)
	testb	$0x10, %al
	jz	read_spd	// Repeat the routine
	
complete:
	GET_SMBUS(SMBHSTDAT0)	// The read value should be in %al
	RETSP

/***************************************
 * Sub-Routine: center_strobe_in_data_eye	
 * results: al = 0xff -> fail verify
 *	    al = 0x00 -> pass veryfy  	
 * Trashed: eax, edx, ecx
 * Description: ecx= init value 1-127
 *		ebx= keep track the address for loop 
 *		
***************************************/ 
center_strobe_in_data_eye:
	xorl	%eax, %eax
	xorl	%ecx, %ecx
	xorl	%edx, %edx
start_init:
	// Since most of the blades usually set this 
	// value to 0x1d or 0x1e  
	//incw	%cx
	movb	$0x1e, %cl
	
	//Clear bit 7 of ( Fn1 Offset 0xb7) 
	//DQS Delay Line Enable Reg
	CS_READ_BYTE(_21PAD1_REG(0xb7))	
	andb	$0x7f, %dl
	movw 	$0xc1b7, %ax
	PCI_WRITE_CONFIG_BYTE
	
	// writing 1 to DQS Delay Line Adjust Reg 0-8
	xorl 	%eax, %eax
	xorl 	%ebx, %ebx
	xorl	%edx, %edx
	movw 	$0xc1d8, %bx
init_loop:
	movw	%bx, %ax
	movb 	%cl, %dl
	PCI_WRITE_CONFIG_BYTE
	incw	%bx
	cmpw	$0xc1df, %bx
	jle	init_loop

	//CS_WRITE_BYTE(_21PAD1_REG(0xe3),0x01)
	movw	$0xc1e3, %ax
	movb	%cl, %dl
	PCI_WRITE_CONFIG_BYTE
	
	// Set bit 7 of DQS Delay Line Enable Reg
	CS_READ_BYTE(_21PAD1_REG(0xb7))	
	orb	$0x80, %dl
	movw 	$0xc1b7, %ax
	PCI_WRITE_CONFIG_BYTE

	// PERFORM MEM WRITE/READ/VERIFY Test	
	// write pattern 0x55 to the start
	// of memory at 0000:0000 for 32 bytes.
	xorl 	%edx, %edx
write_pattern:
	movb	$PATTERN, (%edx)
	incb	%dl			
	cmpb	$0x20, %dl
	jle	write_pattern
	
	// read pattern
	xorl	%eax, %eax
	xorl	%edx, %edx
read_pattern:
	movb	(%edx),%al
	cmpb	$PATTERN, %al
	jne	fault_read
	incb	%dl
	cmpb	$0x20, %dl
	jle	read_pattern

pass_read:	
	// If pass, return 00
	movb	$0x00, %al
	RETSP
	
	// If fail verify, repeat with incremented
	// value	
fault_read:
	cmpw	$0x7f, %cx
	jl	start_init	
	// Return the value of %cl when fail	
	movb	%cl, %al
	RETSP

//============================================================================
/*****************************************	
* MAIN ROUTINE: start_raminit
******************************************/
start_raminit:
	
	// Clearing registers before detecting RAM
	// Fn 0 offset 74h-7bh	config addr = 0xc000 | offset
	// Fn 1 offset 80h-87h	config addr = 0xc100 | offset
	// Fn 1 offset F0h-FFh
	xorl %eax, %eax
	movw $0xc000, %ax
	orb  $0x73, %al
loop1:
	incb %al
	// CS_WRITE_BYTE(_21PAD0_REG( %al ),0)
	// We have to do this manualy
	movl $0x0000, %edx
	PCI_WRITE_CONFIG_BYTE
	testb $0x7b, %al
	jnz loop1

	xorl %eax, %eax
	movw $0xc100, %ax
	orb $0x7f, %al
loop2:	
	incb %al
	//CS_WRITE_BYTE(_21PAD1_REG( %al ),0)
	movl $0x0000, %edx
	PCI_WRITE_CONFIG_BYTE
	testb $0x87, %al
	jnz loop2
	
	xorl %eax, %eax
	movw $0xc100, %ax	
	orb $0xef, %al
loop3:	
	incb %al
	//CS_WRITE_BYTE(_21PAD1_REG( %al ),0)
	movl $0x0000, %edx
	PCI_WRITE_CONFIG_BYTE
	testb $0xff, %al
	jnz loop3
	
	//-----------------------------------------------------------
	/* MEMORY TYPING 
	* Using SPD to detect DDR RAM by reading 
	* byte 2 of the EEPROM. Other necessary
	* bytes are:
	* byte  3	Number of row
	* 	4	Number of columns
	*	5	Number of sides
	* 	13	Data width
	*	17	Module Banks per row
	*   21 bit 1	Buffered/Unbuffered
	*/
	CONSOLE_INFO_TX_STRING($Mem_Typing)

	// ??? This must be done to probe the flashrom
	// using smbus.c program.  So, I just tried putting
	// it here. 
	CS_WRITE_BYTE(M7101_REG(0xb7),0x4)

	xorb	%al, %al
	xorb 	%bl, %bl
	movb	$0x02, %bl
	CALLSP(read_spd)	// result is in %al
	cmp	$0x07, %al	
	jne	ram_not_found

	// DDR RAM is found
	CONSOLE_INFO_TX_STRING($RAM_FOUND)
set_rank_type:
	// side 1
	xorl	%ecx, %ecx
	orb	$0x1, %cl	// DIMM is present
	
check_width:
	movb	$13, %bl		
	CALLSP(read_spd)	// read SPD 13
	testb	$0x4, %al
	jnz	_4bit
	testb	$0x8, %al
	jnz 	_8bit
	testb	$0x16, %al
	orb	$0x08, %cl	// 16 bit wide
	jmp 	done_check_width
_4bit:
	//orb	$0x08, %ch	// 4 bit part present
	orb	$0x13, %ch	// try this instead
	jmp	done_check_width  // 4 bit wide
_8bit:
	orb	$0x04, %cl	// 8 bit wide
done_check_width:
	CONSOLE_INFO_TX_STRING($DONE_CHECK_WIDTH)
	
check_buffered:
	movb	$21, %bl
	CALLSP(read_spd)	// read SPD 21 bit 1
	testb	$0x02, %al
	jz	done_check_buffered
	
	orb	$0x2, %cl
done_check_buffered:
	CONSOLE_INFO_TX_STRING($DONE_CHECK_BUFFERED)
	
check_addressing_type:
#if HARD_CODED
	// HARD-CODED THIS PART!!!!!
	// for now to 0x20 = 256Mb
	orb	$0x20, %cl
#else
	// Figure out the addressing type using
	// 	- 2^ (NumofROws+NumofByte+Numofbankperrow)
	// %dl keeps track of # of bit to shift
	// NOT SURE!!!!
	xorl	%edx, %edx
	movb	$3, %bl
	CALLSP(read_spd)
	movb 	%al, %dl
	
	movb	$4, %bl
	CALLSP(read_spd)
	addb 	%al, %dl
	
	movb	$17, %bl
	CALLSP(read_spd)
	addb 	%al, %dl
	
	movl	$0x1, %eax
	shll	$16, %ecx
	movb	%dl, %cl
	shll	%cl, %eax
	shrl	$16, %ecx
	
	cmpl	$64, %eax
	je	done_check_addressing_type
	cmpl	$128, %eax
	je 	_128Mbit
	cmpl	$256, %eax
	je 	_256Mbit
	cmpl	$512, %eax
	
	orb	$0x30, %cl
	jmp	done_check_addressing_type
_128Mbit:
	orb	$0x10, %cl
	jmp	done_check_addressing_type
_256Mbit:
	orb	$0x20, %cl
#endif

done_check_addressing_type:

done_side1:
	//CS_WRITE_BYTE(_21PAD1_REG(0x80),%cl)
	//CS_WRITE_BYTE(_21PAD1_REG(0x44),%ch)
	// Manually do this
	xorl %eax, %eax
	xorl %edx, %edx
	movw $0xc180, %ax
	movb %cl, %dl
	PCI_WRITE_CONFIG_BYTE

	xorl %eax, %eax
	xorl %edx, %edx
	movw $0xc144, %ax
	movb %ch, %dl
	PCI_WRITE_CONFIG_BYTE
	CONSOLE_INFO_TX_STRING($DONE_SIDE1)
	
	movb	$0x05, %bl
	CALLSP(read_spd)
	testb	$0x02, %al
	jz	prog_mem_timing_control	

	// side 2
	//CS_WRITE_BYTE(_21PAD1_REG(0x81),%cl)
	//CS_WRITE_BYTE(_21PAD1_REG(0x44),%ch)
	// Manually do this
	xorl %eax, %eax
	xorl %edx, %edx
	movw $0xc181, %ax
	movb %cl, %dl
	PCI_WRITE_CONFIG_BYTE

	xorl %eax, %eax
	xorl %edx, %edx
	movw $0xc144, %ax
	movb %ch, %dl
	PCI_WRITE_CONFIG_BYTE
	CONSOLE_INFO_TX_STRING($DONE_SIDE2)

prog_mem_timing_control:
	// CAS Latency = 2.5, Buffered
	CS_WRITE_BYTE(_21PAD1_REG(0xc8),0x94)
	
	xorl %eax,%eax
	xorl %edx,%edx
	//CS_READ_BYTE(_21PAD1_REG(0xc9))
	//andb $0xf8, %dl
	//orb $0x01, %dl
	//movw $0xc1c9, %ax
	//PCI_WRITE_CONFIG_BYTE
	CS_WRITE_BYTE(_21PAD1_REG(0xc9),0x49)
	
	//-----------------------------------------------------------
	/* MEMORY CLOCK CONTROL
	 * Fn 1 offset 40h-43h 
	 * bit 0 = 1
	 * bit 17:16 = 10b
	 * bit 20 = 1
	*/
	
	CONSOLE_INFO_TX_STRING($Config_Mem_Clock)
	xorl %eax,%eax
	xorl %edx,%edx
	// writing 10b into bits 17:16 of Mem Interface
	// timing reg ( Fn1 offset 0x40-43)
	//CS_READ_BYTE(_21PAD1_REG(0x42))
	//andb $0xfc, %dl
	//orb $0x02, %dl
	//movw $0xc142, %ax
	//PCI_WRITE_CONFIG_BYTE
	CS_WRITE_BYTE(_21PAD1_REG(0x42),0x02)
	
	// Enable Mem clock by setting bit 0 of Mem Timing 
	// Interface Reg
	//CS_READ_BYTE(_21PAD1_REG(0x40))
	//orb $0x01, %dl
	//movw $0xc140, %ax
	//PCI_WRITE_CONFIG_BYTE
	CS_WRITE_BYTE(_21PAD1_REG(0x40),0x01)
 
	// wait 200 us
	DELAY(loop200)
	DELAY(loop200)

	// Enable Mem Clock Enable signals
	// Setting bit 20 of the Mem Timeing Interface Reg
	//CS_READ_BYTE(_21PAD1_REG(0x42))
	//orb $0x10, %dl
	//movw $0xc142, %ax
	//PCI_WRITE_CONFIG_BYTE
	CS_WRITE_BYTE(_21PAD1_REG(0x42),0x12)
	
	//-----------------------------------------------------------
	/* DDR SDRAM INITIALIZATION SEQUENCE
	 * Fn 1 offset 88h-8bh
	 */
	CONSOLE_INFO_TX_STRING($Init_DDR_SDRAM)
	// wait 200us
	DELAY(loop200)

	// After each write to DDR Mode and Init Reg,
	// Wait fo rthe Mem Request Active bit to clear

	// Issue Precharge All bank
	CS_WRITE_BYTE(_21PAD1_REG(0x8b),0x00)	
	CS_WRITE_BYTE(_21PAD1_REG(0x8a),0x00)	
	CS_WRITE_BYTE(_21PAD1_REG(0x89),0x00)	
	CS_WRITE_BYTE(_21PAD1_REG(0x88),0x1b)	
1:	
	CS_READ_BYTE(_21PAD1_REG(0x88))	// result is in %dl
	testb 	$0x8, %al
	jnz	1b		
	
	// Issue Load Extended Mode Register
	CS_WRITE_BYTE(_21PAD1_REG(0x8b),0x00)	
	CS_WRITE_BYTE(_21PAD1_REG(0x8a),0x00)	
	CS_WRITE_BYTE(_21PAD1_REG(0x89),0x00)	
	CS_WRITE_BYTE(_21PAD1_REG(0x88),0x19)	
2:	
	CS_READ_BYTE(_21PAD1_REG(0x88));	// result is in %dl
	testb 	$0x8, %al
	jnz	2b		
	
	// Issue Load Mode Register for CAS 2.5????
	CS_WRITE_BYTE(_21PAD1_REG(0x8b),0x16)	
	CS_WRITE_BYTE(_21PAD1_REG(0x8a),0xa0)	
	CS_WRITE_BYTE(_21PAD1_REG(0x89),0x00)	
	CS_WRITE_BYTE(_21PAD1_REG(0x88),0x1a)	

	// wait 200us
	DELAY(loop200)
3:	
	CS_READ_BYTE(_21PAD1_REG(0x88))	// result is in %dl
	testb 	$0x8, %al
	jnz	3b		
	
	// Issue Precharge All bank
	CS_WRITE_BYTE(_21PAD1_REG(0x8b),0x16)	
	CS_WRITE_BYTE(_21PAD1_REG(0x8a),0x00)	
	CS_WRITE_BYTE(_21PAD1_REG(0x89),0x00)	
	CS_WRITE_BYTE(_21PAD1_REG(0x88),0x1b)	
4:	
	CS_READ_BYTE(_21PAD1_REG(0x88))	// result is in %dl
	testb 	$0x8, %al
	jnz	4b		
	
	// Issue Two Auto Refresh TWICE!!!
	CS_WRITE_BYTE(_21PAD1_REG(0x8b),0x16)	
	CS_WRITE_BYTE(_21PAD1_REG(0x8a),0x00)	
	CS_WRITE_BYTE(_21PAD1_REG(0x89),0x00)	
	CS_WRITE_BYTE(_21PAD1_REG(0x88),0x1c)	

5:
	CS_READ_BYTE(_21PAD1_REG(0x88))	// result is in %dl
	testb 	$0x8, %al
	jnz	5b		
	
	CS_WRITE_BYTE(_21PAD1_REG(0x8b),0x16)	
	CS_WRITE_BYTE(_21PAD1_REG(0x8a),0x00)	
	CS_WRITE_BYTE(_21PAD1_REG(0x89),0x00)	
	CS_WRITE_BYTE(_21PAD1_REG(0x88),0x1c)	
6:	
	CS_READ_BYTE(_21PAD1_REG(0x88))	// result is in %dl
	testb 	$0x8, %al
	jnz	6b		
	
	// Clear Initial active bit 4
	CS_WRITE_BYTE(_21PAD1_REG(0x88),0x08)

7:	
	CS_READ_BYTE(_21PAD1_REG(0x88))	// result is in %dl
	testb 	$0x8, %al
	jnz	7b		
	
	//-----------------------------------------------------------
	/* READ DATA STROBE TIMING ADJUST
	 * There are couple things to do here to adjust the 
	 * data strobe signal
	 * 	- Temporary initializing address space
	 * 	- Centering the strobe in the data eye
   	 * 	- Mem write/read/verify test
	 * 	- Main Strobe adjust procedure
	 * 
	 * Fn 1 offset 0xb7		DQS Delay Line Enable
	 *	       0xd8-0xdf,0xe3  	DQS Delay Line Adjust 0-8
	 *	       0xe1		CAS to Read Preamble Delay
	 * 	       0xe2,0xe4-0xeb	Read preamble Fine Adjust		
	 * 	- NOTE: Some data are hardcoded in order to 
	 *	 shortcut the steps and save some space.
	 */
	CONSOLE_INFO_TX_STRING($Adjust_Read_Data_Strobes)

initializing_address_space:
	// initialize Rank Select Reg 0 to 0x4
	CS_WRITE_BYTE(_21PAD1_REG(0xf1),0x00)
	CS_WRITE_BYTE(_21PAD1_REG(0xf0),0x04)
	// initialize Rank Select Reg 1 to 0x8
	CS_WRITE_BYTE(_21PAD0_REG(0xf3),0x00)
	CS_WRITE_BYTE(_21PAD0_REG(0xf2),0x08)
	// Set mem controller's Local Lower to 0
	CS_WRITE_BYTE(_21PAD0_REG(0x75),0x00)
	CS_WRITE_BYTE(_21PAD0_REG(0x74),0x00)
	// Set mem controller's Local Upper to 0
	CS_WRITE_BYTE(_21PAD0_REG(0x77),0x00)
	CS_WRITE_BYTE(_21PAD0_REG(0x76),0x08)
	// Set mem controller's Global Upper-32 to 0
	CS_WRITE_BYTE(_21PAD0_REG(0x79),0x00)
	CS_WRITE_BYTE(_21PAD0_REG(0x78),0x08)
	// Set mem controller's Global Upper to 0
	CS_WRITE_BYTE(_21PAD0_REG(0x7b),0x00)
	CS_WRITE_BYTE(_21PAD0_REG(0x7a),0x08)
	
main_strobe_adjust:
	// CAS to Read Preamble Delay -> 2
	// Read Preamble Fine Adjust 0-8 ->dd
	CS_WRITE_BYTE(_21PAD1_REG(0xe1),0x02)
	CS_WRITE_BYTE(_21PAD1_REG(0xe2),0xdd)
	CS_WRITE_BYTE(_21PAD1_REG(0xe4),0xdd)
	CS_WRITE_BYTE(_21PAD1_REG(0xe5),0xdd)
	CS_WRITE_BYTE(_21PAD1_REG(0xe6),0xdd)
	CS_WRITE_BYTE(_21PAD1_REG(0xe7),0xdd)
	CS_WRITE_BYTE(_21PAD1_REG(0xe8),0xdd)
	CS_WRITE_BYTE(_21PAD1_REG(0xe9),0xdd)
	CS_WRITE_BYTE(_21PAD1_REG(0xea),0xdd)
	CS_WRITE_BYTE(_21PAD1_REG(0xeb),0xdd)
	
	//Center the strobes in the data eye.
	CALLSP(center_strobe_in_data_eye)
	//Verify
	cmpb	$0x0, %al
	jne	fail_verify
	CONSOLE_INFO_TX_STRING($pass_write_read_verify)

	//-----------------------------------------------------------
	/* MEMORY REFRESH
	 * Fn1 offset 0x8e = 0x10
	 *            0x8f = 0x04
	 *            0xe0 = 0x22
	 */
	CONSOLE_INFO_TX_STRING($Memory_Refresh)
	// Just try putting this right here.
	CS_WRITE_BYTE(_21PAD1_REG(0x8c),0x08)	// ECC Control
	CS_WRITE_BYTE(_21PAD1_REG(0x8d),0x89)	// Symmetric Select and Config
	CS_WRITE_BYTE(_21PAD1_REG(0x8f),0x04)
	CS_WRITE_BYTE(_21PAD1_REG(0x8e),0x10)
	CS_WRITE_BYTE(_21PAD1_REG(0xe0),0x22)
	
	//-----------------------------------------------------------
	/* MEMORY SIZING
	 * Program
	 * Fn 1 offset  0xf0-0xf7 Rank Select Addressing Reg
	 * 		0x74-0x75 Local Lower Memory Range
	 * 		0x76-0x77 Local Upper Memory Range
	 * 		0x78-0x79 Global Upper Memory-32 Range
	 * 		0x7a-0x7b Global Upper Memory Range
	 * - This part is HARD CODED to 1GB for now
	 */
	CONSOLE_INFO_TX_STRING($Memory_Sizing)
#if HARD_CODED	
	// Program the Local Lower Mem range to 0
	CS_WRITE_BYTE(_21PAD0_REG(0x75),0x0)
	CS_WRITE_BYTE(_21PAD0_REG(0x74),0x0)
	// Program the Local Upper Mem range to 64=0x40
	CS_WRITE_BYTE(_21PAD0_REG(0x77),0x0)
	CS_WRITE_BYTE(_21PAD0_REG(0x76),0x40)
	// Program the Global-32 Upper Mem range to 64=0x40
	CS_WRITE_BYTE(_21PAD0_REG(0x79),0x0)
	CS_WRITE_BYTE(_21PAD0_REG(0x78),0x40)
	// Program the Global Upper Mem range to 64=0x40
	CS_WRITE_BYTE(_21PAD0_REG(0x7b),0x0)
	CS_WRITE_BYTE(_21PAD0_REG(0x7a),0x40)

	// Program Rank Select 0 Addressing Reg to 32=0x20
	CS_WRITE_BYTE(_21PAD1_REG(0xf1),0x0)
	CS_WRITE_BYTE(_21PAD1_REG(0xf0),0x20)
	// Program Rank Select 1 Addressing Reg to 64=0x40
	CS_WRITE_BYTE(_21PAD1_REG(0xf3),0x0)
	CS_WRITE_BYTE(_21PAD1_REG(0xf2),0x40)
	// Program Rank Select 2 Addressing Reg to 64=0x40
	CS_WRITE_BYTE(_21PAD1_REG(0xf5),0x0)
	CS_WRITE_BYTE(_21PAD1_REG(0xf4),0x40)
	// Program Rank Select 3 Addressing Reg to 64=0x40
	CS_WRITE_BYTE(_21PAD1_REG(0xf7),0x0)
	CS_WRITE_BYTE(_21PAD1_REG(0xf6),0x40)
	// Program Rank Select 4 Addressing Reg to 64=0x40
	CS_WRITE_BYTE(_21PAD1_REG(0xf9),0x0)
	CS_WRITE_BYTE(_21PAD1_REG(0xf8),0x40)
	// Program Rank Select 5 Addressing Reg to 64=0x40
	CS_WRITE_BYTE(_21PAD1_REG(0xfb),0x0)
	CS_WRITE_BYTE(_21PAD1_REG(0xfa),0x40)
	// Program Rank Select 6 Addressing Reg to 64=0x40
	CS_WRITE_BYTE(_21PAD1_REG(0xfd),0x0)
	CS_WRITE_BYTE(_21PAD1_REG(0xfc),0x40)
	// Program Rank Select 7 Addressing Reg to 64=0x40
	CS_WRITE_BYTE(_21PAD1_REG(0xff),0x0)
	CS_WRITE_BYTE(_21PAD1_REG(0xfe),0x40)
	jmp done_for_now
#else
	// bl = value to assign to register
	// cx = target address
	xorl 	%ebx, %ebx
	xorl 	%ecx, %ecx
	
	// Sizing memory from register 0x80-87
	// sizeInMB = (2^(#ofRowAddr + #ofColumnAddr + Datawidth + 
	// 	  #BanksperROw))/ 8
	// (bl should be 0x40 for 1GB DDR)
check_size:	
	xorl	%ecx, %ecx
	movb	$3, %bl
	CALLSP(read_spd)
	movb 	%al, %cl
	movb	$4, %bl
	CALLSP(read_spd)
	addb 	%al, %cl
	movb	$17, %bl
	CALLSP(read_spd)
	addb 	%al, %cl
	movb	$13, %bl
	CALLSP(read_spd)
	addb 	%al, %cl
	
	// 16MB Granularity 
	subb	$24, %cl

	// 2^(%cl)	
	movl	$0x1, %ebx
	shll	%cl, %ebx	// size if bit
	shrl	$0x3, %ebx	// size if Byte
	
	movl 	%ebx, %ecx	
	// Check double-sided
	movb	$5, %bl
	CALLSP(read_spd)
	testb	$0x2, %al
	jz	done_check_size			
	 
	// Multiply by 2
	shll	$0x1, %ecx
	
done_check_size:
	
	movl	%ecx, %ebx
	// Program the Local Lower Mem range to 0
	CS_WRITE_BYTE(_21PAD0_REG(0x75),0x0)
	CS_WRITE_BYTE(_21PAD0_REG(0x74),0x0)

	// Program the Local/Global-32/Global Upper Mem range to 64=0x40
	xorl 	%eax, %eax
	xorl 	%edx, %edx
	movl	$0xc075, %ecx
sizing_loop1:
	incl	%ecx
	movw 	%cx, %ax
	movb 	%bl, %dl
	PCI_WRITE_CONFIG_BYTE
	
	incl	%ecx
	movw 	%cx, %ax
	movb 	$0x0, %dl
	PCI_WRITE_CONFIG_BYTE
	
	cmpw	$0xc07b, %cx
	jl	sizing_loop1

	// NOTE: For rlx 800i, there is only 1 DIMM
	// 	 which use reg 0x80 and 0x81 for the 2-sided

	// Program Rank Select Register 0-7
	// Function 1 offset 0xf0-0xff
	
	// Program Rank Sel register 0 with 1/2 the RAM
	movw	$0xc1f0, %ax
	movb	%bl, %dl
	shrl	$0x1, %edx
	PCI_WRITE_CONFIG_BYTE
	movw	$0xc1f1, %ax
	movb	$0x0, %dl
	PCI_WRITE_CONFIG_BYTE
	
	// Program Rank Sel register 1 with the full size of RAM
	movw	$0xc1f1, %cx
sizing_loop2:
	incl	%ecx
	movw 	%cx, %ax
	movb 	%bl, %dl
	PCI_WRITE_CONFIG_BYTE
	
	incl	%ecx
	movw 	%cx, %ax
	movb 	$0x0, %dl
	PCI_WRITE_CONFIG_BYTE

	cmpw	$0xc1ff, %cx
	jl	sizing_loop2

#endif
	
ram_not_found:
	CONSOLE_INFO_TX_STRING($RAM_not_found)
	jmp done_for_now

fail_verify:
	CONSOLE_INFO_TX_HEX8(%al)
	CONSOLE_INFO_TX_STRING($fail_write_read_verify)
	jmp done_for_now

done_for_now:
	CONSOLE_INFO_TX_STRING($DONE_FOR_NOW)
	CALLSP(dumpnorth)
