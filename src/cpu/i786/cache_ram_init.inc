#if defined(CACHE_RAM_BASE) && defined(CACHE_RAM_SIZE)
	/* Disable the cache while we set up the cache ram MTRR */
	movl	%cr0, %eax
	orl	$0x40000000, %eax
	movl	%eax, %cr0

	/* Set up an mtrr in write-back mode over some arbitrary
	 * location.  As long as we do not get a capacity miss,
	 * or a multiprocessor conflict miss this should allow us to
	 * function as if we have memory even when it hasn't been
	 * enabled yet.
	 */
	movl	$0x204, %ecx		/* mtrr[0] physical base register */
	xorl	%edx, %edx
	movl	$(CACHE_RAM_BASE | 0x006), %eax
	wrmsr

	movl	$0x205, %ecx		/* mtrr[0] physical mask register */
	movl	$0x0000000f, %edx
	movl	$(~(CACHE_RAM_SIZE - 1) | 0x800), %eax
	wrmsr

	/* Reenable the cache now that the mtrr is set up */
	movl	%cr0, %eax
	andl	$0x9fffffff, %eax
	movl	%eax, %cr0

	/* Force cache ram area into cache */
	movl	$CACHE_RAM_BASE, %esi
	movl	$(CACHE_RAM_BASE + CACHE_RAM_SIZE), %edi
1:	movl	(%esi), %eax
	addl	$4, %esi
	movl	%eax, (%esi)
	cmpl	%esi, %edi
	jnz	1b

	/* Load a different set of data segments */
	movw	$0x08, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
#endif