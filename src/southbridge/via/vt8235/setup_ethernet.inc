/*
 * This early setup is a handy place to enable the Ethernet if the user wants
 * it.
 */
enable_eth:
	movl	$CONFIG_ADDR(0, SUPERIO_DEVFN, 0x50), %eax
	PCI_READ_CONFIG_DWORD

	/* Turn on just the Ethernet and disable modem */
	orl	$0x1080, %eax
	mov	%eax, %ecx

	movl	$CONFIG_ADDR(0, SUPERIO_DEVFN, 0x50), %eax
	PCI_WRITE_CONFIG_DWORD
