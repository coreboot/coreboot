/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * The stub is a generic wrapper for bootstrapping a C-based SMM handler. Its
 * primary purpose is to put the CPU into protected mode with a stack and call
 * into the C handler.
 *
 * The stub_entry_params structure needs to correspond to the C structure
 * found in smm.h.
 */

#include <cpu/x86/cr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic_def.h>

.code32
.section ".module_parameters", "aw", @progbits
stub_entry_params:
stack_size:
.long 0
stack_top:
.long 0
c_handler:
.long 0
fxsave_area:
.long 0
fxsave_area_size:
.long 0
/* apic_to_cpu_num is a table mapping the default APIC id to CPU num. If the
 * APIC id is found at the given index, the contiguous CPU number is index
 * into the table. */
apic_to_cpu_num:
.fill CONFIG_MAX_CPUS,2,0xffff
/* allows the STM to bring up SMM in 32-bit mode */
start32_offset:
.long smm_trampoline32 - _start

.data
/* Provide fallback stack to use when a valid CPU number cannot be found. */
fallback_stack_bottom:
.skip 128
fallback_stack_top:

#define CR0_CLEAR_FLAGS \
	(CR0_CD | CR0_NW | CR0_PG | CR0_AM | CR0_WP | \
	 CR0_NE | CR0_TS | CR0_EM | CR0_MP)

#define SMM_DEFAULT_SIZE 0x10000

.text
.code16
.global _start
_start:
smm_handler_start:
#if CONFIG(SMM_LAPIC_REMAP_MITIGATION)
	/* Check if the LAPIC register block overlaps with the stub.
	 * This block needs to work without data accesses because they
	 * may be routed into the LAPIC register block.
	 * Code accesses, on the other hand, are never routed to LAPIC,
	 * which is what makes this work in the first place.
	 */
	mov	$LAPIC_BASE_MSR, %ecx
	rdmsr
	and	$(~0xfff), %eax
	call	1f
	/* Get the current program counter */
1:
	pop	%ebx
	sub	%ebx, %eax
	cmp	$(SMM_DEFAULT_SIZE), %eax
	ja	untampered_lapic
1:
#if CONFIG(CONSOLE_SERIAL)
	/* emit "Crash" on serial */
	mov	$(CONFIG_TTYS0_BASE), %dx
	mov	$'C', %al
	out	%al, (%dx)
	mov	$'r', %al
	out	%al, (%dx)
	mov	$'a', %al
	out	%al, (%dx)
	mov	$'s', %al
	out	%al, (%dx)
	mov	$'h', %al
	out	%al, (%dx)
#endif /* CONFIG_CONSOLE_SERIAL */
	/* now crash for real */
	ud2
untampered_lapic:
#endif
	movl	$(smm_relocate_gdt), %ebx
	lgdtl	(%ebx)

	movl	%cr0, %eax
	andl	$~CR0_CLEAR_FLAGS, %eax
	orl	$CR0_PE, %eax
	movl	%eax, %cr0

	/* Enable protected mode */
	ljmpl	$0x8, $smm_trampoline32

.align 4
smm_relocate_gdt:
	/* The first GDT entry is used for the lgdt instruction. */
	.word	smm_relocate_gdt_end - smm_relocate_gdt - 1
	.long	smm_relocate_gdt
	.word	0x0000

	/* gdt selector 0x08, flat code segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x9b, 0xcf, 0x00 /* G=1 and 0x0f, 4GB limit */

	/* gdt selector 0x10, flat data segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x93, 0xcf, 0x00

	/* gdt selector 0x18, flat code segment (64-bit) */
	.word	0xffff, 0x0000
	.byte	0x00, 0x9b, 0xaf, 0x00

	/* gdt selector 0x20 tss segment */
	.word   0xffff, 0x0000
	.byte   0x00, 0x8b, 0x80, 0x00
smm_relocate_gdt_end:

.align 4
.code32
.global smm_trampoline32
smm_trampoline32:
	/* Use flat data segment */
	movw	$0x10, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	xor	%ax, %ax /* zero out the gs and fs segment index */
	movw	%ax, %fs
	movw	%ax, %gs /* Used by cpu_info in ramstage */

	/* The CPU number is calculated by reading the initial APIC id. Since
	 * the OS can manipulate the APIC id use the non-changing cpuid result
	 * for APIC id (eax). A table is used to handle a discontiguous
	 * APIC id space.  */
apic_id:
	mov	$LAPIC_BASE_MSR, %ecx
	rdmsr
	and	$LAPIC_BASE_X2APIC_ENABLED, %eax
	cmp	$LAPIC_BASE_X2APIC_ENABLED, %eax
	jne	xapic

x2apic:
	mov	$0xb, %eax
	mov	$0, %ecx
	cpuid
	mov	%edx, %eax
	jmp	apicid_end

xapic:
	mov	$1, %eax
	cpuid
	mov	%ebx, %eax
	shr	$24, %eax

apicid_end:
	mov	$(apic_to_cpu_num), %ebx
	xor	%ecx, %ecx

1:
	cmp	(%ebx, %ecx, 2), %ax
	je	1f
	inc	%ecx
	cmp	$CONFIG_MAX_CPUS, %ecx
	jne	1b
	/* This is bad. One cannot find a stack entry because a CPU num could
	 * not be assigned. Use the fallback stack and check this condition in
	 * C handler. */
	movl	$(fallback_stack_top), %esp
	/* Clear fxsave location as there will be no saving/restoring. */
	xor	%edi, %edi
	jmp	2f
1:
	movl	stack_size, %eax
	mul	%ecx /* %eax(stack_size) * %ecx(cpu) = %eax(offset) */
	movl	stack_top, %ebx
	subl	%eax, %ebx /* global_stack_top - offset = stack_top */
	mov	%ebx, %esp

	/* Write canary to the bottom of the stack */
	movl	stack_size, %eax
	subl	%eax, %ebx /* %ebx(stack_top) - size = %ebx(stack_bottom) */
	movl	%ebx, (%ebx)
#if ENV_X86_64
	movl	$0, 4(%ebx)
#endif

	/* Create stack frame by pushing a NULL stack base pointer */
	pushl	$0x0
	mov	%esp, %ebp

	/* Allocate locals (fxsave, efer_backup) */
	subl	$0xc, %esp

	/* calculate fxsave location */
	mov	fxsave_area, %edi
	test	%edi, %edi
	jz	2f
	movl	fxsave_area_size, %eax
	mul	%ecx
	add	%eax, %edi

2:
	/* Save location of fxsave area. */
	mov	%edi, -4(%ebp)
	test	%edi, %edi
	jz	1f

	/* Enable sse instructions. */
	mov	%cr4, %eax
	orl	$(CR4_OSFXSR | CR4_OSXMMEXCPT), %eax
	mov	%eax, %cr4

	/* Save FP state. */
	fxsave	(%edi)

1:
	/* Align stack to 16 bytes. Another 32 bytes are pushed below. */
	andl	$0xfffffff0, %esp

#if ENV_X86_64
	mov	%ecx, %edi
	/* Backup IA32_EFER. Preserves ebx. */
	movl	$(IA32_EFER), %ecx
	rdmsr
	movl	%eax, -0x8(%ebp)
	movl	%edx, -0xc(%ebp)

	/* entry64.inc preserves ebx, esi, edi */
#include <cpu/x86/64bit/entry64.inc>
	mov	%edi, %ecx

#endif

	/* Call into the c-based SMM relocation function with the platform
	 * parameters. Equivalent to:
	 *   struct arg = { c_handler_params, cpu_num, smm_runtime, canary };
	 *   c_handler(&arg)
	 */
#if ENV_X86_64
	push	%rbx /* uintptr_t *canary */
	push	%rcx /* size_t cpu */

	mov	%rsp, %rdi	/* *arg */

	movabs	c_handler, %eax
	call	*%rax

	/*
	 * The only reason to go back to protected mode is that RSM doesn't restore
	 * MSR registers and MSR IA32_EFER was modified by entering long mode.
	 * Drop to protected mode to safely operate on the IA32_EFER MSR.
	 */

	/* Disable long mode. */
	#include <cpu/x86/64bit/exit32.inc>

	/* Restore IA32_EFER as RSM doesn't restore MSRs. */
	movl	$(IA32_EFER), %ecx
	rdmsr
	movl	-0x8(%ebp), %eax
	movl	-0xc(%ebp), %edx

	wrmsr

#else
	push	$0x0 /* Padding */
	push	%ebx /* uintptr_t *canary */
	push	%ecx /* size_t cpu */
	push	%esp /* smm_module_params *arg (allocated on stack). */
	mov	c_handler, %eax
	call	*%eax
#endif
	/* Retrieve fxsave location. */
	mov	-4(%ebp), %edi
	test	%edi, %edi
	jz	1f

	/* Restore FP state. */
	fxrstor	(%edi)

1:
	/* Exit from SM mode. */
	rsm
