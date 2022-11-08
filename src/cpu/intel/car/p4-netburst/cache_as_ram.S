/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/intel/post_codes.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/post_code.h>
#include <cpu/x86/lapic_def.h>

/* Macro to access Local APIC registers at default base. */
#define LAPIC(x)		$(LAPIC_DEFAULT_BASE | LAPIC_ ## x)

.section .init
.global bootblock_pre_c_entry

#include <cpu/intel/car/cache_as_ram_symbols.inc>

.code32
_cache_as_ram_setup:

bootblock_pre_c_entry:

cache_as_ram:
	post_code(POST_BOOTBLOCK_CAR)

	movl	$LAPIC_BASE_MSR, %ecx
	rdmsr
	andl	$LAPIC_BASE_MSR_BOOTSTRAP_PROCESSOR, %eax
	jz	ap_init

	/* Clear/disable fixed MTRRs */
	mov	$fixed_mtrr_list, %ebx
	xor	%eax, %eax
	xor	%edx, %edx

clear_fixed_mtrr:
	movzwl	(%ebx), %ecx
	wrmsr
	add	$2, %ebx
	cmp	$fixed_mtrr_list_end, %ebx
	jl	clear_fixed_mtrr

	/* Figure out how many MTRRs we have, and clear them out */
	mov	$MTRR_CAP_MSR, %ecx
	rdmsr
	movzb	%al, %ebx		/* Number of variable MTRRs */
	mov	$MTRR_PHYS_BASE(0), %ecx
	xor	%eax, %eax
	xor	%edx, %edx

clear_var_mtrr:
	wrmsr
	inc	%ecx
	wrmsr
	inc	%ecx
	dec	%ebx
	jnz	clear_var_mtrr
	post_code(POST_SOC_SET_DEF_MTRR_TYPE)

	/* Configure the default memory type to uncacheable. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	andl	$(~0x00000cff), %eax
	wrmsr

	post_code(POST_SOC_DETERMINE_CPU_ADDR_BITS)

	/* Determine CPU_ADDR_BITS and load PHYSMASK high
	 * word to %edx.
	 */
	movl	$0x80000000, %eax
	cpuid
	cmpl	$0x80000008, %eax
	jc	addrsize_no_MSR
	movl	$0x80000008, %eax
	cpuid
	movb	%al, %cl
	sub	$32, %cl
	movl	$1, %edx
	shl	%cl, %edx
	subl	$1, %edx
	jmp	addrsize_set_high
addrsize_no_MSR:
	movl	$1, %eax
	cpuid
	andl	$(1 << 6 | 1 << 17), %edx	/* PAE or PSE36 */
	jz	addrsize_set_high
	movl	$0x0f, %edx

	/* Preload high word of address mask (in %edx) for Variable
	 * MTRRs 0 and 1 and enable local APIC at default base.
	 */
addrsize_set_high:
	xorl	%eax, %eax
	movl	$MTRR_PHYS_MASK(0), %ecx
	wrmsr
	movl	$MTRR_PHYS_MASK(1), %ecx
	wrmsr
	movl	$LAPIC_BASE_MSR, %ecx
	not	%edx
	movl	%edx, %ebx
	rdmsr
	andl	%ebx, %edx
	andl	$(~LAPIC_BASE_MSR_ADDR_MASK), %eax
	orl	$(LAPIC_DEFAULT_BASE | LAPIC_BASE_MSR_ENABLE), %eax
	wrmsr

bsp_init:

	post_code(POST_SOC_BSP_INIT)

	/* Send INIT IPI to all excluding ourself. */
	movl	LAPIC(ICR), %edi
	movl	$(LAPIC_DEST_ALLBUT | LAPIC_INT_ASSERT | LAPIC_DM_INIT), %eax
1:	movl	%eax, (%edi)
	movl	$0x30, %ecx
2:	pause
	dec	%ecx
	jnz	2b
	movl	(%edi), %ecx
	andl	$LAPIC_ICR_BUSY, %ecx
	jnz	1b

	post_code(POST_SOC_COUNT_CORES)

	movl	$1, %eax
	cpuid
	btl	$28, %edx
	jnc	sipi_complete
	bswapl	%ebx
	movzx	%bh, %edi
	cmpb	$1, %bh
	jbe	sipi_complete	/* only one LAPIC ID in package */

	movl	$0, %eax
	cpuid
	movb	$1, %bl
	cmpl	$4, %eax
	jb	cores_counted
	movl	$4, %eax
	movl	$0, %ecx
	cpuid
	shr	$26, %eax
	movb	%al, %bl
	inc	%bl

