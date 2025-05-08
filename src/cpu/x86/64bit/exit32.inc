/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * For dropping from long mode to protected mode.
 *
 * For reference see "AMD64 ArchitectureProgrammer's Manual Volume 2",
 * Document 24593-Rev. 3.31-July 2019 Chapter 5.3
 *
 * Clobbers: rax, rbx, rcx, rdx
 */
.code64

#include <cpu/x86/gdt.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/cr.h>

drop_longmode:
#if !ENV_CACHE_AS_RAM
	/* Ensure cache is clean. */
	wbinvd
#endif
	/* Set  32-bit code segment and ss */
	mov	$GDT_CODE_SEG, %rcx
	/* SetCodeSelector32 will drop us to protected mode on return */
	call	SetCodeSelector32

	/* Skip SetCodeSelector32 */
.code32
	jmp	__longmode_compatibility

.align 8
.code64
SetCodeSelector32:
	# pop the return address from stack
	pop	%rbx

	# save rsp because we need to push it after ss
	mov	%rsp, %rdx

	# use iret to jump to a 32-bit offset in a new code segment
	# iret will pop cs:rip, flags, then ss:rsp
	mov	%ss, %ax	# need to push ss, but push ss instruction
	push	%rax		# not valid in x64 mode, so use ax
	push	%rdx		# the rsp to load
	pushfq			# push rflags
	push	%rcx		# cx is code segment selector from caller
	push	%rbx		# push the IP for the next instruction

	# the iretq will behave like ret, with the new cs/ss value loaded
	iretq

.align 4
.code32
__longmode_compatibility:
	/* Running in 32-bit compatibility mode */

	/* Use flat data segment */
	movl	$GDT_DATA_SEG, %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %ss
	movl	%eax, %fs

	/* Disable paging. */
	movl	%cr0, %eax
	andl	$(~CR0_PG), %eax
	movl	%eax, %cr0

	/* Disable long mode. */
	movl	$(IA32_EFER), %ecx
	rdmsr
	andl	$(~EFER_LME), %eax
	wrmsr

	/* Disable PAE. */
	movl	%cr4, %eax
	andl	$(~CR4_PAE), %eax
	movl	%eax, %cr4

	/* Clear page table register */
	xor	%eax, %eax
	movl	%eax, %cr3

__longmode_exit:
