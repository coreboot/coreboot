	/* Dump the first 64 longs for devfn 0, bus 0 
	 * i.e. the north bridge. 
	 */
	
	jmp	dumpnorth_skip
	.section ".rom.data"

dn_banner:	.string "dump northbridge: \r\n"
dn_done:	.string "Done.\r\n"
dn_before:      .string "Before setting values: \r\n"
dn_after:       .string "After setting values: \r\n" 
#define NORTHBRIDGE_DEVFN 0xc000
#define NORTHBRIDGE1_DEVFN 0xc100
#define M1535_DEVFN 0x3800
#define M7101_DEVFN 0x8800
//#define M7101_DEVFN 0x3000

	.previous
	

	dumpnorth:
	mov %esp, %ebp
	CONSOLE_INFO_TX_STRING($dn_banner)
	movl	$NORTHBRIDGE_DEVFN, %ecx
//	xorl     %ecx, %ecx
1:
	CONSOLE_INFO_TX_HEX32(%ecx)
	CONSOLE_INFO_TX_CHAR($'-')
	movl	%ecx, %eax

	PCI_READ_CONFIG_DWORD
	CONSOLE_INFO_TX_HEX32(%eax)
	CONSOLE_INFO_TX_CHAR($'\r')
	CONSOLE_INFO_TX_CHAR($'\n')
	addl    $0x4, %ecx
	cmpb	$0, %cl
	jne	1b
	CONSOLE_INFO_TX_STRING($dn_done)


	movl	$NORTHBRIDGE1_DEVFN, %ecx
2:
	CONSOLE_INFO_TX_HEX32(%ecx)
	CONSOLE_INFO_TX_CHAR($'-')
	movl	%ecx, %eax

	PCI_READ_CONFIG_DWORD
	CONSOLE_INFO_TX_HEX32(%eax)
	CONSOLE_INFO_TX_CHAR($'\r')
	CONSOLE_INFO_TX_CHAR($'\n')
	addl    $0x4, %ecx
	cmpb	$0, %cl
	jne	2b
	CONSOLE_INFO_TX_STRING($dn_done)

	movl	$M1535_DEVFN, %ecx
3:
	CONSOLE_INFO_TX_HEX32(%ecx)
	CONSOLE_INFO_TX_CHAR($'-')
	movl	%ecx, %eax

	PCI_READ_CONFIG_DWORD
	CONSOLE_INFO_TX_HEX32(%eax)
	CONSOLE_INFO_TX_CHAR($'\r')
	CONSOLE_INFO_TX_CHAR($'\n')
	addl    $0x4, %ecx
	cmpb	$0, %cl
	jne	3b
	CONSOLE_INFO_TX_STRING($dn_done)

	movl	$M7101_DEVFN, %ecx
4:
	CONSOLE_INFO_TX_HEX32(%ecx)
	CONSOLE_INFO_TX_CHAR($'-')
	movl	%ecx, %eax

	PCI_READ_CONFIG_DWORD
	CONSOLE_INFO_TX_HEX32(%eax)
	CONSOLE_INFO_TX_CHAR($'\r')
	CONSOLE_INFO_TX_CHAR($'\n')
	addl    $0x4, %ecx
	cmpb	$0, %cl
	jne	4b
	CONSOLE_INFO_TX_STRING($dn_done)

	mov %ebp, %esp
	RETSP
dumpnorth_skip:
