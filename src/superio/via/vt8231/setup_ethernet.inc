/*
 * This early setup is a handy place to enable the Ethernet if the user wants
 * it.
 */
#if ENABLE_VT8231_LAN
enable_eth:
	movl	CONFIG_ADDR(0, SUPERIO_DEVFN, 0x51), %eax
	movb	$0x10, %dl
	PCI_WRITE_CONFIG_BYTE
#endif

