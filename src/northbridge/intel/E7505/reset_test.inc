#define MCH_DRC 0x7c
#define DRC_DONE (1 << 29)
	/* If I have already booted once skip a bunch of initialization */
	/* To see if I have already booted I check to see if memory
	 * has been enabled.
	 */
	movl	$MCH_DRC, %eax
	PCI_READ_CONFIG_DWORD
	testl	$DRC_DONE, %eax
	setnz	%al
