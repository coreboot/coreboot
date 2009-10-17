	/* preserve BIST in %eax */
__fpu_start:
	movl %eax, %ebp

	/* Disable floating point emulation */
	movl	%cr0, %eax
	andl	$~(1<<2), %eax
	movl	%eax, %cr0

	movl %ebp, %eax
