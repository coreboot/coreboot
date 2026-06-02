/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/post_codes.h>
#include <cpu/x86/64bit/entry64.inc>
#include <cpu/x86/post_code.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>

.section .init, "ax", @progbits

.global bootblock_resume_entry
bootblock_resume_entry:
	post_code(POSTCODE_BOOTBLOCK_RESUME_ENTRY)

	/* Get an early timestamp */
	rdtsc
	movd	%eax, %mm1
	movd	%edx, %mm2

	/* Fall through to bootblock_pre_c_entry */

/*
 * on entry:
 *   mm0: BIST (ignored)
 *   mm2_mm1: timestamp
 */

.global bootblock_pre_c_entry
bootblock_pre_c_entry:
	post_code(POSTCODE_BOOTBLOCK_PRE_C_ENTRY)

	/*
	 * Configure the default memory type to cachable. This code runs from RAM and
	 * does not access MMIO, so it's safe to mark everything cachable.
	 */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	and	$(~0x0cff), %ax
	orl	$(MTRR_DEF_TYPE_EN | MTRR_TYPE_WRBACK), %eax
	wrmsr

	/* Enable cache (CR0.CD = 0, CR0.NW = 0). */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	movl	%eax, %cr0

#if ENV_X86_64
	setup_longmode $PML4E
#endif

	/* Clear .bss section */
	cld
	xor	%eax, %eax
	movl	$(_ebss), %ecx
	movl	$(_bss), %edi
	sub	%edi, %ecx
	shrl	$2, %ecx
	rep	stosl

	movl	$_eearlyram_stack, %esp

	/* Copy .data section content */
#include <cpu/x86/copy_data_section.inc>

#if ENV_X86_64
	/* Disable cache (CR0.CD = 1, CR0.NW = 1). */
	mov	%cr0, %rax
	or	$((CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	mov	%rax, %cr0
#else
	/* Disable cache (CR0.CD = 1, CR0.NW = 1). */
	mov	%cr0, %eax
	or	$((CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	mov	%eax, %cr0
#endif

	/* Restore default memory type MSR. C code will set up proper MTRRs. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	and	$(~0x0cff), %ax
	wrmsr

	/* Align the stack for call to bootblock_c_entry(). */
	and	$0xfffffff0, %esp

#if ENV_X86_64
	movd	%mm2, %rdi
	shlq	$32, %rdi
	movd	%mm1, %rsi
	or	%rsi, %rdi
#else
	/* Keep stack aligned by accounting for the next to push instructions */
	sub	$8, %esp

	movd	%mm2, %eax
	pushl	%eax		/* tsc[63:32] */
	movd	%mm1, %eax
	pushl	%eax		/* tsc[31:0] */
#endif

	post_code(POSTCODE_BOOTBLOCK_PRE_C_DONE)

	call	bootblock_c_entry
	/* Never reached */

.halt_forever:
	post_code(POSTCODE_DEAD_CODE)
	hlt
	jmp	.halt_forever
