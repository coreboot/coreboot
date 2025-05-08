/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/gdt.h>
#include <cpu/x86/post_code.h>

/* Place the stack in the bss section. It's not necessary to define it in
 * the linker script. */
	.section .bss, "aw", @nobits
.global _stack
.global _estack
.global _stack_size

.align 16
_stack:
.space CONFIG_STACK_SIZE
_estack:
.set _stack_size, _estack - _stack

	.section ".text._start", "ax", @progbits
#if ENV_X86_64
	.code64
#else
	.code32
#endif
	.globl _start
_start:
	cli
#if ENV_X86_64
	movabs	$gdtaddr, %rax
	lgdt	(%rax)
#else
	lgdt	%cs:gdtaddr
	ljmp	$GDT_CODE_SEG, $1f
#endif
1:	movl	$GDT_DATA_SEG, %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %ss
	xor	%eax, %eax /* zero out the gs and fs segment index */
	movl	%eax, %fs
	movl	%eax, %gs /* Will be used for cpu_info */
#if ENV_X86_64
	mov	$GDT_CODE_SEG64, %ecx
	call	SetCodeSelector
#endif

	post_code(POSTCODE_ENTRY_C_START)		/* post 13 */

	cld

#if ENV_X86_64
	mov	%rdi, %rax
	movabs	%rax, _cbmem_top_ptr
	movabs	$_stack, %rdi
#else
	/* The return argument is at 0(%esp), the calling argument at 4(%esp) */
	movl	4(%esp), %eax
	movl	%eax, _cbmem_top_ptr
	leal	_stack, %edi
#endif

#if ENV_X86_64
	/** poison the stack. Code should not count on the
	 * stack being full of zeros. This stack poisoning
	 * recently uncovered a bug in the broadcast SIPI
	 * code.
	 */
	movabs	$_estack, %rcx
	sub	%rdi, %rcx
	shr	$3, %rcx   /* it is 64 bit aligned, right? */
	movq	$0xDEADBEEFDEADBEEF, %rax
	rep
	stosq

	/* Set new stack with enforced alignment. */
	movabs	$_estack, %rsp
	movq	$(0xfffffffffffffff0), %rax
	and	%rax, %rsp
#else
	/** poison the stack. Code should not count on the
	 * stack being full of zeros. This stack poisoning
	 * recently uncovered a bug in the broadcast SIPI
	 * code.
	 */
	movl	$_estack, %ecx
	subl	%edi, %ecx
	shrl	$2, %ecx   /* it is 32 bit aligned, right? */
	movl	$0xDEADBEEF, %eax
	rep
	stosl

	/* Set new stack with enforced alignment. */
	movl	$_estack, %esp
	andl	$(0xfffffff0), %esp
#endif

	/*
	 *	Now we are finished. Memory is up, data is copied and
	 *	bss is cleared.   Now we call the main routine and
	 *	let it do the rest.
	 */
	post_code(POSTCODE_PRE_HARDWAREMAIN)	/* post 6e */

#if CONFIG(ASAN_IN_RAMSTAGE)
	call asan_init
#endif

#if CONFIG(GDB_WAIT)
	call gdb_hw_init
	call gdb_stub_breakpoint
#endif
	call	main
	/* NOTREACHED */
.Lhlt:
	post_code(POSTCODE_DEAD_CODE)	/* post ee */
	hlt
	jmp	.Lhlt

#if CONFIG(GDB_WAIT)

	.globl gdb_stub_breakpoint
gdb_stub_breakpoint:
#if ENV_X86_64
	pop	%rax	/* Return address */
	pushfq
	mov	%cs, %rbx
	push	%rbx
	push	%rax	/* Return address */
	push	$0	/* No error code */
	push	$32	/* vector 32 is user defined */
#else
	popl	%eax	/* Return address */
	pushfl
	pushl	%cs
	pushl	%eax	/* Return address */
	pushl	$0	/* No error code */
	pushl	$32	/* vector 32 is user defined */
