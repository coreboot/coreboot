	/*
	 * Put the processor back into a reset state
	 * with respect to the xmm registers.
	 */
	
	xorps %xmm0, %xmm0
	xorps %xmm1, %xmm1
	xorps %xmm2, %xmm2
	xorps %xmm3, %xmm3
	xorps %xmm4, %xmm4
	xorps %xmm5, %xmm5
	xorps %xmm6, %xmm6
	xorps %xmm7, %xmm7

	/* Disable sse instructions */
	movl	%cr4, %eax
	andl	$~(3<<9), %eax
	movl	%eax, %cr4
