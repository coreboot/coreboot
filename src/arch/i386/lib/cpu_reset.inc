jmp cpu_reset_out

__cpu_reset:
	movl	$0xffffffff, %ebp
	jmp	__main

cpu_reset_out:


