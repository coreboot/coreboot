	/*
	 * Enable the use of the xmm registers
	 */

	/* Disable floating point emulation */
	movl	%cr0, %eax
	andl	$~(1<<2), %eax
	movl	%eax, %cr0

	/* Enable sse instructions */
	movl	%cr4, %eax
	orl	$(1<<9), %eax
	movl	%eax, %cr4
