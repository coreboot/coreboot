#if 0
	movl	$0x000f0000, %eax
	movl	$0x000f1000, %ebx
	CALLSP(ramtest)
#endif

#if 1
	movl	$0x00100000, %eax
	movl	$0x00180000, %ebx
	CALLSP(ramtest)

	xorl	%eax, %eax
	movl	$0x00080000, %ebx
	CALLSP(ramtest)
#endif

#if 1

	xorl	%eax, %eax
	movl	$0x00001000, %ebx
	CALLSP(ramtest)
#endif

#if 0
	movl	$0xffe00000, %eax
	movl	$0xfff00000, %ebx
	CALLSP(ramtest)
#endif
