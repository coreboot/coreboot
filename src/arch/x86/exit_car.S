/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/mtrr.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/cache.h>

/* Place the stack in the bss section. It's not necessary to define it in the
 * the linker script. */
	.section .bss, "aw", @nobits
.global _stack
.global _estack
.global _stack_size

_stack:
.space CONFIG_STACK_SIZE
_estack:
.set _stack_size, _estack - _stack

.text
.global _start
_start:
	/* Assume stack alignment doesn't matter here as chipset_teardown_car
	   is expected to be implemented in assembly. */

	/* Migrate GDT to this text segment */
#if ENV_X86_64
	call	gdt_init64
#else
	call	gdt_init
#endif

#if ENV_X86_64
	mov	%rdi, %rax
	movabs %rax, _cbmem_top_ptr
#else
	/* The return argument is at 0(%esp), the calling argument at 4(%esp) */
	movl	4(%esp), %eax
	movl	%eax, _cbmem_top_ptr
#endif
	/* Make sure _cbmem_top_ptr hits dram before invd */
	movl	$1, %eax
	cpuid
	btl	$CPUID_FEATURE_CLFLUSH_BIT, %edx
	jnc	skip_clflush
#if ENV_X86_64
	movabs	$_cbmem_top_ptr, %rax
	clflush	(%rax)
#else
	clflush	_cbmem_top_ptr
#endif

skip_clflush:
	/* chipset_teardown_car() is expected to disable cache-as-ram. */
	call	chipset_teardown_car

	/* Enable caching if not already enabled. */
#if ENV_X86_64
	mov	%cr0, %rax
	and     $(~(CR0_CD | CR0_NW)), %eax
	mov	%rax, %cr0
#else
	mov	%cr0, %eax
	and	$(~(CR0_CD | CR0_NW)), %eax
	mov	%eax, %cr0
#endif
	/* Ensure cache is clean. */
	invd

	movl	$_estack, %esp
	/* Align stack to 16 bytes at call instruction. */
	andl	$0xfffffff0, %esp

	/* Call this in assembly as some platforms like to mess with the bootflow and
	   call into main directly from chipset_teardown_car. */
	call	postcar_mtrr_setup

	/* Call into main for postcar. */
	call main
	/* Should never return. */
1:
	jmp	1b
