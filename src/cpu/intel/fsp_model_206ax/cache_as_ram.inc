/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000,2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2013 Sage Electronic Engineering, LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <cpu/x86/stack.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/post_code.h>
#include <cbmem.h>

#define CACHE_AS_RAM_SIZE CONFIG_DCACHE_RAM_SIZE
#define CACHE_AS_RAM_BASE CONFIG_DCACHE_RAM_BASE

#define CPU_PHYSMASK_HI  (1 << (CONFIG_CPU_ADDR_BITS - 32) - 1)
#define LHLT_DELAY	0x50000		/* delay between post codes on FSP failure */
#define NoEvictMod_MSR 0x2e0

cache_as_ram:
	post_code(0x20)

	/* Send INIT IPI to all excluding ourself. */
	movl	$0x000C4500, %eax
	movl	$0xFEE00300, %esi
	movl	%eax, (%esi)

	/* All CPUs need to be in Wait for SIPI state */
wait_for_sipi:
	movl	(%esi), %eax
	bt	$12, %eax
	jc	wait_for_sipi

	post_code(0x21)
	/* Zero out all fixed range and variable range MTRRs. */
	movl	$mtrr_table, %esi
	movl	$((mtrr_table_end - mtrr_table) / 2), %edi
	xorl	%eax, %eax
	xorl	%edx, %edx
clear_mtrrs:
	movw	(%esi), %bx
	movzx	%bx, %ecx
	wrmsr
	add	$2, %esi
	dec	%edi
	jnz	clear_mtrrs

	/* Init floating point */
	emms
	fninit

	/*
	 * Find the FSP binary in cbfs.
	 * Make a fake stack that has the return value back to this code.
	 */
	lea   fake_fsp_stack, %esp
	jmp   find_fsp
find_fsp_ret:
	/* Save the FSP location */
	mov   %eax,   %ebp
	cmp   $CONFIG_FSP_LOC, %eax
	jb   halt1

	post_code(0x22)

	/* Calculate entry into FSP */
	mov   0x30(%ebp), %eax	/* Load TempRamInitEntry */
	add   0x1c(%ebp), %eax	/* add in the offset for the FSP base address */

	/*
	 * Pass early init variables on a fake stack (no memory yet)
	 * as well as the return location
	 */
	lea   CAR_init_stack, %esp

	/* call FSP binary to setup temporary stack */
	jmp   *%eax

CAR_init_done:
	addl  $4, %esp
	cmp   $0, %eax
	jne   halt2

	/* Save FSP_INFO_HEADER location in ebx */
	mov    %ebp, %ebx

	/*
	 * setup bootloader stack
	 * ecx:  stack base
	 * edx:  stack top
	 */
	lea   -4(%edx), %esp
	movl	%esp, %ebp

before_romstage:
	post_code(0x23)

	/* Call romstage.c main function. */
	pushl   %ebx
	call	main

romstage_main_return:
	post_code(0x2f)

/* Disable cache. */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0

	post_code(0x31)

	/* Disable MTRR. */
	movl	$MTRRdefType_MSR, %ecx
	rdmsr
	andl	$(~MTRRdefTypeEn), %eax
	wrmsr

	post_code(0x32)

	/* Zero out all fixed range and variable range MTRRs. */
	movl	$mtrr_table, %esi
	movl	$((mtrr_table_end - mtrr_table) / 2), %edi
	xorl	%eax, %eax
	xorl	%edx, %edx
_clear_mtrrs_:
	movw	(%esi), %bx
	movzx	%bx, %ecx
	wrmsr
	add	$2, %esi
	dec	%edi
	jnz	_clear_mtrrs_

	post_code(0x33)

	/* Enable cache. */
	movl	%cr0, %eax
	andl	$~(CR0_CacheDisable | CR0_NoWriteThrough), %eax
	movl	%eax, %cr0

	post_code(0x36)

	/* Disable cache. */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0

	post_code(0x38)

	/* Enable Write Back and Speculative Reads for the first MB
	 * and coreboot_ram.
	 */
	movl	$MTRRphysBase_MSR(0), %ecx
	movl	$(0x00000000 | MTRR_TYPE_WRBACK), %eax
	xorl	%edx, %edx
	wrmsr
	movl	$MTRRphysMask_MSR(0), %ecx
	movl	$(~(CONFIG_RAMTOP - 1) | MTRRphysMaskValid), %eax
	movl	$CPU_PHYSMASK_HI, %edx	// 36bit address space
	wrmsr

