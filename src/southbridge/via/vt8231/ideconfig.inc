config_ide:
// This early setup switches IDE into compatibility mode before PCI gets 
// a chance to assign I/Os
	movl	$CONFIG_ADDR(0, 0x89, 0x42), %eax
//	movb	$0x09, %dl
	movb	$0x00, %dl
	PCI_WRITE_CONFIG_BYTE


