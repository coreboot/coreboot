#include <cpu/p6/mtrr.h>

/* The fixed and variable MTRRs are powered-up with random values, clear them to
 * MTRR_TYPE_UNCACHABLE for safty reason */
earlymtrr_start:
	xorl	%eax, %eax			# clear %eax and %edx
	xorl	%edx, %edx			#
	movl	$fixed_mtrr_msr, %esi

clear_fixed_var_mtrr:
	lodsw	(%esi), %ax
	testw	$0x00, %ax
	jz	set_fixed_mtrr

	movl	%eax, %ecx
	xorl	%eax, %eax
	wrmsr

	jmp	clear_fixed_var_mtrr

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

set_var_mtrr:
	/* enable caching for 0-2(or 4)MB using variable mtrr */
	mov	$0x200, %ecx
	rdmsr
	and	$0xfffffff0, %edx
	or	$0x00000000, %edx
	and	$0x00000f00, %eax
	or	$0x00000006, %eax
	wrmsr

	mov	$0x201, %ecx
	rdmsr
	and	$0xfffffff0, %edx
	or	$0x0000000f, %edx
	and	$0x000007ff, %eax
	or	$0xf0000800, %eax
	wrmsr

enable_mtrr:	
	/* Set the default memory type and enable fixed and variable MTRRs */
	movl	$0x2ff, %ecx
	rdmsr
	and	$0xfffff300, %eax
	/* Enable Fixed and Variable MTRRs */
	or	$0x00000c00, %eax
	wrmsr

	/* enable cache */
	mov	%cr0, %eax
	and	$0x9fffffff,%eax
	mov	%eax, %cr0

	jmp	earlymtrr_end

fixed_mtrr_msr:	
	.word	0x250, 0x258, 0x259
	.word	0x268, 0x269, 0x26A
	.word	0x26B, 0x26C, 0x26D
	.word	0x26E, 0x26F
var_mtrr_msr:
	.word	0x200, 0x201, 0x202, 0x203
	.word	0x204, 0x205, 0x206, 0x207
	.word	0x208, 0x209, 0x20A, 0x20B
	.word	0x20C, 0x20D, 0x20E, 0x20F
	.word	0x000 /* NULL, end of table */
	
earlymtrr_end:
