	/* If I have already booted once skip a bunch of initialization */
	/* To see if I have already booted I check to see if memory
	 * has been enabled.
	 */
	movl	$0x58, %eax
	PCI_READ_CONFIG_DWORD
	testl $(1<<25), %eax
	jnz	__cpu_reset
