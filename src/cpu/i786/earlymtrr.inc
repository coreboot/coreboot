#include <cpu/p6/mtrr.h>

/* The fixed and variable MTRRs are powered-up with random values, clear them to
 * MTRR_TYPE_UNCACHABLE for safty reason 
 */

earlymtrr_start:
	xorl	%eax, %eax			# clear %eax and %edx
	xorl	%edx, %edx			#
	movl	$fixed_mtrr_msr, %esi

clear_fixed_var_mtrr:
	lodsl	(%esi), %eax
	testl	%eax, %eax
	jz	clear_fixed_var_mtrr_out

	movl	%eax, %ecx
	xorl	%eax, %eax
	wrmsr

	jmp	clear_fixed_var_mtrr
clear_fixed_var_mtrr_out:

#ifdef MEMORY_HOLE
set_fixed_mtrr:
	/* enable Write Back Cache for 0-640KB */
	movl	$MTRRfix64K_00000_MSR, %ecx
	rdmsr
	movl	$0x06060606, %edx
	movl	$0x06060606, %eax
	wrmsr

	movl	$MTRRfix16K_80000_MSR, %ecx
	rdmsr
	movl	$0x06060606, %edx
	movl	$0x06060606, %eax
	wrmsr
#endif /* MEMORY_HOLE */

set_var_mtrr:
#if 0
	/* enable caching for 0 - 128MB using variable mtrr */
	movl	$0x200, %ecx
	rdmsr
	andl	$0xfffffff0, %edx
	orl	$0x00000000, %edx
	andl	$0x00000f00, %eax
	orl	$0x00000006, %eax
	wrmsr

	movl	$0x201, %ecx
	rdmsr
	andl	$0xfffffff0, %edx
	orl	$0x0000000f, %edx
	andl	$0x000007ff, %eax
	orl	$0xf0000800, %eax
	wrmsr
#endif

#if defined(XIP_ROM_SIZE) && defined(XIP_ROM_BASE)
	/* enable write protect caching so we can do execute in place
	 * on the flash rom.
	 */
	movl	$0x202, %ecx
	xorl	%edx, %edx
	movl	$(XIP_ROM_BASE | 0x005), %eax
	wrmsr	

	movl	$0x203, %ecx
	movl	$0x0000000f, %edx
	movl	$(~(XIP_ROM_SIZE - 1) | 0x800), %eax
	wrmsr
#endif /* XIP_ROM_SIZE && XIP_ROM_BASE */

enable_mtrr:	
	/* Set the default memory type and enable fixed and variable MTRRs */
	movl	$0x2ff, %ecx
	xorl	%edx, %edx
#ifdef MEMORY_HOLE
	/* Enable Fixed and Variable MTRRs */
	movl	$0x00000c00, %eax
#else
	/* Enable Variable MTRRs */
	movl	$0x00000800, %eax
#endif /* MEMORY_HOLE */
	wrmsr


	/* enable cache */
	movl	%cr0, %eax
	andl	$0x9fffffff,%eax
	movl	%eax, %cr0

	jmp	earlymtrr_end

fixed_mtrr_msr:	
	.long	0x250, 0x258, 0x259
	.long	0x268, 0x269, 0x26A
	.long	0x26B, 0x26C, 0x26D
	.long	0x26E, 0x26F
var_mtrr_msr:
	.long	0x200, 0x201, 0x202, 0x203
	.long	0x204, 0x205, 0x206, 0x207
	.long	0x208, 0x209, 0x20A, 0x20B
	.long	0x20C, 0x20D, 0x20E, 0x20F
	.long	0x000 /* NULL, end of table */
	
earlymtrr_end:
