	/* Dump the first 64 longs for devfn 0, bus 0 
	 * i.e. the north bridge. 
	 */
	
	jmp	dumpnorth_skip
	.section ".rom.data"

dn_banner:	.string "dump northbridge: \r\n"
dn_done:	.string "Done.\r\n"
dn_before:      .string "Before setting values: \r\n"
dn_after:       .string "After setting values: \r\n" 
	.previous
	

dumpnorth:
	mov %esp, %ebp
	CONSOLE_INFO_TX_STRING($dn_banner)
	xorl     %ecx, %ecx
1:
	CONSOLE_INFO_TX_HEX8(%cl)
	CONSOLE_INFO_TX_CHAR($':')
	CONSOLE_INFO_TX_CHAR($' ')

2:
	movl	%ecx, %eax
	PCI_READ_CONFIG_BYTE
	CONSOLE_INFO_TX_HEX8(%al)
	CONSOLE_INFO_TX_CHAR($' ')
	
	incl	%ecx
	testb	$0xf, %cl
	jnz	2b

	CONSOLE_INFO_TX_CHAR($'\r')
	CONSOLE_INFO_TX_CHAR($'\n')
	cmpl	$256, %ecx
	jne	1b
	CONSOLE_INFO_TX_STRING($dn_done)
	mov %ebp, %esp
	RETSP
dumpnorth_skip:
