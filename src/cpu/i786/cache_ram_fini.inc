#if defined(CACHE_RAM_BASE) && defined(CACHE_RAM_SIZE)
	/* Note:  We cannot be running from simulated ram in
	 * this code.  If we are evil things will happen.
	 */

	/* Disable the cache */
	movl	%cr0, %eax
	orl	$0x40000000, %eax
	movl	%eax, %cr0

	/* Flush everything that is left in the cache,
	 * We don't want random writes to memory to occur later on.
	 */
	invd

	/* Disable the cache ram mtrr */
	movl	$0x204, %ecx
	xorl	%eax, %eax
	xorl	%edx, %edx
	wrmsr

	movl	$0x205, %ecx
	xorl	%eax, %eax
	xorl	%edx, %edx
	wrmsr

	/* Reenable the cache now that the mtrr is cleared */
	movl	%cr0, %eax
	andl	$0x9fffffff, %eax
	movl	%eax, %cr0

	/* Reload the normal data segments */
	movw	$0x18, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs
#endif
