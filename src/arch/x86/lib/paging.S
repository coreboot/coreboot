#if CONFIG_PGE==1
#include <cpu/x86/cr.h>

	jmp code

	.section .rom.data
#include "pagetables.S"
	.previous

code:
	movl	$pml1, %ecx		/* load address of page directory */
	movl	%ecx, %cr3
	jmp	1f
1:
	movl	%cr4, %edx
	orl	$CR4_PSE, %edx			/* 4m pages */
	movl	%edx, %cr4

	movl	%cr0, %edx
	orl	$CR0_PG, %edx

	movl	$paging, %eax
	movl	%edx, %cr0
	jmp	*%eax
paging:
#endif

