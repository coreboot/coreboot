jmp	ecc_ram_initialize

ecc_ram_1:		.string "ecc_ram_1\r\n"
ecc_ram_2:		.string "ecc_ram_2\r\n"
ecc_ram_3:		.string "ecc_ram_3\r\n"
ecc_ram_4:		.string "ecc_ram_4\r\n"

ecc_ram_initialize:
	TTYS0_TX_STRING($ecc_ram_1)
	CALL_LABEL(get_ecc_ram_size_bytes_ebx)

	/* If we don't have an ECC SDRAM size skip the zeroing */
	testl	%ebx, %ebx
	jz	zero_ecc_ram_out
	movl	%ebx, %ebp

	/* Compute the next greater power of two memory size, to use in the mtrrs */
	bsrl	%ebp, %ecx
	movl	$1, %esi
	shll	%cl, %esi
	/* See if I need to round up */
	subl	$1, %esi
	testl	%esi, %ebp
	jz	1f
	incl	%ecx
1:	movl	$1, %esi
	shll	%cl, %esi

	/* Set caching on all of memory into write-combining mode. 
	 * So we can zero it quickly.
	 */
	/* Disable the cache while we set up a new MTRR over memory */
	movl	%cr0, %eax
	orl	$0x40000000, %eax
	movl	%eax, %cr0

	movl	$0x200, %ecx		/* mtrr[0] physical base register */
	movl	$0x00000000, %edx
	movl	$0x00000001, %eax
	wrmsr

	movl	$0x201, %ecx		/* mtrr[0] physical mask register */
	movl	$0x0000000f, %edx
	xorl	%eax, %eax
	subl	%esi, %eax
	andl	$0xfffff000, %eax
	orl	$0x800, %eax
	wrmsr

	/* Reenable the cache now that the mtrr is set up */
	movl	%cr0, %eax
	andl	$0x9fffffff, %eax
	movl	%eax, %cr0

	/* Now zero the memory */
	TTYS0_TX_STRING($ecc_ram_2)
	cld

#if !defined(HAVE_PC80_MEMORY_HOLE)
	/* The 640KB - 1MB memory should not be enabled at this point. */
	xorl	%eax, %eax
	xorl	%edi, %edi
	movl	%ebp, %ecx
	shrl	$2, %ecx
	rep	stosl

#else /* HAVE_PC80_MEMORY_HOLE */

	xorl	%eax, %eax		/* zero */
	xorl	%edi, %edi		/* destination */
	movl	$0x28000,%ecx
	rep	stosl

	xorl	%eax, %eax
	movl	$0x100000, %edi
	movl	%ebp, %ecx
	subl	%edi, %ecx
	shrl	$2, %ecx
	rep	stosl
#endif /* HAVE_PC80_MEMORY_HOLE */

	TTYS0_TX_STRING($ecc_ram_3)

	/* Change caching on memory from write-combining to write-back. */
	/* Disable the cache while we set up a new MTRR over memory */
	movl	%cr0, %eax
	orl	$0x40000000, %eax
	movl	%eax, %cr0

	movl	$0x200, %ecx
	movl	$0x00000000, %edx
	movl	$0x00000006, %eax
	wrmsr

	/* Reenable the cache now that the mtrr is set up */
	movl	%cr0, %eax
	andl	$0x9fffffff, %eax
	movl	%eax, %cr0

zero_ecc_ram_out:
	TTYS0_TX_STRING($ecc_ram_4)

