	/* preserve BIST in %eax */
	movl %eax, %ebp

	/*
	 * Enable the use of the xmm registers
	 */

	/* Enable sse instructions */
	movl	%cr4, %eax
	orl	$(1<<9), %eax
	movl	%eax, %cr4

	movl	%ebp, %eax

