jmp cpu_reset_out
#include <cpu/p6/mtrr.h>
#include <cpu/p6/apic.h>
#include <loglevel.h>


.section ".rom.data"

#if ASM_CONSOLE_LOGLEVEL >= BIOS_SPEW
cpu_reset_str:		.string "cpu_reset\r\n";
cpu_apic_str:		.string "apic: ";
cpu_size_set_str:	.string	"cpu memory size set\r\n";
#endif
.previous


__cpu_reset:
	CONSOLE_SPEW_TX_STRING($cpu_reset_str)

#ifdef SMP
	/* Enable the local apic, and map it where we expext it */
	movl	$APIC_BASE_MSR, %ecx
	rdmsr
	orl	$APIC_BASE_MSR_ENABLE, %eax
	andl	$(~APIC_BASE_MSR_ADDR_MASK), %eax
	orl	$APIC_DEFAULT_BASE, %eax
	/* xorl	%edx, %edx */
	wrmsr

	/* Get the apic_id */
	movl	(APIC_ID + APIC_DEFAULT_BASE), %edi
	shrl	$24, %edi

	CONSOLE_SPEW_TX_STRING($cpu_apic_str)
	CONSOLE_SPEW_TX_HEX32(%edi)
	CONSOLE_SPEW_TX_CHAR($'\r')
	CONSOLE_SPEW_TX_CHAR($'\n')
#endif

	CALLSP(set_memory_size)

	CONSOLE_SPEW_TX_STRING($cpu_size_set_str);

#ifdef SMP
	/* Test to see if we are the boot strap processor.
	 * If so the boot must be complete.
	 */
	movl	$APIC_BASE_MSR, %ecx
	rdmsr
	testl	$APIC_BASE_MSR_BOOTSTRAP_PROCESSOR, %eax
	jnz	__reboot

	/* Fixed mtrrs are enabled by the C code so if they
	 * aren't enabled yet we must be a secondary
	 * processor initializing in an SMP system.
	 */
	mov	$MTRRdefType_MSR, %ecx
	rdmsr
	testl	$0x400, %eax
	jnz	__reboot

	/* Get the apic_id */
	movl	(APIC_ID + APIC_DEFAULT_BASE), %edi
	shrl	$24, %edi

	/* Get the cpu index (MAX_CPUS on error) */
	movl	$-4, %ebx
1:	addl	$4, %ebx
	cmpl	$(MAX_CPUS << 2), %ebx
	je	2
	cmpl	%edi, EXT(initial_apicid)(%ebx)
	jne	1b
2:	shrl	$2, %ebx
	
	/* set the stack pointer */
	movl	$_estack, %esp
	movl	%ebx, %eax
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


