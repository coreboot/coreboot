	/* Save the BIST result */
	movl	%eax, %ebp
	
	/*
	 * Enabling mmx registers is a noop
	 * Enable the use of the xmm registers
	 */

        /* Enable sse instructions */
        movl    %cr4, %eax
        orl     $(1<<9), %eax
        movl    %eax, %cr4

	/* Disable floating point emulation */
	movl	%cr0, %eax
	andl	$~(1<<2), %eax
	movl	%eax, %cr0

        /* enable sse extension */
        movl    %cr0, %eax
        andl    $~(1<<1), %eax
        movl    %eax, %cr0

	/* Restore the BIST result */
	movl	%ebp, %eax
