jmp	mainboard_raminit_out
#define DEFAULT_RAM_TRACE_SETTINGS 0
#define USE_ECC_SDRAM 1



	/*
	 * Routine:	spd_to_dimm_side0
	 * Arguments:	%bl SMBUS_MEM_DEVICE
 	 *
	 * Results:	%edx DIMM register index
	 * 
	 * Used:	%ebx, %edx, %esp
	 * Trashed:	%eflags
	 * Preserved:	%eax, %ebx, %ecx, %esi, %edi, %ebp
	 *
	 * Effects:	Dimms are not necessarily in the same order on the smbus
	 *		as they are in chipset register indexes.  This function
	 *		maps the SMBUS device id to the logical index in
	 *		the chipset, that is used to refer to a particular dimm.
	 */
spd_to_dimm_side0:
	movl	%ebx, %edx
	andl	$0xff, %edx
	subl	$(SMBUS_MEM_DEVICE_START), %edx
	/* 0 -> 0 */
	cmpl	$0, %edx
	jne	1f
	movl	$0, %edx
	RETSP
	/* 1 -> 2 */
1:	cmpl	$1, %edx
	jne	1f
	movl	$2, %edx
	RETSP
	/* 2 -> 4 */
1:	cmpl	$2, %edx
	jne	1f
	movl	$4, %edx
	RETSP
	/* 3 -> 6 */
1:	movl	$6, %edx
	RETSP


	/*
	 * Routine:	spd_to_dimm_side1
	 * Arguments:	%bl SMBUS_MEM_DEVICE
 	 *
	 * Results:	%edx DIMM register index
	 * 
	 * Used:	%ebx, %edx, %esp
	 * Trashed:	%eflags
	 * Preserved:	%eax, %ebx, %ecx, %esi, %edi, %ebp
	 *
	 * Effects:	Dimms are not necessarily in the same order on the smbus
	 *		as they are in chipset register indexes.  This function
	 *		maps the SMBUS device id to the logical index in
	 *		the chipset, that is used to refer to a particular dimm.
	 */
spd_to_dimm_side1:
	movl	%ebx, %edx
	andl	$0xff, %edx
	subl	$(SMBUS_MEM_DEVICE_START), %edx
	/* 0 -> 1 */
	cmpl	$0, %edx
	jne	1f
	movl	$1, %edx
	RETSP
	/* 1 -> 3 */
1:	cmpl	$1, %edx
	jne	1f
	movl	$3, %edx
	RETSP
	/* 2 -> 5 */
1:	cmpl	$2, %edx
	jne	1f
	movl	$5, %edx
	RETSP
	/* 3 -> 7 */
1:	movl	$7, %edx
	RETSP


/* Set the calibration delay.  These values may need to change per mainboard
 * so we put them here.
 */

sdram_software_calibration_delay:
#if DEFAULT_RAM_TRACE_SETTINGS
	.byte 0x69, 0x00, 0x00, 0x6b
#else
	.byte 0x69, 0x00, 0x00, 0x54
#endif

mainboard_constant_register_values:
#if DEFAULT_RAM_TRACE_SETTINGS
#else
	.long 0x18c, 0x090e2d0e
	.long 0x190, 0x3f0f2d0e
	.long 0x194, 0x2d0e2d0e
	.long 0x198, 0x2d0e2d0e
#endif
#if USE_ECC_SDRAM
	.long 0x48, (3 << 14)|(2 << 10)|(0 << 8)|(0 << 4)|(0 << 0)
#else
	.long 0x48, 0
#endif
mainboard_constant_register_values_end:


	/*
	 * Routine:	mainboard_verify_dram_timing
	 * Arguments:	%edi the computed timing for the current dimm.
	 * Trashed:	%eflags
	 * Results:	cf clear
	 *		%edi has a timing supported by this motherboard
	 * 	On Error:	
	 *              cf set
	 *		%edi holds a timing not supported by this motherboard
	 *
	 * Effects:	Verifies we can use the current dimm settings
	 *		on the tyan guinness motherboard.
	 *		Currently the only potential problem is putting
	 *		in unregistered SDRAM.
	 */
mainboard_verify_dram_timing:
	testl	$(1<<27), %edi
	jnz	mainboard_verify_dram_timing_ok	
mainboard_verify_dram_timing_error:
	stc
	jmp	mainboard_verify_dram_timing_out
mainboard_verify_dram_timing_ok:
	clc
mainboard_verify_dram_timing_out:
	RET_LABEL(mainboard_verify_dram_timing)

#undef DEFAULT_RAM_TRACE_SETTINGS
mainboard_raminit_out:
	