cores_counted:
	movl	%edi, %eax
	divb	%bl
	cmpb	$1, %al
	jbe	sipi_complete	/* only LAPIC ID of a core */

	/* For a hyper-threading processor, cache must not be disabled
	 * on an AP on the same physical package with the BSP.
	 */

hyper_threading_cpu:

	post_code(POST_SOC_CPU_HYPER_THREADING)

	/* Send Start IPI to all excluding ourself. */
	movl	LAPIC(ICR), %edi
	movl	$(LAPIC_DEST_ALLBUT | LAPIC_DM_STARTUP), %eax
	orl	$ap_sipi_vector_in_rom, %eax
1:	movl	%eax, (%edi)
	movl	$0x30, %ecx
2:	pause
	dec	%ecx
	jnz	2b
	movl	(%edi), %ecx
	andl	$LAPIC_ICR_BUSY, %ecx
	jnz	1b

	post_code(POST_SOC_CPU_SIBLING_DELAY)

	/* Wait for sibling CPU to start. */
1:	movl	$(MTRR_PHYS_BASE(0)), %ecx
	rdmsr
	andl	%eax, %eax
	jnz	sipi_complete

	movl	$0x30, %ecx
2:	pause
	dec	%ecx
	jnz	2b
	jmp	1b


ap_init:
	post_code(POST_SOC_CPU_AP_INIT)

	/* Do not disable cache (so BSP can enable it). */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	movl	%eax, %cr0

	post_code(POST_SOC_SET_MTRR_BASE)

	/* MTRR registers are shared between HT siblings. */
	movl	$(MTRR_PHYS_BASE(0)), %ecx
	movl	$(1 << 12), %eax
	xorl	%edx, %edx
	wrmsr

	post_code(POST_SOC_AP_HALT)

ap_halt:
	cli
1:	hlt
	jmp	1b



sipi_complete:

	post_code(POST_SOC_SET_CAR_BASE)

	/* Set Cache-as-RAM base address. */
	movl	$(MTRR_PHYS_BASE(0)), %ecx
	movl	car_mtrr_start, %eax
	orl	$MTRR_TYPE_WRBACK, %eax
	xorl	%edx, %edx
	wrmsr

	/* Set Cache-as-RAM mask. */
	movl	$(MTRR_PHYS_MASK(0)), %ecx
	rdmsr
	movl	car_mtrr_mask, %eax
	orl	$MTRR_PHYS_MASK_VALID, %eax
	wrmsr

	post_code(POST_SOC_ENABLE_MTRRS)

	/* Enable MTRR. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	orl	$MTRR_DEF_TYPE_EN, %eax
	wrmsr

	/* Enable L2 cache Write-Back (WBINVD and FLUSH#).
	 *
	 * MSR is set when DisplayFamily_DisplayModel is one of:
	 * 06_0x, 06_17, 06_1C
	 *
	 * Description says this bit enables use of WBINVD and FLUSH#.
	 * Should this be set only after the system bus and/or memory
	 * controller can successfully handle write cycles?
	 */

#define EAX_FAMILY(a)	(a << 8)	/* for family <= 0fH */
#define EAX_MODEL(a)	(((a & 0xf0) << 12) | ((a & 0xf) << 4))

	movl	$1, %eax
	cpuid
	movl	%eax, %ebx
	andl	$EAX_FAMILY(0x0f), %eax
	cmpl	$EAX_FAMILY(0x06), %eax
	jne	no_msr_11e
	movl	%ebx, %eax
	andl	$EAX_MODEL(0xff), %eax
	cmpl	$EAX_MODEL(0x17), %eax
	je	has_msr_11e
	cmpl	$EAX_MODEL(0x1c), %eax
	je	has_msr_11e
	andl	$EAX_MODEL(0xf0), %eax
	cmpl	$EAX_MODEL(0x00), %eax
	jne	no_msr_11e
has_msr_11e:
	movl	$0x11e, %ecx
	rdmsr
	orl	$(1 << 8), %eax
	wrmsr
