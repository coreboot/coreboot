/* SPDX-License-Identifier: GPL-2.0-only */

/* NOTE: This handler assumes the SMM window goes from 0xa0000
 * to 0xaffff. In fact, at least on Intel Core CPUs (i945 chipset)
 * the SMM window is 128K big, covering 0xa0000 to 0xbffff.
 * So there is a lot of potential for growth in here. Let's stick
 * to 64k if we can though.
 */

#include <cpu/x86/lapic_def.h>
#include <cpu/x86/msr.h>

/*
 * +--------------------------------+ 0xaffff
 * |  Save State Map Node 0         |
 * |  Save State Map Node 1         |
 * |  Save State Map Node 2         |
 * |  Save State Map Node 3         |
 * |  ...                           |
 * +--------------------------------+ 0xaf000
 * |                                |
 * |                                |
 * |                                |
 * +--------------------------------+ 0xa8400
 * | SMM Entry Node 0 (+ stack)     |
 * +--------------------------------+ 0xa8000
 * | SMM Entry Node 1 (+ stack)     |
 * | SMM Entry Node 2 (+ stack)     |
 * | SMM Entry Node 3 (+ stack)     |
 * | ...                            |
 * +--------------------------------+ 0xa7400
 * |                                |
 * | SMM Handler                    |
 * |                                |
 * +--------------------------------+ 0xa0000
 *
 */

/* SMM_HANDLER_OFFSET is the 16bit offset within the ASEG
 * at which smm_handler_start lives. At the moment the handler
 * lives right at 0xa0000, so the offset is 0.
 */

#define SMM_HANDLER_OFFSET 0x0000

#if ENV_X86_64
.bss
ia32efer_backup_eax:
.long 0
ia32efer_backup_edx:
.long 0
#endif

/* initially SMM is some sort of real mode. Let gcc know
 * how to treat the SMM handler stub
 */

.section ".handler", "a", @progbits

.code16

/**
 * SMM code to enable protected mode and jump to the
 * C-written function void smi_handler(u32 smm_revision)
 *
 * All the bad magic is not all that bad after all.
 */
#define SMM_START 0xa0000
#define SMM_END   0xb0000
#if SMM_END <= SMM_START
#error invalid SMM configuration
#endif
.global smm_handler_start
smm_handler_start:
#if CONFIG(SMM_LAPIC_REMAP_MITIGATION)
	/* Check if the LAPIC register block overlaps with SMM.
	 * This block needs to work without data accesses because they
	 * may be routed into the LAPIC register block.
	 * Code accesses, on the other hand, are never routed to LAPIC,
	 * which is what makes this work in the first place.
	 */
	mov	$LAPIC_BASE_MSR, %ecx
	rdmsr
	and	$(~0xfff), %eax
	sub	$(SMM_START), %eax
	cmp	$(SMM_END - SMM_START), %eax
	ja	untampered_lapic
1:
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
	/* now crash for real */
	ud2
untampered_lapic:
#endif
	movw	$(smm_gdtptr16 - smm_handler_start + SMM_HANDLER_OFFSET), %bx
	lgdtl	%cs:(%bx)

	movl	%cr0, %eax
	andl	$0x7FFAFFD1, %eax /* PG,AM,WP,NE,TS,EM,MP = 0 */
	orl	$0x60000001, %eax /* CD, NW, PE = 1 */
	movl	%eax, %cr0

	/* Enable protected mode */
	ljmpl	$0x08, $1f

.code32
1:
	/* flush the cache after disabling it */
	wbinvd

	/* Use flat data segment */
	movw	$0x10, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	xor	%ax, %ax /* zero out the gs and fs segment index */
	movw	%ax, %fs
	movw	%ax, %gs /* Will be used for cpu_info */

	/* FIXME: Incompatible with X2APIC_SUPPORT. */
	/* Get this CPU's LAPIC ID */
	movl	$(LAPIC_DEFAULT_BASE | LAPIC_ID), %esi
	movl	(%esi), %ecx
	shr	$24, %ecx

	/* This is an ugly hack, and we should find a way to read the CPU index
	 * without relying on the LAPIC ID.
	 */

	/* calculate stack offset by multiplying the APIC ID
	 * by 1024 (0x400), and save that offset in ebp.
	 */
	shl	$10, %ecx
	movl	%ecx, %ebp

	/* We put the stack for each core right above
	 * its SMM entry point. Core 0 starts at 0xa8000,
	 * we spare 0x10 bytes for the jump to be sure.
	 */
	movl	$0xa8010, %eax
	subl	%ecx, %eax		/* subtract offset, see above */
	movl	%eax, %ebx		/* Save bottom of stack in ebx */

#define SMM_STACK_SIZE	(0x400 - 0x10)
	/* clear stack */
	cld
	movl	%eax, %edi
	movl	$(SMM_STACK_SIZE >> 2), %ecx
	xorl	%eax, %eax
	rep	stosl

	/* set new stack */
	addl	$SMM_STACK_SIZE, %ebx
	movl	%ebx, %esp

#if ENV_X86_64
	/* Backup IA32_EFER. Preserves ebx. */
	movl	$(IA32_EFER), %ecx
	rdmsr
	movl	%eax, ia32efer_backup_eax
	movl	%edx, ia32efer_backup_edx

	/* Enable long mode. Preserves ebx. */
#include <cpu/x86/64bit/entry64.inc>

#endif
	/* Call C handler */
	call	smi_handler

#if ENV_X86_64
	/*
	 * The only reason to go back to protected mode is that RSM doesn't restore
	 * MSR registers and MSR IA32_EFER was modified by entering long mode.
	 * Drop to protected mode to safely operate on the IA32_EFER MSR.
	 */

	/* Disable long mode. */
	#include <cpu/x86/64bit/exit32.inc>

	/* Restore IA32_EFER as RSM doesn't restore MSRs. */
	movl	$(IA32_EFER), %ecx
	movl	ia32efer_backup_eax, %eax
	movl	ia32efer_backup_edx, %edx
	wrmsr
#endif

	/* To return, just do rsm. It will "clean up" protected mode */
	rsm

.code16

.align	4, 0xff

smm_gdtptr16:
	.word	smm_gdt_end - smm_gdt - 1
	.long	smm_gdt - smm_handler_start + 0xa0000 + SMM_HANDLER_OFFSET

.code32

smm_gdt:
	/* The first GDT entry can not be used. Keep it zero */
	.long	0x00000000, 0x00000000

	/* gdt selector 0x08, flat code segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x9b, 0xcf, 0x00 /* G=1 and 0x0f, 4GB limit */

	/* gdt selector 0x10, flat data segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x93, 0xcf, 0x00

	/* gdt selector 0x18, flat code segment (64-bit) */
	.word   0xffff, 0x0000
	.byte   0x00, 0x9b, 0xaf, 0x00
smm_gdt_end:


.section ".jumptable", "a", @progbits

/* This is the SMM jump table. All cores use the same SMM handler
 * for simplicity. But SMM Entry needs to be different due to the
 * save state area. The jump table makes sure all CPUs jump into the
 * real handler on SMM entry.
 */

/* This code currently supports up to 4 CPU cores. If more than 4 CPU cores
 * shall be used, below table has to be updated, as well as smm.ld
 */

/* GNU AS/LD will always generate code that assumes CS is 0xa000. In reality
 * CS will be set to SMM_BASE[19:4] though. Knowing that the smm handler is the
 * first thing in the ASEG, we do a far jump here, to set CS to 0xa000.
 */

.code16
jumptable:
	/* core 3 */
	ljmp	$0xa000, $SMM_HANDLER_OFFSET
.align 1024, 0x00
	/* core 2 */
	ljmp	$0xa000, $SMM_HANDLER_OFFSET
.align 1024, 0x00
	/* core 1 */
	ljmp	$0xa000, $SMM_HANDLER_OFFSET
.align 1024, 0x00
	/* core 0 */
	ljmp	$0xa000, $SMM_HANDLER_OFFSET
.align 1024, 0x00
