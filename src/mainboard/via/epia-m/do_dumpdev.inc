	movl	$0x0, %ecx
	CALLSP(dumpdev)

	movl	$CONFIG_ADDR(0,0x88,0), %ecx
	CALLSP(dumpdev)

	movl	$CONFIG_ADDR(0,0x89,0), %ecx
	CALLSP(dumpdev)
