	/*
	 * Put the processor back into a reset state
	 * with respect to the xmm registers.
	 */
	
	pxor %xmm0, %xmm0
	pxor %xmm1, %xmm1
	pxor %xmm2, %xmm2
	pxor %xmm3, %xmm3
	pxor %xmm4, %xmm4
	pxor %xmm5, %xmm5
	pxor %xmm6, %xmm6
	pxor %xmm7, %xmm7

	/* Disable floating point emulation */
	movl	%cr0, %eax
	andl	$~(1<<2), %eax
	movl	%eax, %cr0

	/* Disable sse instructions */
	movl	%cr4, %eax
	andl	$~(3<<9), %eax
	movl	%eax, %cr4
