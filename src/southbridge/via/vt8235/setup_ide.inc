config_ide:
	movl	$CONFIG_ADDR(0, 0x88, 0x50), %eax
	movb	$0x00, %dl
	PCI_WRITE_CONFIG_BYTE

	movl	$CONFIG_ADDR(0, 0x89, 0x04), %eax
	movb	$0x07, %dl
	PCI_WRITE_CONFIG_BYTE

	movl	$CONFIG_ADDR(0, 0x89, 0x40), %eax
	movb	$0x03, %dl
	PCI_WRITE_CONFIG_BYTE

// This early setup switches IDE into compatibility mode before PCI gets 
// a chance to assign I/Os
#if (!ENABLE_IDE_NATIVE_MODE)
	movl	$CONFIG_ADDR(0, 0x89, 0x42), %eax
//	movb	$0x09, %dl
	movb	$0x00, %dl
	PCI_WRITE_CONFIG_BYTE

	movl	$CONFIG_ADDR(0, 0x89, 0x3c), %eax
	movb	$0x0e, %dl
	PCI_WRITE_CONFIG_BYTE

	movl	$CONFIG_ADDR(0, 0x89, 0x3d), %eax
	movb	$0x00, %dl
	PCI_WRITE_CONFIG_BYTE
#endif
