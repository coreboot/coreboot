	/* copy data segment from FLASH ROM to CACHE */
	movl	$(EXT(_ldata) - CACHE_RAM_BASE), %esi
	movl	$EXT(_data), %edi
	movl	$(EXT(_eldata) - CACHE_RAM_BASE), %ecx
	subl	%esi, %ecx
	jz	1f			/* should not happen */
	rep
	movsb
1:

	/** clear bss */
	movl	$EXT(_bss), %edi
	movl	$EXT(_ebss), %ecx
	subl	%edi, %ecx
	jz	1f
	xorl	%eax, %eax
	rep
	stosb
1:

	/* set new stack */
	movl	$(_stack + STACK_SIZE), %esp

	call	cache_ram_start