no_msr_11e:

	post_code(POST_SOC_ENABLE_CACHE)

	/* Cache the whole rom to fetch microcode updates */
	movl	$MTRR_PHYS_BASE(1), %ecx
	xorl	%edx, %edx
	movl	rom_mtrr_base, %eax
	orl	$MTRR_TYPE_WRPROT, %eax
	wrmsr
	movl	$MTRR_PHYS_MASK(1), %ecx
	rdmsr
	movl	rom_mtrr_mask, %eax
	orl	$MTRR_PHYS_MASK_VALID, %eax
	wrmsr

	/* Enable cache (CR0.CD = 0, CR0.NW = 0). */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	invd
	movl	%eax, %cr0

#if CONFIG(MICROCODE_UPDATE_PRE_RAM)
	update_microcode:
	/* put the return address in %esp */
	movl	$end_microcode_update, %esp
	jmp	update_bsp_microcode
	end_microcode_update:
#endif
	post_code(POST_SOC_DISABLE_CACHE)
	/* Disable caching to change MTRR's. */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0

	/*
	 * An unidentified combination of speculative reads and branch
	 * predictions inside WRPROT-cacheable memory can cause invalidation
	 * of cachelines and loss of stack on models based on NetBurst
	 * microarchitecture. Therefore disable WRPROT region entirely for
	 * all family F models.
	 */
	movl	$1, %eax
	cpuid
	cmp	$0xf, %ah
	jne	cache_rom

disable_cache_rom:
	movl	$MTRR_PHYS_MASK(1), %ecx
	rdmsr
	andl	$(~MTRR_PHYS_MASK_VALID), %eax
	wrmsr
	jmp	fill_cache

cache_rom:
	/* Enable cache for our code in Flash because we do XIP here */
	movl	$MTRR_PHYS_BASE(1), %ecx
	xorl	%edx, %edx
	movl	$_program, %eax
	andl	xip_mtrr_mask, %eax
	orl	$MTRR_TYPE_WRPROT, %eax
	wrmsr
	movl	$MTRR_PHYS_MASK(1), %ecx
	rdmsr
	movl	xip_mtrr_mask, %eax
	orl	$MTRR_PHYS_MASK_VALID, %eax
	wrmsr

fill_cache:
	post_code(POST_SOC_FILL_CACHE)
	/* Enable cache. */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	invd
	movl	%eax, %cr0

	/* Clear the cache memory region. This will also fill up the cache. */
	cld
	xorl	%eax, %eax
	movl	$_car_mtrr_start, %edi
	movl	$_car_mtrr_size, %ecx
	shr	$2, %ecx
	rep	stosl

	/* Setup the stack. */
	mov	$_ecar_stack, %esp

	/* Need to align stack to 16 bytes at call instruction. Account for
	the pushes below. */
	andl	$0xfffffff0, %esp
	subl	$4, %esp

#if ENV_X86_64
	#include <cpu/x86/64bit/entry64.inc>

	movd	%mm2, %rdi
	shlq	$32, %rdi	/* BIST */
	movd	%mm1, %rsi
	or	%rsi, %rdi	/* tsc[63:32] */
	movd	%mm0, %rsi	/* tsc[31:0] */

#else
	/* push TSC and BIST to stack */
	movd	%mm0, %eax
	pushl	%eax	/* BIST */
	movd	%mm2, %eax
	pushl	%eax	/* tsc[63:32] */
	movd	%mm1, %eax
	pushl	%eax	/* tsc[31:0] */
#endif

before_c_entry:
	post_code(POST_BOOTBLOCK_BEFORE_C_ENTRY)
	call	bootblock_c_entry_bist

	/* Should never see this postcode */
	post_code(POST_DEAD_CODE)

.Lhlt:
	hlt
	jmp	.Lhlt

fixed_mtrr_list:
	.word	MTRR_FIX_64K_00000
	.word	MTRR_FIX_16K_80000
	.word	MTRR_FIX_16K_A0000
	.word	MTRR_FIX_4K_C0000
	.word	MTRR_FIX_4K_C8000
	.word	MTRR_FIX_4K_D0000
	.word	MTRR_FIX_4K_D8000
	.word	MTRR_FIX_4K_E0000
	.word	MTRR_FIX_4K_E8000
	.word	MTRR_FIX_4K_F0000
	.word	MTRR_FIX_4K_F8000
fixed_mtrr_list_end:

_cache_as_ram_setup_end:
