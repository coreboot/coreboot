jmp cpu_reset_out

#ifdef DEBUG
cpu_reset_str:		.string "cpu_reset\r\n";
cpu_size_set_str:	.string	"cpu memory size set\r\n";
#endif


__cpu_reset:
#ifdef DEBUG
	TTYS0_TX_STRING($cpu_reset_str);
#endif /* DEBUG */

	CALLSP(set_memory_size)

#ifdef DEBUG
	TTYS0_TX_STRING($cpu_size_set_str);
#endif /* DEBUG */

#ifdef SMP
	/* Test to see if we are the boot strap processor.
	 * If so the boot must be complete.
	 */
	movl	$0x1b, %ecx
	rdmsr
	testl	$0x100, %eax
	jnz	__reboot

	/* Fixed mtrrs are enabled by the C code so if they
	 * aren't enabled yet we must be a secondary
	 * processor initializing in an SMP system.
	 */
	mov	$MTRRdefType_MSR, %ecx
	rdmsr
	testl	$0x400, %eax
	jnz	__reboot
	
	/* set the stack pointer */
	movl	$_estack, %esp
	movl	$APIC_DEFAULT_BASE, %edi
	movl	APIC_ID(%edi), %eax
	shrl	$24, %eax
	movl	$STACK_SIZE, %ebx
	mull	%ebx
	subl	%eax, %esp

	call	EXT(secondary_cpu_init)
	/* Fall through in case we somehow return */
#endif /* SMP */

__reboot:
	movl	$0xffffffff, %ebp
	jmp	__main

cpu_reset_out:
