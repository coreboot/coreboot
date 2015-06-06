/* Store the initial timestamp for booting in mmx registers. This works
 * because the bootblock isn't being compiled with MMX support so mm0 and
 * mm1 will be preserved into romstage. */
	.code32

.global stash_timestamp
stash_timestamp:

	/* Save the BIST value */
	movl	%eax, %ebp

	finit
	rdtsc
	movd	%eax, %mm0
	movd	%edx, %mm1

	/* Restore the BIST value to %eax */
	movl	%ebp, %eax
