	/* Dump the first 64 longs for devfn 0, bus 0 
	 * i.e. the north bridge. 
	 */
	
#define CS_WRITE_BYTE(addr, byte) \
	movl $addr, %eax	; \
	movl $byte, %edx	; \
	PCI_WRITE_CONFIG_BYTE

#define CS_WRITE_WORD(addr, word) \
	movl $addr, %eax	; \
	movl $word, %ecx	; \
	PCI_WRITE_CONFIG_WORD

#define CS_WRITE_LONG(addr, dword) \
	movl $addr, %eax	; \
	movl $dword, %ecx	; \
	PCI_WRITE_CONFIG_DWORD
	
#define DEVFN(device, function) (((device) << 3) + (function))
#ifndef CONFIG_ADDR
#define CONFIG_ADDR(bus,devfn,where) (((bus) << 16) | ((devfn) << 8) | (where))
#endif

	jmp	dumpdev_skip

dd_banner:	.string "dump device: "
dd_ret:		.string "\r\n"
dd_done:	.string "Done.\r\n"
dd_before:      .string "Before setting values: \r\n"
dd_after:       .string "After setting values: \r\n" 

# expects device devfn in %ecx

dumpdev:
	mov %esp, %ebp
	CONSOLE_INFO_TX_STRING($dd_banner)
	CONSOLE_INFO_TX_HEX32(%ecx)
	CONSOLE_INFO_TX_STRING($dd_ret)
#	xorl     %ecx, %ecx
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

	CONSOLE_INFO_TX_STRING($dd_ret)
#	CONSOLE_INFO_TX_CHAR($'\r')
#	CONSOLE_INFO_TX_CHAR($'\n')
	cmpb	$0, %cl
	jne	1b
	CONSOLE_INFO_TX_STRING($dd_done)
	mov %ebp, %esp
	RETSP
dumpdev_skip:
