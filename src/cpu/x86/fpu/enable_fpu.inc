	/* preserve BIST in %eax */
	movl %eax, %ebp

	/* Disable floating point emulation */
	movl	%cr0, %eax
	andl	$~(1<<2), %eax
	movl	%eax, %cr0

	movl %ebp, %eax