#endif
	jmp	int_hand
#endif

	.globl gdt, gdt_end
	.global per_cpu_segment_descriptors, per_cpu_segment_selector

gdtaddr:
	.word	gdt_end - gdt - 1
#if ENV_X86_64
	.quad	gdt
#else
	.long	gdt		/* we know the offset */
#endif

	.data

	/*
	 * This is the gdt for coreboot's ramstage.
	 * It is different from the gdt in ASM part of coreboot
	 * which is defined in gdt_init.S
	 *
	 * When the machine is initially started, we use a very simple
	 * gdt from ROM (that in gdt_init.S) which only contains those
	 * entries we need for protected mode and long mode.
	 *
	 * When we're executing code from RAM, we want to do more complex
	 * stuff, like initializing PCI option ROMs in real mode, or doing
	 * a resume from a suspend to RAM, which happens in real mode.
	 *
	 * Keep in sync with 'cpu/x86/gdt.h'.
	 */
gdt:
	/* selgdt 0, unused */
	.word	0x0000, 0x0000		/* dummy */
	.byte	0x00, 0x00, 0x00, 0x00

	/* selgdt 0x08, flat code segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x9b, 0xcf, 0x00 /* G=1 and 0x0f, So we get 4Gbytes
					  for limit */

	/* selgdt 0x10, flat data segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x93, 0xcf, 0x00

	/* selgdt 0x18, flat code segment (64-bit) */
	.word   0xffff, 0x0000
	.byte   0x00, 0x9b, 0xaf, 0x00

	/* gdt selector 0x20 tss segment, used by STM */
	.word   0xffff, 0x0000
	.byte   0x00, 0x8b, 0x80, 0x00

	/* The next two entries are used for executing ACPI S3 RESUME and VGA option ROMs */

	/* selgdt 0x28 16 bit 64k code at 0x00000000 */
	.word	0xffff, 0x0000
	.byte	0, 0x9a, 0, 0

	/* selgdt 0x30 16 bit 64k data at 0x00000000 */
	.word	0xffff, 0x0000
	.byte	0, 0x92, 0, 0

	/* The next entry is used for VGA option ROMs. See x86_asm.S */

	/*
	 * selgdt 0x38, flat code segment 16 bits
	 *
	 * FIXME: It's only used in %ds (therefore used like a data segment).
	 *        The PCI Specification doesn't enforce this.
	 *        Is this a workaround for broken Option ROMs?
	 */
	.word	0xffff, 0x0000
	.byte	0x00, 0x9b, 0x8f, 0x00 /* G=1 and 0x0f, So we get 4Gbytes for
					* limit
					*/

per_cpu_segment_descriptors:
	.rept CONFIG_MAX_CPUS
	/* flat data segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x93, 0xcf, 0x00
	.endr
gdt_end:

/* Segment selector pointing to the first per_cpu_segment_descriptor. */
per_cpu_segment_selector:
	.long	per_cpu_segment_descriptors - gdt

	.section ".text._start", "ax", @progbits
#if ENV_X86_64
SetCodeSelector:
	# save rsp because iret will align it to a 16 byte boundary
	mov	%rsp, %rdx

	# use iret to jump to a 64-bit offset in a new code segment
	# iret will pop cs:rip, flags, then ss:rsp
	mov	%ss, %ax	# need to push ss..
	push	%rax		# push ss instruction not valid in x64 mode,
				# so use ax
	push	%rsp
	pushfq
	push	%rcx		# cx is code segment selector from caller
	movabs	$setCodeSelectorLongJump, %rax
	push	%rax

	# the iret will continue at next instruction, with the new cs value
	# loaded
	iretq

setCodeSelectorLongJump:
	# restore rsp, it might not have been 16-byte aligned on entry
	mov	%rdx, %rsp
	ret
#endif
