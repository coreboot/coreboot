
	/* Carefully print the failure if the built in self test did not pass */
	testl	%eax, %eax
	jnz	bist32_fail
