	/* Dump the first 64 longs for devfn 0, bus 0 
	 * i.e. the north bridge. 
	 */
	
	jmp	dumpnorth_skip
	.section ".rodata"

dn_banner:	.string "dump northbridge: \r\n"
dn_done:	.string "Done.\r\n"
dn_before:      .string "Before setting values: \r\n"
dn_after:       .string "After setting values: \r\n" 
	
	.text
	dumpnorth:
	mov %esp, %ebp
	CONSOLE_INFO_TX_STRING($dn_banner)
	xorl     %ecx, %ecx
1:
	CONSOLE_INFO_TX_HEX32(%ecx)
	CONSOLE_INFO_TX_CHAR($'-')
	movl	%ecx, %eax

	PCI_READ_CONFIG_DWORD
	CONSOLE_INFO_TX_HEX32(%eax)
	CONSOLE_INFO_TX_CHAR($'\r')
	CONSOLE_INFO_TX_CHAR($'\n')
	addl    $0x4, %ecx
	cmpl	$256, %ecx
	jne	1b
	CONSOLE_INFO_TX_STRING($dn_done)
	mov %ebp, %esp
	RETSP
dumpnorth_skip:
