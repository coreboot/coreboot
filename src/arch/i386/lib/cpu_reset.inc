/* -*- asm -*- */

jmp cpu_reset_out

__cpu_reset:
	/* set the boot_complete flag */
	movl	$0xffffffff, %ebp
	jmp	__main

cpu_reset_out:


