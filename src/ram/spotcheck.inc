	/*
	 * This is much more of a "Is my SDRAM properly configured?"
	 * test than a "Is my SDRAM faulty?" test.  Not all bits
	 * are tested.   -Tyson
	 *
	 * Non temporal store and conversion to a spot check
	 * so systems with extremely bad memory will give a reasonable
	 * error message instead of freaking out. - Eric
	 */

	.section ".rom.data"
#ifndef CONFIG_MAX_SPOTCHECK_ERRORS
#define CONFIG_MAX_SPOTCHECK_ERRORS 20
#endif

# debugging SMJ
#define NO_HLT

spot_test:	.string " Spot checking: "
spot_error:	.string "Blatant memory errors found.\r\n"

	/*
	 * Routine:	spot_check
	 * Arguments:	eax starting memory address
	 *              ebx ending memory address
	 *              esp return address
	 * Results:	Returns if no errors are found.
	 * Trashed:	eax, ebx, edx, esi, edi, ebp
	 * Effects:	
	 */
	
spot_check:
	movl	%eax, %esi
	movl	%ebx, %edi
	movl	%esp, %ebp

	CONSOLE_INFO_TX_STRING($spot_test)
	CONSOLE_INFO_TX_HEX32(%esi)
	CONSOLE_INFO_TX_CHAR($'-')
	CONSOLE_INFO_TX_HEX32(%edi)
	CONSOLE_INFO_TX_CHAR($'\r')
	CONSOLE_INFO_TX_CHAR($'\n')

	/* =========== Fill ram block ======== */

	movl	%esi, %ebx
1:
	/* Use a non temporal store to go faster and
	 * to bypass the cache.
	 */
	movnti	%ebx, (%ebx)
	addl	$4, %ebx
	cmpl	%edi, %ebx
	jl	1b

	/* ========= Verify ram block ========== */

	movl	%esi, %ebx
	/* Keep error count in %esi */
	xorl	%esi, %esi

1:
	cmpl	%ebx, (%ebx)
	jne	4f
3:
	addl	$4, %ebx
	cmpl	%edi, %ebx
	jl	1b
	jmp	6f

4:
	/* Display address with error */

	CONSOLE_INFO_TX_HEX32(%ebx)
	CONSOLE_INFO_TX_CHAR($':')

	/* Display data in address with error */

	CONSOLE_INFO_INLINE_TX_HEX32(0(%ebx))

	CONSOLE_INFO_TX_CHAR($'\r')
	CONSOLE_INFO_TX_CHAR($'\n')
	incl	%esi
	cmpl	$CONFIG_MAX_SPOTCHECK_ERRORS, %esi
	jbe	3b
#ifndef NO_HLT
5:	
	CONSOLE_INFO_TX_STRING($spot_error)
	intel_chip_post_macro(0xf1)
	jmp	.Lhlt
#endif

6:
#ifndef NO_HLT
	/* If any memory errors occured hlt */
	testl	%esi, %esi
	jnz	5b
#endif
	mov %ebp, %esp

	RETSP

	.previous
