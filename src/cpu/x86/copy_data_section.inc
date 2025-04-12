/* SPDX-License-Identifier: GPL-2.0-only */

#if ENV_SEPARATE_DATA_AND_BSS

/*
 * Copy .data section content from _data_load to it's linked
 * address. Usually that's inside Cache-As-Ram.
 * This code can be included from 32 bits or 64 bits code.
 * It also preserves registers by using stack.
 */
copy_data_section:
#if ENV_X86_64
	push	%rcx
	push	%rdi
	push	%rsi
#else
	pushl	%ecx
	pushl	%edi
	pushl	%esi
#endif

	movl	$(_edata), %ecx
	movl	$(_data), %edi
	sub	%edi, %ecx
	movl	$(_data_load),%esi
	shrl	$2, %ecx
	rep	movsl

#if ENV_X86_64
	pop	%rsi
	pop	%rdi
	pop	%rcx
#else
	popl	%esi
	popl	%edi
	popl	%ecx
#endif

#endif	/* ENV_SEPARATE_DATA_AND_BSS */
