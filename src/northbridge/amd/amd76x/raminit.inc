jmp amd76x_out

#define DEBUG_RAM_CONFIG 0

#if DEBUG_RAM_CONFIG
	/*
	 * Routine:	dump_sdram_config
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	%eax, %ebx, %edx, %esp, %eflags
	 * Effects:	Writes pci config space to the serial port for 
	 *              device 0 functions 0&1
	 * Notes:
	 */
dump_sdram_config:
	movl	$0x00000000, %ebx
dump_sdram_config_byte_header:
	testb	$0x0f,%bl
	jnz	dump_sdram_config_byte
	CONSOLE_DEBUG_TX_CHAR($'\r')
	CONSOLE_DEBUG_TX_CHAR($'\n')
	movl	%ebx, %eax
	CALLSP(ttys0_tx_hex32)
	CONSOLE_DEBUG_TX_CHAR($':')
	CONSOLE_DEBUG_TX_CHAR($' ')

dump_sdram_config_byte:
	CONSOLE_DEBUG_TX_CHAR($' ')
	movl	%ebx, %eax
	PCI_READ_CONFIG_BYTE
	CALLSP(ttys0_tx_hex8)
dump_sdram_next_byte:
	incl	%ebx
	cmpl	$0x00000200, %ebx
	jne	dump_sdram_config_byte_header
	CONSOLE_DEBUG_TX_CHAR($'\r')
	CONSOLE_DEBUG_TX_CHAR($'\n')
	RET_LABEL(dump_sdram_config)
#endif

	/* Config Status (Read Only)
	 * 0x88
	 * [31:29] AGP Clock Mux
	 * [28:26] System Clock Mux
	 * [25:25] Type Detect
	 *         0 == AGP card uses 1.5V signalling
	 *         1 == AGP card uses 3.3V signalling
	 * [24:24] AMD Athlon Processor System Bus Threshold
	 *         0 == Bus input threshold between: 1.3V and 1.9V
	 *         1 == Bus input threshold between: 2.0V and 2.2V
	 * [23:23] AMD Athlon Processor Push-Pull Driver
	 *         0 == Disabled
	 *         1 == Enabled
	 * [22:22] AMD-762 System Controller Push-Pull Driver
	 *         0 == Disabled
	 *         1 == Enabled
 	 * [21:20] AMD-762 Clock Speed
	 *         00 == 100Mhz
	 *         01 == 66Mhz
	 *         10 == Reserved
	 *         11 == 133Mhz
	 * [19:18] AMD Athlon Processor 1 System Bus Length
	 *         00 == Short
	 *         ...
	 *         11 == Long
	 * [17:16] AMD Athlon Procesor 0 System Bus Length
	 *         00 == Short
	 *         ...
	 *         11 == Long
	 * [15:15] Tristate Enable
	 * [14:14] NAND Enable
	 * [13:13] Bypass_PLLs
	 *         0 == AMD-762 PLLs enabled
	 *         1 == AMD-762 PLLs bypassed
	 * [12:12] Disable Divider
	 *  [11:8] CPU1_Divider
	 *         0000 ==  11.0 
	 *         0001 ==  11.5 
	 *         0010 ==  12.0 
	 *         0011 ==  12.5
	 *         0100 ==  5.0 
	 *         0101 ==  5.5 
	 *         0110 ==  6.0 
	 *         0111 ==  6.5 
	 *         1000 ==  7.0 
	 *         1001 ==  7.5 
	 *         1010 ==  8.0 
	 *         1011 ==  8.5 
	 *         1100 ==  9.0 
	 *         1101 ==  9.5  
	 *         1110 ==  10.0
	 *         1111 ==  10.5
	 *   [7:7] External SIP ROM Enabled
	 *   [6:6] 66MhzPCI
	 *         0 == This platform only supports a 33Mhz PCI bus
	 *         1 == This platfrom supports a 66/33Mhz PCI bus
	 *   [5:5] INCLK Enable
	 *   [4:4] OUTCLCK Enable
	 *   [3:0] CPU0_Divider
	 *         0000 ==  11.0 
	 *         0001 ==  11.5 
	 *         0010 ==  12.0 
	 *         0011 ==  12.5
	 *         0100 ==  5.0 
	 *         0101 ==  5.5 
	 *         0110 ==  6.0 
	 *         0111 ==  6.5 
	 *         1000 ==  7.0 
	 *         1001 ==  7.5 
	 *         1010 ==  8.0 
	 *         1011 ==  8.5 
	 *         1100 ==  9.0 
	 *         1101 ==  9.5  
	 *         1110 ==  10.0
	 *         1111 ==  10.5
	 */
	
	/* DDR PDL Configuration Registers
	 * 0x144 - 0x188
	 * [31:24] Clock Delay
	 *         This field provides the number of buffers that one-half of 
	 *         the period of the system clock
	 * [23:16] Software Calibration Delay
 	 *         This bit field represents the amount of delay that is 
	 *         required for the corresponding DQS.
	 *         256 times a percentage of the half period of the system clock.
	 * [15:8]  Calibration Delay
         *         Number of buffers used for last calibration delay
	 * [7:0]   Actual Delay
	 *         The calibration delay value in buffers that is in effect.
	 */

	/*
	 * Routine:	sdram_dynamic_pdl_config
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	%eax, %ebx, %edx, %esi, %eflags
	 * Effects:	Based upon clock speed and mainboard 
         *              set the timing parameters appropriately.
	 * Notes:	The table ``sdram_software_calibration_delay''
	 *		comes from mainboard_raminit.inc as the needed
 	 *		values may changed depending upon your motherboard.
	 *		
	 */
sdram_dynamic_pdl_config:
	/* read the config status to see if we are at 100 or 133 Mhz */
	movl	$0x80000088, %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfc, %dx
	inl	%dx, %eax
	/* Now isolate the appropriate bit field */
	shrl	$20, %eax
	andl	$3, %eax
	/* Now get the actual delay to use */
	xorl	%ebx, %ebx
	movb	sdram_software_calibration_delay(%eax), %bl
	/* Adjust the delay so it is in the right bits */
	shll	$16, %ebx

	/* Now set the configuration registers */
	movl	$0x80000144, %esi
sdram_set_pdl_config:
	movl	%esi, %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfc, %dx
	movl	%ebx, %eax
	outl	%eax, %dx

	addl	$0x4, %esi
	cmpl	$0x8000018c, %esi
	jl	sdram_set_pdl_config
	RET_LABEL(sdram_dynamic_pdl_config)

	/*
	 * Table:	constant_register_values
	 */
	.p2align 3
constant_register_values:
	/* ECC Mode/Status
	 * 0x48
	 * [15:14] Serr_Enable
	 *         00 == SERR# Assertion is disabled
	 *         x1 == Muliple bit errors force SERR assertion
	 *         1x == Single bit errors force SERR assertion
	 * [12:12] ECC_Debug 1 == Enable ECC debugging mode
	 *
	 * [11:10] ECC_Mode
	 *         00 == ECC disabled
	 *         01 == EC_Hiperf Error checking and status reporting is enabled
	 *         10 == ECC_Hiperf Error checking, status reporting, 
	 *               and PCI/AGP destined memory is corrected.
	 *         11 == ECC_Scrub Error checking, status reporting, 
	 *               PCI/AGP destined memory is corrected, and
	 *               All corrected errors are written back to main memory.
	 * [09:08] ECC_Status
	 *         00 == No error
	 *         x1 == Multi bit error occured
	 *         1x == Single bit error occured
	 *
	 * [07:04] ECC_CS_MED
	 *         Chip on which first multi bit ECC error occured
	 * [03:00] ECC_CS_SED
	 *         Chip on which first single bit ECC error occured
 	 *
	 * Current settings:
	 * - All ECC errors trigger an exception.
	 * - Use hiperformace ECC error checking, and PCI/AGP error correction.
	 * - Clear the ECC_Status
	 * 
	 */
.long	0x48, (3 << 14)|(2 << 10)|(3 << 8)|(0 << 4)|(0 << 0)
	/* PCI_Control */
	/* 0x4C 
	 * [02:02] 1 == PCI Delayed Transaction Enable
	 * [01:01] 1 == PCI Ordering Rules Compliance Enable
	 * [00:00] 1 == Function 1 Enable
	 */
.long	0x4C, (1 << 2)|(1 << 1)|(1 << 0)
#if 0 /* Calculated via SPD */
	/* DRAM Timing 
	 * 0x54
	 * [31:31] 1 == Add wait state on all superbypass reads
	 * [30:30] Address Timing for copy A 
	 *         0 == No extra delay
	 *         1 == XXps delay
	 * [29:29] Address Timing for copy B
	 *         0 == No extra delay
	 *         1 == XXps delay
	 * [28:28] 1 == Add read wait state
	 *
	 * [27:27] 1 == Registered DIMM enabled
	 * [26:26] tWTR: Write to Read Delay
	 *         0 == tWTR duration is 1 clock cycle
	 *         1 == tWTR duration is 2 clock cycles
	 * [25:24] tWR: Write recovery time
	 *         00 == tWR duration is 1 clock cycle
	 *         01 == Reserved
	 *         10 == tWR duration is 2 clock cycles
	 *         11 == tWR duration is 3 clock cycles
	 * [23:23] tRRD: Activate Bank A to Activate Bank B Command Delay
	 *         0 == tRRD duration is 2 clock cycles
	 *         1 == tRRD duration is 3 clock cycles
	 *
	 * [18:16] Idle Cycle Limit
	 *         111 = Disable idle precharge
	 *         110 = 48 cycles
	 *         101 = 32 cycles
	 *         100 = 24 cycles
	 *         011 = 16 cycles
	 *         010 = 12 cycles
	 *         001 =  8 cycles (recommended "safe" configuration)
	 *         000 =  0 cycles
	 *
	 * [15:14] Page Hit limit
	 *         00 == 1 cycle
	 *         01 == 4 cycles
	 *         10 == 8 cycles (recommended "safe" configuration)
	 *         11 == 16 cycles
	 *
	 * [11:09] tRC: minimum time from activate to activate of the same bank
	 *         111 = 10 cycles
	 *         110 =  9 cycles
	 *         101 =  8 cycles (recommended "safe" configuration)
	 *         100 =  7 cycles
	 *         011 =  6 cycles
	 *         010 =  5 cycles
	 *         001 =  4 cycles
	 *         000 =  3 cycles
	 * [08:07] tRP:	precharge time
	 *         00 = 3 cycles
	 *         01 = 2 cycles
	 *         10 = 1 cycles
	 *         11 = 4 cycles
	 * [06:04] tRAS: minimum bank active time (time between activate & precharge)
	 *         111 = 9 cycles
	 *         110 = 8 cycles
	 *         101 = 7 cycles (recommeded "safe" confiuration)
	 *         100 = 6 cycles
	 *         011 = 5 cycles
	 *         010 = 4 cycles
	 *         001 = 3 cycles
	 *         000 = 2 cycles
	 * [03:02] CAS latency of SDRAM
	 *         11 = Reserved
	 *         10 = 2.5 cycles
	 *         01 = 2 cycles
	 *         00 = 3 cycles
	 * [01:00] tRCD: Ras to Cas Delay (from activate to r/w command)
	 *         11 = 4 cycles
	 *         10 = 3 cycles (recommeded "safe" configuration)
	 *         01 = 2 cycles
	 *         00 = 1 cycles
	 *
	 * Current settings:	
	 * tRCD = ceil(20ns/ 7.5ns) = 3 cycles
	 * CAS = 2.5 cycles
	 * tRAS = ceil(45ns / 7.5ns) = 6 cycles
	 * tRP = ceil(20ns / 7.5ns) = 3 cycles
	 * tRC = != 0 -> tRAS + tRP = ceil((20ns + 45ns) / 7.5 ns) = 9 cycles
	 *
	 * Page Hit Limit = 8 cycles
	 * Idle Cycle Limit = 8 cycles
	 *
	 * tRRD = ceil(15ns / 7.5ns) = 2 cycles
	 *
	 * tWR = guess = 2 cycles
	 * tWTR = guess = 2 cycles
	 * Registered DIMM enabled
	 * Add Read Wait State
	 * Address Timing for copy B XXps delay because of registered DIMMs
	 * Address Timing for copy A XXps delay because of registered DIMMs
	 * Superbypass Wait State Enabled because we are 133Mhz not 100Mhz
	 */
.long	0x54, (1 << 31)|(1 << 30)|(1 << 29)|(1 << 28)|(1 << 27)|(1 << 26)|(2 << 24)|(0 << 23)|(1 << 16)|(2 << 14)|(6 << 9)|(0 << 7)|(4 << 4)|(2 << 2)|(2 << 0)
#else
.long	0x54, 0
#endif
	/* DRAM Mode/Status 
	 * 0x58
	 * [31:26] Clock Disable 0-5
	 *         0 = Clock Pair Enabled
	 *         1 = Clock Pair Disabled
	 * [25:25] SDRAM initialization (Set to one to begin SDRAM initialization)
	 * [23:23] Mode Register Status 1 == Set and Writing
	 * [22:21] Suspend to Ram Control
	 *         00 == Not yet programmed. (Won't do memory inialization)
	 *         01 == Resume from suspend to disk or normal startup
	 *         1X == Resume from suspend to ram.
	 * [20:20] 1 == Burst Refresh Enable
	 * [19:19] 1 == Refresh Disable
	 * [17:16] Cycles per refresh
	 *               System Clock
	 *               66Mhz    100Mhz   133Mhz
	 *         00 =  30.72us  20.48us  15.36us
	 *         01 =  23.34us  15.36us  11.52us
	 *         10 =  15.36us  10.24us   7.68us
	 *         11 =   7.68us   7.68us   3.84us
	 * [07:00] Chip Select N X4 mode Enable
	 *         0 == This chip select consists of non X4 device
	 *         1 == This chip select consists of X4 device
	 *
	 * Current values:
	 * No X4 devices present
	 * Refresh rate needed 15.6us using 15.36us
	 * Initially refresh is disabled
	 * Burst refresh disabled (documented as buggy)
	 * Not doing a Load Mode register yet.
	 * Not doing SDRAM initialization yet.
	 * All clock lines enabled.
	 */
	/* 0x02210000 */
.long 0x58, (0 << 31)|(0 << 30)|(0 << 29)|(0 << 28)|(0 << 27)|(0 << 26)|(0 << 25)|(0 << 23)|(1 << 21)|(0 << 20)|(1 << 19)|(00 << 16)|(0 << 7)|(0 << 6)|(0 << 5)|(0 << 4)|(0 << 3)|(0 << 2)|(0 << 1)|(0 << 0)
	/* Memory Status/Control
	 * 0x70
	 * [18:18] == 1 Ram Self-Refresh Enable
	 * [10:10] == 1 PCI Pipeline Memory Accesses
	 * [09:09] == 1 PCI Block Write Enable
	 */
.long 0x70, ( 1 << 18) | (1 << 10) | ( 1 << 9)

	/* Memory Base Address Registers
	 * 0xC0, 0xC4, 0xC8, 0xCC, 0xD0, 0xD4, 0xD8, 0xDC
	 * [31:23] Chip Select Base (8MB aligned)
	 * [15:07] Chip Select Mask (Set bits are ignored)
	 * [02:01] Addressing Mode
	 *         00 == Reserved
	 *         01 == Used for 64MB & 128MB SDRAM
	 *         10 == Used for 256MB & 512MB SDRAM
	 *         11 == Reserved
	 * [00:00] Chip Select Enable
	 *
	 * Current Setting:
	 * All banks empty
	 * This is configured by the SPD code
	 */
.long 0xC0, (0x000 << 23)|(0x000 << 7)|(0 << 1)|(0 << 0)
.long 0xC4, (0x000 << 23)|(0x000 << 7)|(0 << 1)|(0 << 0)
.long 0xC8, (0x000 << 23)|(0x000 << 7)|(0 << 1)|(0 << 0)
.long 0xCC, (0x000 << 23)|(0x000 << 7)|(0 << 1)|(0 << 0)
.long 0xD0, (0x000 << 23)|(0x000 << 7)|(0 << 1)|(0 << 0)
.long 0xD4, (0x000 << 23)|(0x000 << 7)|(0 << 1)|(0 << 0)
.long 0xD8, (0x000 << 23)|(0x000 << 7)|(0 << 1)|(0 << 0)
.long 0xDC, (0x000 << 23)|(0x000 << 7)|(0 << 1)|(0 << 0)

	/* DDR PDL Calibration Control
	 * 0x140
	 * [7:7] Software Recalibration
	 *       0 == Calibration complete (default)
	 *       1 == Calibration inprogress / Start Calibration
	 * [6:6] Use Actual Delay
	 *       Writing a 1 says to use the manually calibrated values.
	 * [5:5] Auto Calibration mode
	 *       0 == Auto-calibration mode off(default)
	 *       1 == Auto-Calibration mode on
	 * [4:4] Actual Delay Update Inhibit
	 *       0 == Update all the PDL's after calibration
	 *       1 == Do not update the Actual PDL delay values
	 * [1:0] Auto Calibratio Period
	 *       00 == 10000 system clocks
	 *       01 == 1000000 system clocks
	 *       10 == 10000000 system clocks
	 *       11 == reserved
	 */
.long 0x140, ((0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<0))
	
	/* DDR Pad Configuration
	 * 0x8C [31:16] DQS Pad
	 * 0x8C [15:00] MDAT Pad
	 * 0x90 [31:16] CLK Pad
	 * 0x90 [15:00] CS Pad
	 * 0x94 [31:16] CMDB Pad
	 * 0x94 [15:00] CMDA Pad
	 * 0x98 [31:16] MAB Pad
	 * 0x98 [15:00] MAA Pad
	 *
	 * [13:11] Rising (P) Edge Slew Rate
	 *         000 == Slew rate 0 (slowest)
	 *         001 == Slew rate 1
	 *         ...
	 *         111 == Slew rate 7 (fastest)
	 * [10:08] Falling (N) Edge Slew Rate
	 *         000 == Slew rate 0 (slowest)
	 *         001 == Slew rate 1
	 *         ...
	 *         111 == Slew rate 7 (fastest)
	 * [03:02] P Transitor Drive Strength
	 *         00 == Drive Strength 0 (weakest)
	 *         01 == Drive Strength 1
	 *         10 == Drive Strength 2
	 *         11 == Drive Strength 3 (strongest)
	 * [01:00] N Transitor Drive Strength
	 *         00 == Drive Strength 0 (weakest)
	 *         01 == Drive Strength 1
	 *         10 == Drive Strength 2
	 *         11 == Drive Strength 3 (strongest)
	 */
