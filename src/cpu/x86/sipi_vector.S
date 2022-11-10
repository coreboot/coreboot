/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/cr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/x86/msr.h>
#include <arch/ram_segs.h>

#define __RAMSTAGE__

/* The SIPI vector is responsible for initializing the APs in the system. It
 * loads microcode, sets up MSRs, and enables caching before calling into
 * C code. */

.section ".module_parameters", "aw", @progbits
ap_start_params:
gdtaddr:
.word 0 /* limit */
.long 0 /* table */
.word 0 /* unused */
idt_ptr:
.long 0
per_cpu_segment_descriptors:
.long 0
per_cpu_segment_selector:
.long 0
stack_top:
.long 0
stack_size:
.long 0
microcode_lock:
.long 0
microcode_ptr:
.long 0
msr_table_ptr:
.long 0
msr_count:
.long 0
c_handler:
.long 0
ap_count:
.long 0

#define CR0_CLEAR_FLAGS_CACHE_ENABLE (CR0_CD | CR0_NW)
#define CR0_SET_FLAGS (CR0_CLEAR_FLAGS_CACHE_ENABLE | CR0_PE)
#define CR0_CLEAR_FLAGS \
	(CR0_PG | CR0_AM | CR0_WP | CR0_NE | CR0_TS | CR0_EM | CR0_MP)

.text
.code16
.global _start
_start:
	cli
	xorl	%eax, %eax
	movl	%eax, %cr3    /* Invalidate TLB*/

	/* On hyper threaded cpus, invalidating the cache here is
	 * very very bad.  Don't.
	 */

	/* setup the data segment */
	movw	%cs, %ax
	movw	%ax, %ds

	/* The gdtaddr needs to be relative to the data segment in order
	 * to properly dereference it. The .text section comes first in an
	 * rmodule so _start can be used as a proxy for the load address. */
	movl	$(gdtaddr), %ebx
	sub	$(_start), %ebx

	lgdtl	(%ebx)

	movl	%cr0, %eax
	andl	$~CR0_CLEAR_FLAGS, %eax
	orl	$CR0_SET_FLAGS, %eax
	movl	%eax, %cr0

	ljmpl	$RAM_CODE_SEG, $1f
1:
	.code32
	movw	$RAM_DATA_SEG, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	xor	%ax, %ax /* zero out the gs and fs segment index */
	movw	%ax, %fs
	movw	%ax, %gs /* Will be used for cpu_info */

	/* Load the Interrupt descriptor table */
	mov	idt_ptr, %ebx
	lidt	(%ebx)

1:
	/* Obtain CPU number. */
	movl	ap_count, %ecx
	inc	%ecx
	lock cmpxchg %ecx, ap_count
	jnz	1b

	/* Setup stacks for each CPU. */
	movl	stack_size, %eax
	mul	%ecx
	movl	stack_top, %edx
	subl	%eax, %edx
	mov	%edx, %esp

	/* Save CPU number for calling the AP entry */
	push	%ecx

	/*
	 * The following code only needs to run on Intel platforms and thus the caller
	 * doesn't provide a microcode_ptr if not on Intel.
	 * On Intel platforms which update microcode using FIT the version check will
	 * also skip the microcode update.
	 */

	/* Determine if one should check microcode versions. */
	mov	microcode_ptr, %edi
	test	%edi, %edi
	jz	microcode_done /* Bypass if no microde exists. */

	/* Get the Microcode version. */
	xorl	%eax, %eax
	xorl	%edx, %edx
	movl	$IA32_BIOS_SIGN_ID, %ecx
	wrmsr
	mov	$1, %eax
	cpuid
	mov	$IA32_BIOS_SIGN_ID, %ecx
	rdmsr
	/* If something already loaded skip loading again. */
	test	%edx, %edx
	jnz	microcode_done

	/*
	 * Intel SDM and various BWGs specify to use a semaphore to update microcode
	 * on one thread per core on Hyper-Threading enabled CPUs. Due to this complex
	 * code would be necessary to determine the core #ID, initializing and picking
	 * the right semaphore out of CONFIG_MAX_CPUS / 2.
	 * Instead of the per core approachm, as recommended, use one global spinlock.
	 * Assuming that only pre-FIT platforms with Hyper-Threading enabled and at
	 * most 8 threads will ever run into this condition, the boot delay is negligible.
	 */

	/* Determine if parallel microcode loading is allowed. */
	cmpl	$0xffffffff, microcode_lock
	je	load_microcode

	/* Protect microcode loading. */
lock_microcode:
	lock btsl $0, microcode_lock
	jc	lock_microcode

load_microcode:
	/* Load new microcode. */
	mov	$IA32_BIOS_UPDT_TRIG, %ecx
	xor	%edx, %edx
	mov	%edi, %eax
	/* The microcode pointer is passed in pointing to the header. Adjust
	 * pointer to reflect the payload (header size is 48 bytes). */
	add	$48, %eax
	pusha
	wrmsr
	popa

	/* Unconditionally unlock microcode loading. */
	cmpl	$0xffffffff, microcode_lock
	je	microcode_done

	xor	%eax, %eax
	mov	%eax, microcode_lock

microcode_done:
	/*
	 * Load MSRs. Each entry in the table consists of:
	 * 0: index,
	 * 4: value[31:0]
	 * 8: value[63:32]
	 */
	mov	msr_table_ptr, %edi
	mov	msr_count, %ebx
	test	%ebx, %ebx
	jz	1f

#if CONFIG(X86_AMD_FIXED_MTRRS)
	/* Allow modification of RdDram and WrDram bits */
	mov	$SYSCFG_MSR, %ecx
	rdmsr
	or	$SYSCFG_MSR_MtrrFixDramModEn, %eax
	wrmsr
#endif

load_msr:
	mov	(%edi), %ecx
	mov	4(%edi), %eax
	mov	8(%edi), %edx
	wrmsr
	add	$12, %edi
	dec	%ebx
	jnz	load_msr

#if CONFIG(X86_AMD_FIXED_MTRRS)
	mov	$SYSCFG_MSR, %ecx
	rdmsr
	and	$~SYSCFG_MSR_MtrrFixDramModEn, %eax
	wrmsr
#endif

1:
	/* Enable caching. */
	mov	%cr0, %eax
	and	$~(CR0_CLEAR_FLAGS_CACHE_ENABLE), %eax
	mov	%eax, %cr0

#if CONFIG(SSE)
	/* Enable sse instructions. */
	mov	%cr4, %eax
	orl	$(CR4_OSFXSR | CR4_OSXMMEXCPT), %eax
	mov	%eax, %cr4
#endif

	pop	%edi /* Retrieve cpu index */
	andl	$0xfffffff0, %esp /* ensure stack alignment */

#if ENV_X86_64
	/* entry64.inc preserves ebx, esi, edi, ebp */
#include <cpu/x86/64bit/entry64.inc>
	movabs	c_handler, %eax
	call	*%rax
#else
	push	$0
	push	$0
	push	$0
	push	%edi
	mov	c_handler, %eax
	call	*%eax
#endif


halt_jump:
	hlt
	jmp	halt_jump
