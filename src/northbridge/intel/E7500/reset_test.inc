#define MCH_RICM 0x94
#define RICM_DONE (1 << 27)
	/* If I have already booted once skip a bunch of initialization */
	/* To see if I have already booted I check to see if memory
	 * has been enabled.
	 */
	movl	$MCH_RICM, %eax
	PCI_READ_CONFIG_DWORD
	testl	$RICM_DONE, %eax
	setnz	%al