#define DEFAULT_PAD_CONFIG ((5<<27)|(5 <<24)|(3 <<18)|(2<<16)|(5<<11)|(5<<8)|(3<<2)|(2<<0))
.long 0x18c, DEFAULT_PAD_CONFIG
.long 0x190, DEFAULT_PAD_CONFIG
.long 0x194, DEFAULT_PAD_CONFIG
.long 0x198, DEFAULT_PAD_CONFIG

#if 0
	/* disable AGP */
.long 0xA8, 0
.long 0xAC, 0
.long 0xB0, 0
#endif
	
constant_register_values_end:

	/*
	 * Routine:	ram_set_registers
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	%eax, %ebx, %ecx, %edx, %esi, %eflags
	 * Effects:	Do basic ram setup that does not depend on serial
	 *		presence detect information.
	 *		This sets PCI configuration registers to known good
	 *		values based on the tables: 
	 *			constant_register_values
	 *			mainboard_constant_register_values
	 *		Which are a pair of configuration regiser, and value.
	 *		
	 *		sdram_dynamic_pdl_config is called, to set near PCI configuration
	 *		restigers from the near constants that depend on FSB speed.
	 *		
	 * Notes:	The table ``mainboard_constant_register_values''
	 *		comes from mainboard_raminit.inc as the needed
	 *		values may changed depending upon your motherboard.
	 *		
	 */
	.p2align 3
ram_set_registers:
	movl	$constant_register_values, %ebx
	jmp	ram_set_one_register_start
ram_set_one_register:
#if DEBUG_RAM_CONFIG
	movl	%ebx, %esi
	CONSOLE_DEBUG_TX_CHAR($'C')
	CONSOLE_DEBUG_TX_CHAR($':')
	movl	0(%esi), %eax
	CALLSP(ttys0_tx_hex32)
	CONSOLE_DEBUG_TX_CHAR($':')
	movl	4(%esi), %eax
	CALLSP(ttys0_tx_hex32)
	CONSOLE_DEBUG_TX_CHAR($'\r')
	CONSOLE_DEBUG_TX_CHAR($'\n')
	movl	%esi, %ebx
#endif /* DEBUG_RAM_CONFIG */
	movl	0(%ebx), %eax
	movl	4(%ebx), %ecx
	PCI_WRITE_CONFIG_DWORD
	addl	$8, %ebx
ram_set_one_register_start:
	cmpl	$constant_register_values_end, %ebx
	jb	ram_set_one_register

	movl	$mainboard_constant_register_values, %ebx
	jmp	ram_set_one_mainboard_register_start
ram_set_one_mainboard_register:
#if DEBUG_RAM_CONFIG
	movl	%ebx, %esi
	CONSOLE_DEBUG_TX_CHAR($'M')
	CONSOLE_DEBUG_TX_CHAR($'C')
	CONSOLE_DEBUG_TX_CHAR($':')
	movl	0(%esi), %eax
	CALLSP(ttys0_tx_hex32)
	CONSOLE_DEBUG_TX_CHAR($':')
	movl	4(%esi), %eax
	CALLSP(ttys0_tx_hex32)
	CONSOLE_DEBUG_TX_CHAR($'\r')
	CONSOLE_DEBUG_TX_CHAR($'\n')
	movl	%esi, %ebx
#endif /* DEBUG_RAM_CONFIG */
	movl	0(%ebx), %eax
	movl	4(%ebx), %ecx
	PCI_WRITE_CONFIG_DWORD
	addl	$8, %ebx
ram_set_one_mainboard_register_start:
	cmpl	$mainboard_constant_register_values_end, %ebx
	jb	ram_set_one_mainboard_register
	
	CALL_LABEL(sdram_dynamic_pdl_config)
RET_LABEL(ram_set_registers)


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
	 * Routine:	set_dimm_size
	 * Arguments:	%esp return address
	 *              %edi width of the dimm in bits
	 *		%edx dim base register
	 *
	 * Preserved:	%ebx, %esi
	 * Used:	%eax, %ecx, %edx, %esp, %edi, %eflags
	 * Trashed:	%eax, %ecx, %edx, %esp, %edi, %eflags
	 * Effects:	Set the specified dimm base register
	 *
	 */

set_dimm_size:
	/* Convert ram width in bits to log base 2 ram size in 8MB quantities */
	xorl	%eax, %eax
	subl	$26, %edi
	jb	1f

	/* Compute the ignore bits for the DIMM side 1 */
	movl	%edi, %ecx
	incl	%eax
	shll	%cl, %eax
	decl	%eax
	andl	$0x1ff, %eax
	
	/* Add in the addressing mode */
	shll	$7, %eax
	orl	$3, %eax	/* Set the enable and mode 1 */
	cmpl	$32, %edi	/* 256MB */
	jb	1f
	xorl	$6, %eax	/* Clear mode 1 set mode 2 */
1:
	movl	%eax, %ecx	/* Value to write into ecx */
	movl	%edx, %eax	/* Address to write to is in eax */
	PCI_WRITE_CONFIG_DWORD
	RETSP

	/*
	 * Routine:	verify_dimm_enabled
	 * Arguments:	%esp return address
	 *		%edx DIMM register index
	 * Results:	%ecx -- Or'd with a nonzero value when the
	 *			dimm is enabled.
	 *
	 * Trashed:	%eax, %edx
	 *
	 * Results:	zf clear when the dimm is enabled
	 *		zf set when the dimm is not enabled
	 *
	 * Effects:	Tests to see if a DIMM has already been disabled.
	 */
verify_dimm_enabled:
	shll	$2, %edx
	movl	$0xC0, %eax
	addl	%edx, %eax
	PCI_READ_CONFIG_DWORD
	/* Only keep the enable bit */
	andl	$1, %eax
	/* Merge the result into %ecx */
	orl	%eax, %ecx
	RETSP


	/*
	 * Routine:	disable_dimm
	 * Arguments:	%esp return address
	 *		%edx DIMM register index
	 *
	 * Trashed:	%eax, %edx
	 *
	 * Effects:	Clears the base address register enable bit
	 *		for the specified dimm thereby making certain
	 *		that the specified dimm will not get used.
	 * Notes:	The PCI configuration access are hand rolled 
	 *		to minimize the register pressure.
	 */
disable_dimm:
	shll	$2, %edx

	movl	$0x800000C0, %eax
	addl	%edx, %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfc, %dx
	xorl	%eax, %eax
	outl	%eax, %dx
	RETSP

	/*
	 * Routine:	spd_set_ram_size
	 * Arguments:	None
	 *
	 * Preserved:	%ebp
	 * Used:	%eax, %ebx, %ecx, %edx, %esi, %edi, %esp, %eflags
	 * Trashed:	%eax, %ebx, %ecx, %edx, %esi, %edi, %esp, %eflags
	 * Effects:	Uses serial presence detect to set the Memory
	 *              Base Address registers which hold the starting
	 *              address and a mask for each DIMM.
	 *              Only the mask which holds the size of the DIMM
	 *              is set, 
	 * Notes:	
	 *              %ebx holds the configuration port & SMBUS_MEM_DEVICE for
	 *                   the current iteration through the loop.
	 *              %edi holds the memory size for the first side of the DIMM.
	 *		%esi holds the memory size for the second side of the DIMM.
	 *		     Memory size is represented as the log base 2
	 *		     width of the dimm in bits.
	 *                   An unset memory size is represented as -1 ie. 0xFFFFFFFF
	 * 		
	 *		The order of the DIMMS in the configuration
 	 *              registers is not necessarily the order of the
	 *              DIMMS on the smbus.  So spd_to_dimm_side0 and
	 *		spd_to_dimm_side1 must be called to map the
	 *		address from one form to the other.
	 *
	 *		spd_to_dimm_side0 and spd_to_dimm_side1 is
	 *		defined in mainboard_raminit.inc 
	 */
spd_set_ram_size:
	movl	$(SMBUS_MEM_DEVICE_START), %ebx
	/* Read the necessary SPD bytes for the first dimm */
spd_get_dimm_size:
	CALL_LABEL(sdram_spd_get_dimm_size)

	CALLSP(spd_to_dimm_side0)
	shll	$2, %edx
	addl	$0xC0, %edx
	CALLSP(set_dimm_size)

	CALLSP(spd_to_dimm_side1)
	shll	$2, %edx
	addl	$0xC0, %edx
	movl	%esi, %edi
	CALLSP(set_dimm_size)
	
	/* go to the next DIMM */
	addb	$(SMBUS_MEM_DEVICE_INC), %bl /* increment the smbus device */
	cmpb	$SMBUS_MEM_DEVICE_END, %bl
	jbe	spd_get_dimm_size
spd_set_ram_size_out:	
	RET_LABEL(spd_set_ram_size)

	/*
	 * Routine:	order_dimms
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
order_dimms:
	xorl	%ebp, %ebp	/* Top of memory & registers used */
	
order_try_dimms:	
	xorl	%ecx, %ecx 	/* Initialize the start register */
	xorl	%edi, %edi	/* largest config register */
	xorl	%esi, %esi	/* laregest config register value */
order_try_dimm:
	movl	%ecx, %eax
	shll	$2, %eax
	addl	$0xC0, %eax
	PCI_READ_CONFIG_DWORD

	testl	$0x00000001, %eax /* Is it enabled? */
	jz	order_next_dimm

	cmpl	%esi, %eax	/* Is it greater? */
	jbe	order_next_dimm

	btl	%ecx, %ebp	/* Has it already been selected? */
	jc	order_next_dimm

	/* I have a new canidate save it */
	movl	%ecx, %edi	/* config register */
	movl	%eax, %esi	/* config register value */
	
order_next_dimm:
	incl	%ecx		/* next config register */
	cmpl	$8, %ecx
	jb	order_try_dimm

	testl	%esi, %esi
	jz	order_dimms_out
	
	/* Set the highest register */
	btsl	%edi, %ebp	/* remember I have used this register */

	/* Compute the address to write to */
	movl	%edi, %eax
	shll	$2, %eax
	addl	$0xC0, %eax

	/* Compute the memory address increment */
	movl	%esi, %edx
	shrl	$7, %edx
	incl	%edx
	shll	$23, %edx

	/* Compute the memory base register */
	movl	%ebp, %ecx
	andl	$0xff800000, %ecx
	orl	%esi, %ecx

	/* Increment the top of memory */
	addl	%edx, %ebp

	/* Write the base register */
	PCI_WRITE_CONFIG_DWORD
	jmp order_try_dimms
	
order_dimms_out:
	/* clear the extra bytes from %ebp */
	andl	$0xff800000, %ebp

	/* If my memory size is zero go complian */
	testl	%ebp, %ebp
	jz	no_memory

	/* Set the PCI top of memory register */
	movl	$0x9C, %eax
	movl	%ebp, %ecx
	PCI_WRITE_CONFIG_DWORD

	CALLSP(set_memory_size)
	RET_LABEL(order_dimms)

	/*
	 * Routine:	spd_set_ecc_mode
	 * Arguments:	None
	 * Results:	None
	 *
	 * Trashed:	%eax, %ebx, %ecx, %edx, %esi, %edi, %ebp, %esp, %eflags
	 * Effects:	Disabled ECC if not all of the dimms support it.
	 *
	 * Notes:	%bl device on the smbus that is being processed.
	 *		%esi Current computed ECC mode register
	 *              
	 */
spd_set_ecc_mode:
	/* Read the initial ECC SDRAM mode */
	movl	$0x48, %eax
	PCI_READ_CONFIG_DWORD
	movl	%eax, %esi
	movl	$SMBUS_MEM_DEVICE_START, %ebx

spd_get_ecc_mode:
	/* Now test each DIMM in turn to see if ECC is supported. 
	 * If they all don't support ECC we need to disable it.
	 */
	movb	$11, %bh		/* ECC type */
	CALLSP(smbus_read_byte)
	jz	spd_get_ecc_mode_error

	/* If we support ECC sdram continue on. */
	cmpb	$2, %al
	je	spd_get_ecc_mode_next_dimm

	/* We don't support ECC so disable it. */
	andl	$0xfffff3ff, %esi

	jmp spd_get_ecc_mode_next_dimm
spd_get_ecc_mode_error:
	/* Disable dimms that give us fatal errors */
	CALLSP(spd_to_dimm_side0)
	CALLSP(disable_dimm)
	CALLSP(spd_to_dimm_side0)
	CALLSP(disable_dimm)
	
spd_get_ecc_mode_next_dimm:
	addb	$(SMBUS_MEM_DEVICE_INC), %bl	/* increment the smbus device */
	cmpb	$SMBUS_MEM_DEVICE_END, %bl
	jbe	spd_get_ecc_mode

	/* After we have looked at all of the dimms it is time
	 * write back the ECC configuration.
	 */
	movl	%esi, %ecx
	movl	$0x48, %eax
	PCI_WRITE_CONFIG_DWORD
spd_set_ecc_mode_out:
	RET_LABEL(spd_set_ecc_mode)



	/*
	 * Routine:	get_memory_bus_config
	 * Arguments:	%esp return address
	 * Results:	
	 *		cf clear
 	 *		%ebp contains a pointer to a table of configuration
	 *		information for the current memory bus.
	 *	On Error:
	 *		cf set
	 *		We have an unsupported memory bus speed.
	 *		
	 *
	 * Used:	%eax, %edx, %ebp, %esp, %eflags
	 * Trashed:	%eax, %edx
	 * Preserved:	%ebx, %ecx, %esi, %edi
	 *
	 * Notes:	The memory bus configuration is a set of tables that 
	 *		depend on the speed of the memory bus.
	 *		Currently those tables contain a pointer
	 *		to an array of cas latency spd index, and the maximum values
	 *		that apply at those various bus speeds.
	 */

/* Memory Bus Configuration.
 * A set of tables for values that depend upon the speed of the memory bus. 
 * Structure.
 * Pointer to cas latency indexes and the maximum values that apply at various bus speeds.
 */

cas_latency_index_bus_133:
	.byte 25, (7<<2)|2, 23, (7<<4)|5, 9, (7<<4)|5

cas_latency_index_bus_100:
	.byte 25, (10<<2)|0, 23, (10<<4)|0, 9, (10<<4)|0

#define MAX_SPD_REFRESH_RATE_INDEX 5
#define MIN_CHIPSET_REFRESH_RATE   0
refresh_rate_index_bus_133:
	.byte 0, 3, 2, 0, 0, 0

refresh_rate_index_bus_100:
	.byte 1, 3, 3, 0, 0, 0

#if 0
refresh_rate_index_bus_66:
	.byte 2, 3, 3, 0, 0, 0
#endif

#define MEM_CAS_LATENCY_INDEXES	0x00
#define MEM_QUATER_NS_PER_CYCLE	0x04
#define MEM_NS_PER_CYCLE	0x08
#define MEM_SUPERBYPASS_WAIT	0x0c
#define MEM_READ_WAIT_STATE	0x10
#define MEM_REFRESH_RATE	0x14

memory_bus133_config:
	.long	cas_latency_index_bus_133
	.long	0x1e	/* 30.0 quater ns per cycle */
	.long	0x08	/* |~7.51 ns~| per cycle */
	.long	1	/* Needed only at 133Mhz */
	.long	1	/* A read wait state is needed at 100 & 133Mhz */
	.long	refresh_rate_index_bus_133
memory_bus100_config:
	.long	cas_latency_index_bus_100
	.long	0x28	/* 40.0 quater ns per cycle */
	.long	0x0a	/* 10.0 ns per cycle */
	.long	0	/* Not needed below 133Mhz */
	.long	1	/* A read wait state is needed at 100 & 133Mhz */
	.long	refresh_rate_index_bus_100

get_memory_bus_config:
	movl	$0x88, %eax
	PCI_READ_CONFIG_DWORD
	shrl	$20, %eax
	andl	$3,  %eax
memory_bus100:
	cmpl	$0, %eax
	jnz	memory_bus133
	movl	$memory_bus100_config, %ebp
	clc
	jmp	get_memory_bus_config_out
memory_bus133:
	cmpl	$3, %eax
	jnz	memory_bus_error
	movl	$memory_bus133_config, %ebp
	clc
	jmp	get_memory_bus_config_out
memory_bus_error:
	stc
get_memory_bus_config_out:
	RETSP

	/*
	 * Routine:	get_dimm_cas_latency_setting
	 * Arguments:	%bl device on the smbus to read from 
	 *		%edi an initially zeroed copy of the DRAM
	 *		     timing register that applies to this
	 *		     particular chip.
	 *		%ebp Pointer to information that varies by memory bus speed
	 *
	 * Preserved:	%bl, %esi, %ebp
	 * Updated:	%edi
	 * Used:	%eax, %ebx, %ecx, %edx, %edi, %ebp, %esp, %eflags
	 * Trashed:	%eax, %ebx[31:8], %ecx, %edx, %esp,
	 * Results:	cf clear
	 *		%edi updated with the CAS latency settings
	 * 	On Error:	
	 *              cf set
	 *              We have an unsupported cas latency
	 *
	 * Effects:	Computes the CAS latency settings for the DRAM timing register.
	 *		The results of the computation are placed in %edi
	 * Notes:	
	 *		%ebx[31:24] smbus index of byte to read
	 *		%ebx[23:16] device on the smbus
	 *		%ebx[15: 8] bit of slowest latency the dimm might support
	 *		%ebx[ 7: 0] bitset of latency the dimm supports
	 *		%ecx	cas latency SPD index , and max speed table
	 *
	 */
get_dimm_cas_latency_setting:
	/* Get the best CAS latency supported by the AMD760 and the Dimm */
	/* first find the supported CAS latencies */
	/* Byte 18 for SDRAM is interpreted:
	 * Bit 0 == CAS Latency = 1
	 * Bit 1 == CAS Latency = 2
	 * Bit 2 == CAS Latency = 3
	 * Bit 3 == CAS Latency = 4
	 * Bit 4 == CAS Latency = 5
	 * Bit 5 == CAS Latency = 6
	 * Bit 6 == CAS Latency = 7
	 * Bit 7 == TBD
	 */
	/* Byte 18 for DDR SDRAM seems to be interpreted:
	 * Bit 0 == CAS Latency = 1.0
	 * Bit 1 == CAS Latency = 1.5
	 * Bit 2 == CAS Latency = 2.0
	 * Bit 3 == CAS Latency = 2.5
	 * Bit 4 == CAS Latency = 3.0
	 * Bit 5 == TBD
	 * Bit 6 == TBD
	 * Bit 7 == TBD
	 */
	/* Timing information on when you can use the supported CAS Latency
	 * is held in byte pairs 9,10 23,24 25,26.
	 * Let X be the CAS highest CAS latency.
	 * The first pair holds information on CAS Latency X.
	 * The second pair holds information on CAS Latency X - 1 (SDR) or X - 0.5 (DDR)
	 * The third pair holds information on CAS Latency X - 2 (SDR) or X - 1.0 (DDR)
	 */
	/* Read the supported CAS latencies */
	movb	$18, %bh
	CALLSP(smbus_read_byte)
	jz	get_dimm_cas_latency_error
	roll	$16, %ebx
	movb	%al, %bl	/* place the supported latencies in %bl */
	bsrl	%eax, %edx	/* find the highest bit set */
	/* Go back two steps to find the fastest speed we can try and support. */
	subl	$2, %edx
	/* Place the fasted speed in %bh */
	movb	%dl, %bh
	movl	MEM_CAS_LATENCY_INDEXES(%ebp), %ecx
cas_latency_1_0:
	cmpb	$0, %bh
	ja	cas_latency_1_5
	/* I can't support this so go to the next configuration */
	addl	$2, %ecx
cas_latency_1_5:
	cmpb	$1, %bh
	ja	cas_latency_2_0
	/* I can't support this so go to the next configuration */
	addl	$2, %ecx
cas_latency_2_0:
	cmpb	$2, %bh
	ja	cas_latency_2_5

	/* Increment the latency index in case we exit early */
	addl	$2, %ecx
	/* Is this latency supported by the dimm? */
	testb	$(1 << 2), %bl
	jz	cas_latency_2_5
	/* Read the fastest bus speed at which this latency is supported */
	roll	$16, %ebx
	movb	-2(%ecx), %bh
	CALLSP(smbus_read_byte)
	jz	get_dimm_cas_latency_error
	roll	$16, %ebx
	/* Is the memory bus too fast to use this latency */
	cmpb	-1(%ecx), %al
	ja	cas_latency_2_5

	/* We can use a cas_latency of 2.0 mark it in %edi */
	orl	$(1 << 2), %edi
	jmp	found_dimm_cas_latency

cas_latency_2_5:
	cmpb	$3, %bh
	ja 	cas_latency_3_0
	cmpb	$1, %bh
	jb	get_dimm_cas_latency_error_fixup

	/* Increment the latency index in case we exit early */
	addl	$2, %ecx
	/* Is this latency supported by the dimm? */
	testb	$(1 << 3), %bl
	jz	cas_latency_3_0
	/* Read the fastest bus speed at which this latency is supported */
	roll	$16, %ebx
	movb	-2(%ecx), %bh
	CALLSP(smbus_read_byte)
	jz	get_dimm_cas_latency_error
	roll	$16, %ebx
	/* Is the memory bus to fast to use this latency */
	cmpb	-1(%ecx), %al
	ja	cas_latency_3_0

	/* We can use a cas_latency of 2.5 mark it in %edi */
	orl	$(2 << 2), %edi
	jmp	found_dimm_cas_latency

cas_latency_3_0:
	cmpb	$4, %ch
	ja	get_dimm_cas_latency_error_fixup
	cmpb	$2, %ch
	jb	get_dimm_cas_latency_error_fixup

	/* Increment the latency index in case we exit early */
	addl	$2, %ecx
	/* Is this latency supported by the dimm? */
	testb	$(1 << 4), %cl
	jz	get_dimm_cas_latency_error_fixup
	/* Read the fastest bus speed at which this latency is supported */
	roll	$16, %ebx
	movb	-2(%ecx), %bh
	CALLSP(smbus_read_byte)
	jz	get_dimm_cas_latency_error
	roll	$16, %ebx
	/* Is the memory bus to fast to use this latency */
	cmpb	-1(%ecx), %al
	ja	get_dimm_cas_latency_error_fixup

	/* We can use a case latency of 3.0 mark it in %edi */
	orl	$(0 << 2), %edi
	jmp	found_dimm_cas_latency

found_dimm_cas_latency:
	roll	$16, %ebx
	clc
	jmp	get_dimm_cas_latency_out
get_dimm_cas_latency_error_fixup:
	roll	$16, %ebx
get_dimm_cas_latency_error:
	stc
get_dimm_cas_latency_out:	
	RET_LABEL(get_dimm_cas_latency_setting)


	/*
	 * Routine:	get_dimm_registered_setting
	 * Arguments:	%bl device on the smbus to read from 
	 *		%edi an initially zeroed copy of the DRAM
	 *		     timing register that applies to this
	 *		     particular dimm
	 *		%ebp Pointer to information that varies by memory bus speed
	 *
	 * Preserved:	%bl, %esi, %ebp, %ecx
	 * Updated:	%edi
	 * Used:	%eax, %ebx, %edx, %edi, %esp, %eflags
	 * Trashed:	%eax, %edx, %esp
	 * Results:	cf clear
	 *		%edi updated with the registered dimm settings
	 * 	On Error:	
	 *              cf set
	 *              We have an unsupported registered dimm setting
	 *
	 * Effects:	Test to see if a DIMM is registered or not.
	 *
	 */

get_dimm_registered_setting:
	/* Are the dimms registered? */
	movb	$21, %bh
	CALLSP(smbus_read_byte)
	jz	get_dimm_registered_setting_error
	/* It is unclear what constitutes a registered dimm, so assume
	 * any dimm that has some lines registered is a registered dimm.
	 */
	testl	$((1<<4)|(1<<1)), %eax
	jz	get_dimm_registered_setting_ok
	orl	$((1<<30)|(1<<29)|(1<<27)), %edi
get_dimm_registered_setting_ok:
	clc
	jmp get_dimm_registered_setting_out
get_dimm_registered_setting_error:
	stc
get_dimm_registered_setting_out:
	RET_LABEL(get_dimm_registered_setting)
	

	/*
	 * Routine:	get_dimm_ras_to_cas_delay
	 * Arguments:	%bl device on the smbus to read from 
	 *		%edi an initially zeroed copy of the DRAM
	 *		     timing register that applies to this
	 *		     particular dimm
	 *		%ebp Pointer to information that varies by memory bus speed
	 *
	 * Preserved:	%bl, %esi, %ebp, %ecx
	 * Updated:	%edi
	 * Used:	%eax, %ebx, %edx, %edi, %esp, %eflags
	 * Trashed:	%eax, %edx, %esp
	 * Results:	cf clear
	 *		%edi updated with tRCD setting
	 * 	On Error:	
	 *              cf set
	 *              We have an unsupported tRCD setting
	 *
	 * Effects:	Get the Ras to cas delay of a dimm
	 *
	 */
get_dimm_ras_to_cas_delay:
	movb	$29, %bh
	CALLSP(smbus_read_byte)
	jz	get_dimm_ras_to_cas_delay_error
	movl	MEM_QUATER_NS_PER_CYCLE(%ebp), %edx
	addb	%dl, %al	/* Make certain to round up */
	subb	$1, %al
	divb	%dl, %al
	cmpb	$4, %al
	ja	get_dimm_ras_to_cas_delay_error
	subb	$1, %al
	andl	$3, %eax
	orl	%eax, %edi
get_dimm_ras_to_cas_delay_ok:
	clc
	jmp get_dimm_ras_to_cas_delay_out
get_dimm_ras_to_cas_delay_error:
	stc	
get_dimm_ras_to_cas_delay_out:
	RET_LABEL(get_dimm_ras_to_cas_delay)



	/*
	 * Routine:	get_dimm_ras_setting
	 * Arguments:	%bl device on the smbus to read from 
	 *		%edi an initially zeroed copy of the DRAM
	 *		     timing register that applies to this
	 *		     particular dimm
	 *		%ebp Pointer to information that varies by memory bus speed
	 *
	 * Preserved:	%bl, %esi, %ebp, %ecx
	 * Updated:	%edi
	 * Used:	%eax, %ebx, %edx, %edi, %esp, %eflags
	 * Trashed:	%eax, %edx, %esp
	 * Results:	cf clear
	 *		%edi updated with tRAS setting
	 * 	On Error:	
	 *              cf set
	 *              We have an unsupported tRAS setting
	 *
	 * Effects:	Get the tRAS setting for a dimm
	 *
	 */
get_dimm_ras_setting:
	movb	$30, %bh
	CALLSP(smbus_read_byte)
	jz	get_dimm_ras_setting_error
	movl	MEM_NS_PER_CYCLE(%ebp), %edx
	addb	%dl, %al	/* Make certain to round up */
	subb	$1, %al
	divb	%dl, %al
	cmpb	$2, %al
	jb	get_dimm_ras_setting_low
	cmpb	$9, %al
	ja	get_dimm_ras_setting_error
	subl	$2, %eax
set_dimm_ras_setting:
	andl	$7, %eax
	shll	$4, %eax
	orl	%eax, %edi
get_dimm_ras_setting_ok:
	clc
	jmp get_dimm_ras_setting_out
get_dimm_ras_setting_low:
	xorl	%eax, %eax
	jmp	set_dimm_ras_setting
get_dimm_ras_setting_error:
	stc	