#if CONFIG_CACHE_ROM_SIZE
	/* Enable Caching and speculative Reads for the
	 * complete ROM now that we actually have RAM.
	 */
	movl	$MTRRphysBase_MSR(1), %ecx
	movl	$(CACHE_ROM_BASE | MTRR_TYPE_WRPROT), %eax
	xorl	%edx, %edx
	wrmsr
	movl	$MTRRphysMask_MSR(1), %ecx
	movl	$(~(CONFIG_CACHE_ROM_SIZE - 1) | MTRRphysMaskValid), %eax
	movl	$CPU_PHYSMASK_HI, %edx
	wrmsr
#endif

	post_code(0x39)

	/* And enable cache again after setting MTRRs. */
	movl	%cr0, %eax
	andl	$~(CR0_CacheDisable | CR0_NoWriteThrough), %eax
	movl	%eax, %cr0

	post_code(0x3a)

	/* Enable MTRR. */
	movl	$MTRRdefType_MSR, %ecx
	rdmsr
	orl	$MTRRdefTypeEn, %eax
	wrmsr

	post_code(0x3d)

	/* Clear boot_complete flag. */
	xorl	%ebp, %ebp
__main:
	post_code(POST_PREPARE_RAMSTAGE)
	cld			/* Clear direction flag. */

	movl	%ebp, %esi

	movl	$ROMSTAGE_STACK, %esp
	movl	%esp, %ebp
	pushl	%esi
	call	copy_and_run

halt1:
	/*
	 * Failures for postcode 0xBA - failed in find_fsp()
	 *
	 * Values are:
	 * 0x01 - FV signature, "_FVH" not present
	 * 0x02 - FFS GUID not present
	 * 0x03 - FSP INFO Header not found
	 * 0x04 - ImageBase does not equal CONFIG_FSP_LOC - Is the FSP rebased to
	 *        a different location, or does it need to be?
	 * 0x05 - FSP INFO Header signature "FSPH" not found
	 * 0x06 - FSP Image ID is not the expected ID.
	 *        For ivybridge_bd82x6x, the ID is expected to be 'CC2-FSP\0'
	 *        For ivybridge_i89xx, the ID is expected to be 'ST2-FSP\0'
	 *
	 */
	movb 	$0xBA, %ah
	jmp	.Lhlt

halt2:
	/*
	 * Failures for postcode 0xBB - failed in the FSP:
	 *
	 * 0x00 - FSP_SUCCESS: Temp RAM was initialized successfully.
	 * 0x02 - FSP_INVALID_PARAMETER: Input parameters are invalid.
	 * 0x0E - FSP_NOT_FOUND: No valid microcode was found in the microcode region.
	 * 0x03 - FSP_UNSUPPORTED: The FSP calling conditions were not met.
	 * 0x07 - FSP_DEVICE_ERROR: Temp RAM initialization failed
	 * 0x14 - FSP_ALREADY_STARTED: Temp RAM initialization has been invoked
	 */
	movb	$0xBB, %ah

.Lhlt:
	xchg %al, %ah
#if CONFIG_IO_POST
	outb    %al, $CONFIG_IO_POST_PORT
#else
	post_code(POST_DEAD_CODE)
#endif
	movl $LHLT_DELAY, %ecx
.Lhlt_Delay:
	outb	%al, $0xED
	loop	.Lhlt_Delay
	jmp	.Lhlt

	.align 4
fake_fsp_stack:
	.long  find_fsp_ret

CAR_init_params:
	.long  CONFIG_CPU_MICROCODE_CBFS_LOC
	.long  CONFIG_CPU_MICROCODE_CBFS_LEN	/* Microcode Length */
	.long  0xFFFFFFFF - CONFIG_ROM_SIZE + 1	/* Firmware Location */
	.long  CONFIG_ROM_SIZE	/* Total Firmware Length */

CAR_init_stack:
	.long  CAR_init_done
	.long  CAR_init_params

mtrr_table:
	/* Fixed MTRRs */
	.word 0x250, 0x258, 0x259
	.word 0x268, 0x269, 0x26A
	.word 0x26B, 0x26C, 0x26D
	.word 0x26E, 0x26F
	/* Variable MTRRs */
	.word 0x200, 0x201, 0x202, 0x203
	.word 0x204, 0x205, 0x206, 0x207
	.word 0x208, 0x209, 0x20A, 0x20B
	.word 0x20C, 0x20D, 0x20E, 0x20F
/*	.word 0x210, 0x211, 0x212, 0x213 */
mtrr_table_end:

