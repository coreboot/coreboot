/* Disable the TCO watch dog timer in the southbridge */
/* bridge 0, device 1f, function 0, byte d4, bit 1 */

	movl	$0x8000f8d4,%eax
	movl	$0x0cf8,%edx
	outl	%eax,%dx
	movl	$0x0cfc,%edx
	movb	$2,%al
	outb	%al,%dx 