get_dimm_ras_setting_out:
	RET_LABEL(get_dimm_ras_setting)




	/*
	 * Routine:	get_dimm_row_precharge_setting
	 * Arguments:	%bl device on the smbus to read from 
	 *		%edi an initially zeroed copy of the DRAM
	 *		     timing register that applies to this
	 *		     particular dimm
	 *		%ebp Pointer to information that varies by memory bus speed
	 *
	 * Preserved:	%bl, %esi, %ebp, %ecx
	 * Updated:	%edi
	 * Used:	%eax, %ebx, %edx, %edi, %esp, %eflags
	 * Trashed:	%eax, %edx, %esp
	 * Results:	cf clear
	 *		%edi updated with tRP setting
	 * 	On Error:	
	 *              cf set
	 *              We have an unsupported tRP setting
	 *
	 * Effects:	Get the tRP setting for a dimm
	 *
	 */
dimm_row_precharge_translate:
	.byte	2, 1, 0, 3
get_dimm_row_precharge_setting:
	movb	$27, %bh
	CALLSP(smbus_read_byte)
	jz	get_dimm_row_precharge_setting_error
	movl	MEM_QUATER_NS_PER_CYCLE(%ebp), %edx
	addb	%dl, %al	/* Make certain to round up */
	subb	$1, %al
	divb	%dl, %al
	cmpb	$1, %al
	jb	get_dimm_row_precharge_setting_low
	cmpb	$4, %al
	ja	get_dimm_row_precharge_setting_error
	subb	$1, %al
set_dimm_row_precharge_setting:
	andl	$3, %eax
	movb	dimm_row_precharge_translate(%eax), %al
	shll	$7, %eax
	orl	%eax, %edi
get_dimm_row_precharge_setting_ok:
	clc
	jmp get_dimm_row_precharge_setting_out
get_dimm_row_precharge_setting_low:
	xorl	%eax, %eax
	jmp	set_dimm_row_precharge_setting
get_dimm_row_precharge_setting_error:
	stc	
get_dimm_row_precharge_setting_out:
	RET_LABEL(get_dimm_row_precharge_setting)


	/*
	 * Routine:	get_dimm_row_cycle_setting
	 * Arguments:	%bl device on the smbus to read from 
	 *		%edi an initially zeroed copy of the DRAM
	 *		     timing register that applies to this
	 *		     particular dimm
	 *		%ebp Pointer to information that varies by memory bus speed
	 *
	 * Preserved:	%bl, %esi, %ebp
	 * Updated:	%edi
	 * Used:	%eax, %ebx, %ecx, %edx, %edi, %esp, %eflags
	 * Trashed:	%eax, %ecx, %edx, %esp
	 * Results:	cf clear
	 *		%edi updated with tRC setting
	 * 	On Error:	
	 *              cf set
	 *              We have an unsupported tRC setting
	 * Note:	tRC = tRAS + tRP unless byte 0x41 is defined
	 *		In which case byte 0x41 should be used.
	 *
	 * Effects:	Get the tRC setting for a dimm
	 *
	 */
get_dimm_row_cycle_setting:
	movb	$41, %bh
	CALLSP(smbus_read_byte)
	jz	get_dimm_row_cycle_setting_error
	testb	%al, %al
	/* error out if we have the new SPD setting 
	 * This needs to be fixed as soon as we know what kind of
	 * units SPD byte 41 is in
	 */
	jnz	get_dimm_row_cycle_setting_new
	movb	$30, %bh
	CALLSP(smbus_read_byte)
	jz	get_dimm_row_cycle_setting_error
	movl	%eax, %ecx
	movb	$27, %bh
	CALLSP(smbus_read_byte)
	jz	get_dimm_row_cycle_setting_error
	shrl	$2, %eax
	addb	%cl, %al
	movl	MEM_NS_PER_CYCLE(%ebp), %edx
	addb	%dl, %al	/* Make certain to round up */
	subb	$1, %al
	divb	%dl, %al
	cmpb	$3, %al
	jb	get_dimm_row_cycle_setting_low
	cmpb	$10, %al
	ja	get_dimm_row_cycle_setting_error
	subb	$3, %al
set_dimm_row_cycle_setting:
	andl	$7, %eax
	shll	$9, %eax
	orl	%eax, %edi
get_dimm_row_cycle_setting_ok:
	clc
	jmp get_dimm_row_cycle_setting_out

get_dimm_row_cycle_setting_low:
	/* Low values can always be rounded up */
	xorl	%eax, %eax
	jmp	set_dimm_row_cycle_setting

dimm_row_new_cycle_setting_msg:
	.string "\r\n Newly defined SPD byte 41 populated please update the code to handle this\r\n"
get_dimm_row_cycle_setting_new:
	CONSOLE_DEBUG_TX_STRING($dimm_row_new_cycle_setting_msg)
	/* fall through */

get_dimm_row_cycle_setting_error:
	stc	
	/* fall through */

get_dimm_row_cycle_setting_out:
	RET_LABEL(get_dimm_row_cycle_setting)



	/*
	 * Routine:	get_dimm_row_to_row_delay_setting
	 * Arguments:	%bl device on the smbus to read from 
	 *		%edi an initially zeroed copy of the DRAM
	 *		     timing register that applies to this
	 *		     particular dimm
	 *		%ebp Pointer to information that varies by memory bus speed
	 *
	 * Preserved:	%bl, %esi, %ebp, %ecx
	 * Updated:	%edi
	 * Used:	%eax, %ebx, %edx, %edi, %esp, %eflags
	 * Trashed:	%eax, %edx, %esp
	 * Results:	cf clear
	 *		%edi updated with tRRD setting
	 * 	On Error:	
	 *              cf set
	 *              We have an unsupported tRRD setting
	 *
	 * Effects:	Get the tRRD setting for a dimm
	 *
	 */
get_dimm_row_to_row_delay_setting:
	movb	$28, %bh
	CALLSP(smbus_read_byte)
	jz	get_dimm_row_to_row_delay_setting_error
	movl	MEM_QUATER_NS_PER_CYCLE(%ebp), %edx
	addb	%dl, %al	/* Make certain to round up */
	subb	$1, %al
	divb	%dl, %al
	cmpb	$2, %al
	jb	get_dimm_row_to_row_delay_setting_low
	cmpb	$3, %al
	ja	get_dimm_row_to_row_delay_setting_error
	subb	$2, %al
set_dimm_row_to_row_delay_setting:
	andl	$1, %eax
	shll	$23, %eax
	orl	%eax, %edi
get_dimm_row_to_row_delay_setting_ok:
	clc
	jmp get_dimm_row_to_row_delay_setting_out
get_dimm_row_to_row_delay_setting_low:
	xorl	%eax, %eax
	jmp	set_dimm_row_to_row_delay_setting
get_dimm_row_to_row_delay_setting_error:
	stc	
get_dimm_row_to_row_delay_setting_out:
	RET_LABEL(get_dimm_row_to_row_delay_setting)



	/*
	 * Routine:	get_dimm_page_hit_limit_setting
	 * Arguments:	%bl device on the smbus to read from 
	 *		%edi an initially zeroed copy of the DRAM
	 *		     timing register that applies to this
	 *		     particular dimm
	 *		%ebp Pointer to information that varies by memory bus speed
	 *
	 * Preserved:	%bl, %esi, %ebp, %ecx
	 * Updated:	%edi
	 * Used:	%eax, %ebx, %edx, %edi, %esp, %eflags
	 * Trashed:	%eax, %edx, %esp
	 * Results:	cf clear
	 *		%edi updated with page hit limit setting
	 * 	On Error:	
	 *              cf set
	 *              We have an unsupported page hit limit setting
	 *
	 * Notes:	The page hit limit is hard coded at 8 cycles.
	 *		As this is recommeded and we have no need to change it.
	 *
	 */
get_dimm_page_hit_limit_setting:
	movl	$2, %eax	/* recommeded setting of 8 cycles */
	shll	$14, %eax
	orl	%eax, %edi
	clc
	RET_LABEL(get_dimm_page_hit_limit_setting)



	/*
	 * Routine:	get_dimm_idle_cycle_hit_limit_setting
	 * Arguments:	%bl device on the smbus to read from 
	 *		%edi an initially zeroed copy of the DRAM
	 *		     timing register that applies to this
	 *		     particular dimm
	 *		%ebp Pointer to information that varies by memory bus speed
	 *
	 * Preserved:	%bl, %esi, %ebp, %ecx
	 * Updated:	%edi
	 * Used:	%eax, %ebx, %edx, %edi, %esp, %eflags
	 * Trashed:	%eax, %edx, %esp
	 * Results:	cf clear
	 *		%edi updated with idle cycle limmit setting
	 * 	On Error:	
	 *              cf set
	 *              We have an unsupported idle cycle limit setting
	 *
	 * Notes:	The page idle cycle limit is hard coded at 8 cycles.
	 *		This is recommeded and there is no compelling reason
	 *		to change it.
	 *
	 */
get_dimm_idle_cycle_limit_setting:
	movl	$1, %eax	/* recommeded setting of 8 cycles */
	shll	$16, %eax
	orl	%eax, %edi
	clc
	RET_LABEL(get_dimm_idle_cycle_limit_setting)


	/*
	 * Routine:	get_dimm_write_recovery_setting
	 * Arguments:	%bl device on the smbus to read from 
	 *		%edi an initially zeroed copy of the DRAM
	 *		     timing register that applies to this
	 *		     particular dimm
	 *		%ebp Pointer to information that varies by memory bus speed
	 *
	 * Preserved:	%bl, %esi, %ebp, %ecx
	 * Updated:	%edi
	 * Used:	%eax, %ebx, %edx, %edi, %esp, %eflags
	 * Trashed:	%eax, %edx, %esp
	 * Results:	cf clear
	 *		%edi updated with tWR setting
	 * 	On Error:	
	 *              cf set
	 *              We have an tWR setting
	 *
	 * Notes:	Hardcoded to recomended 2 cycles.
	 *
	 */
get_dimm_write_recovery_setting:
	movl	$2, %eax	/* recommeded setting of 2 cycles */
	shll	$24, %eax
	orl	%eax, %edi
	clc
	RET_LABEL(get_dimm_write_recovery_setting)

	/*
	 * Routine:	get_dimm_write_to_read_setting
	 * Arguments:	%bl device on the smbus to read from 
	 *		%edi an initially zeroed copy of the DRAM
	 *		     timing register that applies to this
	 *		     particular dimm
	 *		%ebp Pointer to information that varies by memory bus speed
	 *
	 * Preserved:	%bl, %esi, %ebp, %ecx
	 * Updated:	%edi
	 * Used:	%eax, %ebx, %edx, %edi, %esp, %eflags
	 * Trashed:	%eax, %edx, %esp
	 * Results:	cf clear
	 *		%edi updated with tWTR setting
	 * 	On Error:	
	 *              cf set
	 *              We have an unsupported tWTR setting
	 *
	 * Notes:	Hardcoded to the recomended 2 cycles.
	 *
	 */
get_dimm_write_to_read_setting:
	movl	$1, %eax	/* recommeded setting of 2 cycles */
	shll	$26, %eax
	orl	%eax, %edi
	clc
	RET_LABEL(get_dimm_write_to_read_setting)


	/*
	 * Routine:	get_superbypass_wait_setting
	 * Arguments:	%bl device on the smbus to read from 
	 *		%edi an initially zeroed copy of the DRAM
	 *		     timing register that applies to this
	 *		     particular dimm
	 *		%ebp Pointer to information that varies by memory bus speed
	 *
	 * Preserved:	%bl, %esi, %ebp, %ecx
	 * Updated:	%edi
	 * Used:	%eax, %ebx, %edx, %edi, %esp, %eflags
	 * Trashed:	%eax, %edx, %esp
	 * Results:	cf clear
	 *		%edi updated with superbypass wait setting
	 * 	On Error:	
	 *              cf set
	 *              We have an unsupported superbypass wait setting
	 *
	 * Notes:	The superbypass wait setting is determined soly by the 
	 *		memory bus clock speed.
	 *
	 */
get_superbypass_wait_setting:
	movl	MEM_SUPERBYPASS_WAIT(%ebp), %eax
	shll	$31, %eax
	orl	%eax, %edi
	clc
	RET_LABEL(get_superbypass_wait_setting)


	/*
	 * Routine:	get_read_wait_state
	 * Arguments:	%bl device on the smbus to read from 
	 *		%edi an initially zeroed copy of the DRAM
	 *		     timing register that applies to this
	 *		     particular dimm
	 *		%ebp Pointer to information that varies by memory bus speed
	 *
	 * Preserved:	%bl, %esi, %ebp, %ecx
	 * Updated:	%edi
	 * Used:	%eax, %ebx, %edx, %edi, %esp, %eflags
	 * Trashed:	%eax, %edx, %esp
	 * Results:	cf clear
	 *		%edi updated with read wait state setting
	 * 	On Error:	
	 *              cf set
	 *              We have an unsupported read wait state setting
	 *
	 * Notes:	The read wait state setting is determined soly by the 
	 *		memory bus clock speed.
	 *
	 */
get_read_wait_state_setting:
	movl	MEM_READ_WAIT_STATE(%ebp), %eax
	shll	$28, %eax
	orl	%eax, %edi
	clc
	RET_LABEL(get_read_wait_state_setting)


	/*
	 * Routine:	spd_set_dram_timing
	 * Arguments:	None
	 * Results:	None
	 * Effect:	The DRAM timing register 0x54 is populated with
	 *		a workable value
	 *
	 * Trashed:	%eax, %ebx, %ecx, %edx, %esi, %edi, %esp, %ebp, %eflags
	 *
	 * Notes:	
	 *		%bl device on the smbus to read from 
	 *		%edi Accumulator for DIMM settings
	 *		%esi A copy of the dim setting we used.
	 *		%ebp Pointer to information that varies by memory bus speed
	 *
	 *		Currently we handle mixed dimms by disabling those
	 *		dimms whose timing does not match the timing of the
	 *		first dimm found.  Since many settings can be pessimized
	 *		it would be nice if we found the best and
	 * 		worst possible timing for a set of DIMMS and
	 *		only threw out those dimms that could not
	 *		possibly fit.
	 *
	 */

spd_set_dram_timing:
	/* Initial value for the DRAM Timing Register 0x54 */
	xorl	%edi, %edi
	xorl	%esi, %esi
	movl	$SMBUS_MEM_DEVICE_START, %ebx
	CALLSP(get_memory_bus_config)
	jc 	spd_set_dram_timing_out

spd_get_dram_timing:
	xorl	%ecx, %ecx
	CALLSP(spd_to_dimm_side0)
	CALLSP(verify_dimm_enabled)
	CALLSP(spd_to_dimm_side1)
	CALLSP(verify_dimm_enabled)
	testl	%ecx, %ecx
	jz	spd_dram_timing_error

	CALL_LABEL(get_dimm_registered_setting)
	jc	spd_dram_timing_error

	CALL_LABEL(get_dimm_cas_latency_setting)
	jc	spd_dram_timing_error

	CALL_LABEL(get_dimm_ras_to_cas_delay)
	jc	spd_dram_timing_error

	CALL_LABEL(get_dimm_ras_setting)
	jc	spd_dram_timing_error

	CALL_LABEL(get_dimm_row_precharge_setting)
	jc	spd_dram_timing_error

	CALL_LABEL(get_dimm_row_cycle_setting)
	jc	spd_dram_timing_error

	CALL_LABEL(get_dimm_row_to_row_delay_setting)
	jc	spd_dram_timing_error

	CALL_LABEL(get_dimm_page_hit_limit_setting)
	jc	spd_dram_timing_error

	CALL_LABEL(get_dimm_idle_cycle_limit_setting)
	jc	spd_dram_timing_error

	CALL_LABEL(get_dimm_write_recovery_setting)
	jc	spd_dram_timing_error

	CALL_LABEL(get_dimm_write_to_read_setting)
	jc	spd_dram_timing_error

	CALL_LABEL(get_superbypass_wait_setting)
	jc	spd_dram_timing_error
	
	CALL_LABEL(get_read_wait_state_setting)
	jc	spd_dram_timing_error

	CALL_LABEL(mainboard_verify_dram_timing)
	jc	spd_dram_timing_error

	/* Is this the first timing we have generated */
	testl	%esi,%esi
	jz 	set_dram_timing	

	/* Is this timing the same as the last timing we generated? */
	cmpl	%esi, %edi
	je	spd_get_dram_timing_next_dimm
spd_dram_timing_error:
	/* The two timings are not equal so disable the current dimm */
	CALLSP(spd_to_dimm_side0)
	CALLSP(disable_dimm)
	CALLSP(spd_to_dimm_side1)
	CALLSP(disable_dimm)

spd_get_dram_timing_next_dimm:
	xorl	%edi, %edi
	addb	$(SMBUS_MEM_DEVICE_INC), %bl	/* increment the smbus device */
	cmpb	$SMBUS_MEM_DEVICE_END, %bl
	jbe	spd_get_dram_timing
	/* After we have passed by all of the dimms
	 * it is time to leave.
	 */
	jmp	spd_set_dram_timing_out

set_dram_timing:
	movl	%edi, %esi	/* Save the value of the timing register */
	movl	$0x54, %eax
	movl	%edi, %ecx
	PCI_WRITE_CONFIG_DWORD
	jmp	spd_get_dram_timing_next_dimm

spd_set_dram_timing_out:
	RET_LABEL(spd_set_dram_timing)


	/*
	 * Routine:	spd_get_dimm_width
	 * Arguments:	%bl device on the smbus to read from
	 *		%esi an initially zeroed copy of the 
	 *		 DRAM mode register.
	 *		%ebp Pointer to information that varies by memory bus speed
	 *
	 * Results:	cf clear
	 *		%esi updated with dimm with setting
	 * 	On Error:	
	 *              cf set
	 *              We have an unsupported/unknown dimm width setting
	 *
	 * Trashed:	%eax, %bh, %ecx, %edx, %edi, %eflags
	 * Effects:	The size of each DIMM is set appropriately.
	 *              
	 */
spd_get_dimm_width:
	/* Read by 13 to see how wide I am */
	movb	$13, %bh
	CALLSP	(smbus_read_byte)
	jz	spd_get_dimm_width_error

	/* Now see if I'm an X4 device */
	xorl	%edi, %edi
	cmpb	$4, %al
	jne	1f
	movl	$0x1, %edi
1:
	/* Now set the appropriate bits */
	CALLSP(spd_to_dimm_side0)
	movl	%edx, %ecx
	movl	$1, %eax
	movl	%edi, %edx
	shll	%cl, %eax
	shll	%cl, %edx
	notl	%eax
	andl	%eax, %esi
	orl	%edx, %esi
	CALLSP(spd_to_dimm_side1)
	movl	%edx, %ecx
	movl	$1, %eax
	movl	%edi, %edx
	shll	%cl, %eax
	shll	%cl, %edx
	notl	%eax
	andl	%eax, %esi
	orl	%edx, %esi

	clc
	jmp	spd_get_dimm_width_out
spd_get_dimm_width_error:
	stc
spd_get_dimm_width_out:
	RET_LABEL(spd_get_dimm_width)



	/*
	 * Routine:	spd_get_dimm_refresh_rate
	 * Arguments:	%bl device on the smbus to read from
	 *		%esi an initially zeroed copy of the 
	 *		 DRAM mode register.
	 *		%ebp Pointer to infomration that varies by memory bus speed
	 *
	 * Results:	cf clear
	 *		%esi updated with dimm workable dimm refresh rate
	 *	On Error:	
	 *		cf set
	 *		We have an unsupported/unknown dimm refresh rate
	 *
	 * Trashed:	%eax, %bh, %ecx, %edx, %edi, %eflags
	 * Effects:	The refresh rate is set appropriately for the given
	 *		DIMM.  The refresh rate is never slowed down, just
	 *		speeded up.
	 *		
	 */

spd_get_dimm_refresh_rate:
	/* Read byte 12 to get the dimm's refresh rate */
	movb	$12, %bh
	CALLSP	(smbus_read_byte)
	jz	spd_get_dimm_refresh_rate_error

	/* Now lookup up the chipset setting for this refresh rate */
	andl	$0x7f, %eax
	cmpl	$MAX_SPD_REFRESH_RATE_INDEX, %eax
	ja	spd_get_dimm_refresh_rate_error
	/* Get the refresh rate table */
	movl	MEM_REFRESH_RATE(%ebp), %edx
	/* Now get the refresh rate */
	movb	(%edx,%eax,1), %al
	andl	$0x000000ff, %eax

	/* Isolate the old refresh rate setting */
	movl	%esi, %edx
	shrl	$16, %edx
	andl	$3, %edx

	/* See if the new refresh rate is higher than the old
	 * refresh rate setting.
	 */
	cmpl	%eax, %edx
	ja	spd_get_dimm_refresh_rate_ok

	/* Correctly position the new refresh rate */
	shll	$16, %eax
	/* Clear the old refresh rate */
	andl	$0xFFFCFFFF, %esi
	/* Insert the new refresh rate */
	orl	%eax, %esi
	jmp	spd_get_dimm_refresh_rate_ok

spd_get_dimm_refresh_rate_error:
	stc
	jmp spd_get_dimm_refresh_rate_out

spd_get_dimm_refresh_rate_ok:
	clc
spd_get_dimm_refresh_rate_out:
	RET_LABEL(spd_get_dimm_refresh_rate)


	/*
	 * Routine:	spd_set_dram_mode
	 * Arguments:	None
	 * Results:	None
	 *
	 * Trashed:	%eax, %ebx, %ecx, %edx, %esi, %edi, %ebp, %esp, %eflags
	 * Effects:	Sets the DRAM Mode register according to the SPD bytes.
	 *
	 * Notes:	%bl device on the smbus that is being processed.
	 *		%esi Current computed DRAM mode register
	 *		%ebp Pointer to information that varies by memory bus speed.
	 *              
	 */
spd_set_dram_mode:
	/* Read the initial SDRAM mode */
	movl	$0x58, %eax
	PCI_READ_CONFIG_DWORD
	/* Initialize the values we are setting to a known state. */
	andl	$0xfffcff00, %eax
	movl	%eax, %esi
	movl	$(SMBUS_MEM_DEVICE_START), %ebx
	CALLSP(get_memory_bus_config)
	jc	spd_set_dram_mode_out

spd_get_dram_mode:
	xorl	%ecx, %ecx
	CALLSP(spd_to_dimm_side0)
	CALLSP(verify_dimm_enabled)
	CALLSP(spd_to_dimm_side1)
	CALLSP(verify_dimm_enabled)
	testl	%ecx, %ecx
	jz spd_get_dram_mode_error

	CALL_LABEL(spd_get_dimm_width)	
	jc	spd_get_dram_mode_error
	CALL_LABEL(spd_get_dimm_refresh_rate)
	jc	spd_get_dram_mode_error

	jmp	spd_get_dram_mode_next_dimm
	
spd_get_dram_mode_error:
	/* We have an error getting information for this DIMM disable it */
	CALLSP(spd_to_dimm_side0)
	CALLSP(disable_dimm)
	CALLSP(spd_to_dimm_side1)
	CALLSP(disable_dimm)

spd_get_dram_mode_next_dimm:
	addb	$(SMBUS_MEM_DEVICE_INC), %bl	/* increment the smbus device */
	cmpb	$SMBUS_MEM_DEVICE_END, %bl
	jbe	spd_get_dram_mode

	/* After we have looked at all of the dimms it is
	 * time to set the mode.
	 */
spd_set_dram_mode_out:
	movl	%esi, %ecx
	movl	$0x58, %eax
	PCI_WRITE_CONFIG_DWORD
	RET_LABEL(spd_set_dram_mode)

	/*
	 * Routine:	ram_set_spd_registers
	 * Arguments:	None
	 * Results:	None
	 *
	 * Preserved:	None
	 * Effects:	Information is read from the SPD roms on every
	 *		dimm, and used to populate the appropriate
	 * 		configuration ram registers.  When done all
	 * 		that should be left to do is enable the RAM.
	 */
	
ram_set_spd_registers:
	/* 0xC0 - 0xDC Memory Base Address Registers */
	CALL_LABEL(spd_set_ram_size)
	/* 0x54 DRAM Timing */
	CALL_LABEL(spd_set_dram_timing)
	/* 0x58 DRAM Mode/Status */
	CALL_LABEL(spd_set_dram_mode)
	/* 0x48 ECC Mode/Status */
	CALL_LABEL(spd_set_ecc_mode)
	/* Order the RAM base address registers */
	CALL_LABEL(order_dimms)

	RET_LABEL(ram_set_spd_registers)


	/*
	 * Routine:	enable_sdram
	 * Arguments:	None
	 * Results:	None
	 *
	 * Preserved:	None
	 * Effects:	After this routine has been run you can access
	 *		memory on the currently plugged in DIMMS.
	 */
ram_enabled:	.string "Ram Enabled\r\n"

enable_sdram:
	/* Recalibrate the PDL timing registers */
	movl	$0x80000140, %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfc, %dx
	inl	%dx, %eax
	/* Force a manual  recalibration */
	orl	$0x80, %eax
	outl	%eax, %dx
recalibrate_sdram_wait:
	inl	%dx, %eax
	testl	$0x80, %eax	
	jnz	recalibrate_sdram_wait

	/* Enable auto Recalibration */
	orl	$(1<<5), %eax
	outl	%eax, %dx

	/* Enable the SDRAM */
	movl	$0x80000058, %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfc, %dx
	inl	%dx, %eax	/* Read the register */
	orl	$((1 <<25)|(1 << 23)), %eax  /* Set the enables */
	outl	%eax, %dx	/* Start initializing SDRAM */
enable_sdram_wait:
	movw	$0xcfc, %dx
	inl	%dx, %eax
	testl	$(1 << 23), %eax /* Wait until the mode register is set */
	jnz enable_sdram_wait	

	CONSOLE_DEBUG_TX_STRING($ram_enabled)

	RET_LABEL(enable_sdram)


	/*
	 * Routine:	enable_refresh
	 * Arguments:	None
	 * Results:	None
	 *
	 * Preserved:	None
	 * Effects:	During initialization refresh of the DIMMS has
	 *		been disabled so that it won't get in the way
	 *		of the initialization logic.  Now we enable
	 *		refresh so the DIMMS will preserve their contents.
	 */
enable_refresh:
	movl	$0x80000058, %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfc, %dx
	inl	%dx, %eax
	andl	$(~(1 << 19)), %eax
	outl	%eax, %dx
	RET_LABEL(enable_refresh)
	
#define ENABLE_REFRESH() CALL_LABEL(enable_refresh)

#define FIRST_NORMAL_REFERENCE()



#if DEBUG_RAM_CONFIG
#define SPECIAL_FINISHUP()		CALL_LABEL(dump_sdram_config)
#else
#define SPECIAL_FINISHUP()
#endif

get_ecc_ram_size_bytes_ebx:
	movl	$0x48, %eax
	PCI_READ_CONFIG_DWORD
	shrl	$10, %eax
	and	$3, %eax
	xorl	%ebx, %ebx	/* clear %ebx in case I don't have ecc enabled */
	/* See if ECC is enabled for the SDRAM */
	testl	%eax, %eax
	jz	get_ecc_ram_size_bytes_ebx_out

	movl	$0x9c, %eax
	PCI_READ_CONFIG_DWORD
	movl	%eax, %ebx
get_ecc_ram_size_bytes_ebx_out:
	RET_LABEL(get_ecc_ram_size_bytes_ebx)
	
amd76x_out:
