jmp set_memory_size_out

#include <cpu/k7/mtrr.h>
set_memory_size:
	/* Read the top of memory */
	movl	$0x9c, %eax
	PCI_READ_CONFIG_DWORD

	/* Set the Athlon Top of memory MSR */
	xorl	%edx, %edx
	movl	$TOP_MEM, %ecx
	wrmsr
	movl	$TOP_MEM2, %ecx
	wrmsr

	/* Clear the IOrr's */
	xorl	%edx, %edx
	xorl	%eax, %eax
	movl	$IORR0_BASE, %ecx
	wrmsr
	movl	$IORR0_MASK, %ecx
	wrmsr
	movl	$IORR1_BASE, %ecx
	wrmsr
	movl	$IORR1_MASK, %ecx
	wrmsr

set_memory_size_end:
	RETSP
set_memory_size_out:
