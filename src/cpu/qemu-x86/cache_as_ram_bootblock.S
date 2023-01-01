/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/post_code.h>

#define CBFS_FILE_MAGIC 0
#define CBFS_FILE_LEN (CBFS_FILE_MAGIC + 8)
#define CBFS_FILE_TYPE (CBFS_FILE_LEN + 4)
#define CBFS_FILE_CHECKSUM (CBFS_FILE_TYPE + 4)
#define CBFS_FILE_OFFSET (CBFS_FILE_CHECKSUM + 4)

.section .init, "ax", @progbits
.code32

.global bootblock_pre_c_entry
bootblock_pre_c_entry:

cache_as_ram:
	post_code(POST_BOOTBLOCK_CAR)
	/*
	 * Nothing to do here on qemu, RAM works just fine without any
	 * initialization.
	 */

	/* Clear the cache memory region. This will also clear CAR GLOBAL */
	movl	$_car_region_start, %edi
	movl	$_car_region_end, %ecx
	sub	%edi, %ecx
	shr	$2, %ecx
	xorl	%eax, %eax
	rep	stosl

	post_code(0x21)

#if defined(__x86_64__)
	/*
	 * Copy page tables to final location in DRAM. This prevents some strange
	 * bugs when running KVM enabled:
	 * Accessing MMX instructions in long mode causes an abort
	 * Some physical addresses aren't properly translated
	 * Emulation fault on every instruction fetched due to page tables in ROM
	 * Enabling or disabling paging causes a fault
	 *
	 * First, find page tables in CBFS:
	 */
	lea	pagetables_name, %esi
	mov	$1f, %esp
	jmp	walkcbfs_asm
1:
	cmpl	$0, %eax
	je	.Lhlt

	/* Test if page tables are memory-mapped and skip relocation */
	cmpl	$(CONFIG_ARCH_X86_64_PGTBL_LOC), %eax
	je	pages_done

	movl	CBFS_FILE_OFFSET(%eax), %ebx
	bswap	%ebx
	addl	%eax, %ebx
	movl	%ebx, %esi

	movl	CBFS_FILE_LEN(%eax), %ecx
	bswap	%ecx
	shr	$2, %ecx

	movl	$(CONFIG_ARCH_X86_64_PGTBL_LOC), %edi

loop:
	movl	(%esi), %eax
	movl	%eax, (%edi)
	addl	$4, %esi
	addl	$4, %edi
	decl	%ecx
	jnz	loop
pages_done:
#endif

	movl	$_ecar_stack, %esp

	/* Align the stack and keep aligned for call to bootblock_c_entry() */
	and	$0xfffffff0, %esp

	/* entry64.inc preserves ebx. */
#include <cpu/x86/64bit/entry64.inc>

	/* Restore the BIST result and timestamps. */
#if ENV_X86_64
	movd	%mm2, %rdi
	shlq	$32, %rdi
	movd	%mm1, %rsi
	or	%rsi, %rdi

	movd	%mm0, %rsi
#else
	sub	$4, %esp

	movd	%mm0, %ebx
	movd	%mm1, %eax
	movd	%mm2, %edx

	pushl	%ebx
	pushl	%edx
	pushl	%eax
#endif

before_c_entry:
	call	bootblock_c_entry_bist
	/* Never returns */
.Lhlt:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	.Lhlt

pagetables_name:
        .string "pagetables"
