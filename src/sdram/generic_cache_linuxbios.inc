	/* enable caching for 0 - 16M using a variable mtrr 
	 * This should be all we need to get good performance out of LinuxBIOS.
	 */

	/* Disable the cache while we set up a new MTRR over memory */
	movl	%cr0, %eax
	orl	$0x40000000, %eax
	movl	%eax, %cr0

	movl	$0x200, %ecx		/* mtrr[0] physical base register */
	movl	$0x00000000, %edx
	movl	$0x00000006, %eax
	wrmsr

	movl	$0x201, %ecx		/* mtrr[0] physical mask register */
	movl	$0x0000000f, %edx
	movl	$0xff000800, %eax
	wrmsr

	/* Reenable the cache now that the mtrr is set up */
	movl	%cr0, %eax
	andl	$0x9fffffff, %eax
	movl	%eax, %cr0

