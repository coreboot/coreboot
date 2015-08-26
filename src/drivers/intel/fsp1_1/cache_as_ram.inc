/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000,2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
 * Copyright (C) 2015 Intel Corp.
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
 * Foundation, Inc.
 */

/*
 * Replacement for cache_as_ram.inc when using the FSP binary.  This code
 * locates the FSP binary, initializes the cache as RAM and performs the
 * first stage of initialization.  Next this code switches the stack from
 * the cache to RAM and then disables the cache as RAM.  Finally this code
 * performs the final stage of initialization.
 */

#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/post_code.h>
#include <cbmem.h>

#define LHLT_DELAY	0x50000		/* I/O delay between post codes on failure */

	/*
	 * eax:  BIST value
	 * mm0:  low 32-bits of TSC value
	 * mm1:  high 32-bits of TSC value
	 */

	mov	%eax, %edi

cache_as_ram:
	post_code(0x20)

	/*
	 * edi:  BIST value
	 * mm0:  low 32-bits of TSC value
	 * mm1:  high 32-bits of TSC value
	 */

	/*
	 * Find the FSP binary in cbfs.
	 * Make a fake stack that has the return value back to this code.
	 */
	lea	fake_fsp_stack, %esp
	jmp	find_fsp
find_fsp_ret:
	/* Save the FSP location */
	mov	%eax, %ebp

	/*
	 * Only when a valid FSP binary is found at CONFIG_FSP_LOC is
	 * the returned FSP_INFO_HEADER structure address above the base
	 * address of FSP binary specified by the CONFIG_FSP_LOC value.
	 * All of the error values are in the 0x8xxxxxxx range which are
	 * below the CONFIG_FSP_LOC value.
	 */
	cmp	$CONFIG_FSP_LOC, %eax
	jbe	halt1

	post_code(0x22)

	/* Calculate entry into FSP */
	mov	0x30(%ebp), %eax	/* Load TempRamInitEntry */
	add	0x1c(%ebp), %eax	/* add in the offset for FSP */

	/*
	 * Pass early init variables on a fake stack (no memory yet)
	 * as well as the return location
	 */
	lea	CAR_init_stack, %esp

	/*
	 *       BIST value is zero
	 * eax:  TempRamInitApi address
	 * ebp:  FSP_INFO_HEADER address
	 * edi:  BIST value
	 * esi:  Not used
	 * mm0:  low 32-bits of TSC value
	 * mm1:  high 32-bits of TSC value
	 */

	/* call FSP binary to setup temporary stack */
	jmp	*%eax

CAR_init_done:
	addl	$4, %esp

	/*
	 * ebp:  FSP_INFO_HEADER address
	 * ecx:  Temp RAM base
	 * edx:  Temp RAM top
	 * edi:  BIST value
	 * mm0:  low 32-bits of TSC value
	 * mm1:  high 32-bits of TSC value
	 */

	cmp	$0, %eax
	jne	halt2

	/* Setup bootloader stack */
	movl	%edx, %esp

	/* Save BIST value */
	movd	%edi, %mm2

	/*
	 * ebp:  FSP_INFO_HEADER address
	 * ecx:  Temp RAM base
	 * edx:  Temp RAM top
	 * esp:  Top of stack in temp RAM
	 * mm0:  low 32-bits of TSC value
	 * mm1:  high 32-bits of TSC value
	 * mm2:  BIST value
	 */

	/* Coreboot assumes stack/heap region will be zero */
	cld
	movl	%ecx, %edi
	neg	%ecx
	add	%edx, %ecx
	shrl	$2, %ecx
	xorl	%eax, %eax
	rep	stosl

	/* Save FSP_INFO_HEADER location in ebx */
	mov	%ebp, %ebx

	/*
	 * ebx:  FSP_INFO_HEADER address
	 * esi:  Temp RAM base
	 * esp:  Top of stack in temp RAM
	 * mm0:  low 32-bits of TSC value
	 * mm1:  high 32-bits of TSC value
	 * mm2:  BIST value
	 */

	/* Frame for romstage_main(bist, tsc_low, tsc_hi, fih) */
	pushl	%ebx
	movd	%mm1, %eax
	pushl	%eax
	movd	%mm0, %eax
	pushl	%eax
	movd	%mm2, %eax
	pushl	%eax

before_romstage:
	post_code(0x23)

	/* Call romstage.c main function. */
	call	romstage_main

	/*
	 * eax:  New stack address
	 * ebx:  FSP_INFO_HEADER address
	 */

	/* Switch to the stack in RAM */
	movl	%eax, %esp

	/* Calculate TempRamExit entry into FSP */
	movl	%ebx, %ebp
	mov	0x40(%ebp), %eax
	add	0x1c(%ebp), %eax

	/* Build the call frame */
	pushl	$0

	/* Call TempRamExit */
	call	*%eax
	add	$4, %esp
	cmp	$0, %eax
	jne	halt3

	/* Display the MTRRs */
	call	soc_display_mtrrs

	/*
	 * The stack contents are initialized in src/soc/intel/common/stack.c
	 * to be the following:
	 *
	 *		*
	 *		*
	 *		*
	 *  +36: MTRR mask 1 63:32
	 *  +32: MTRR mask 1 31:0
	 *  +28: MTRR base 1 63:32
	 *  +24: MTRR base 1 31:0
	 *  +20: MTRR mask 0 63:32
	 *  +16: MTRR mask 0 31:0
	 *  +12: MTRR base 0 63:32
	 *   +8: MTRR base 0 31:0
	 *   +4: Number of MTRRs to setup (described above)
	 *   +0: Number of variable MTRRs to clear
	 */

	/* Clear all of the variable MTRRs. */
	popl	%ebx
	movl	$MTRRphysBase_MSR(0), %ecx
	clr	%eax
	clr	%edx

1:
	testl	%ebx, %ebx
	jz	1f
	wrmsr			/* Write MTRR base. */
	inc	%ecx
	wrmsr			/* Write MTRR mask. */
	inc	%ecx
	dec	%ebx
	jmp	1b

1:
	/* Get number of MTRRs. */
	popl	%ebx
	movl	$MTRRphysBase_MSR(0), %ecx
2:
	testl	%ebx, %ebx
	jz	2f

	/* Low 32 bits of MTRR base. */
	popl	%eax
	/* Upper 32 bits of MTRR base. */
	popl	%edx
	/* Write MTRR base. */
	wrmsr
	inc	%ecx
	/* Low 32 bits of MTRR mask. */
	popl	%eax
	/* Upper 32 bits of MTRR mask. */
	popl	%edx
	/* Write MTRR mask. */
	wrmsr
	inc	%ecx

	dec	%ebx
	jmp	2b
2:
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

	post_code(0x3b)

	/* Invalidate the cache again. */
	invd

	post_code(0x3c)

__main:
	post_code(POST_PREPARE_RAMSTAGE)
	cld			/* Clear direction flag. */
	call	romstage_after_car


	movb	$0x69, %ah
	jmp	.Lhlt

halt1:
	/*
	 * Failures for postcode 0xBA - failed in fsp_fih_early_find()
	 *
	 * Values are:
	 * 0x01 - FV signature, "_FVH" not present
	 * 0x02 - FFS GUID not present
	 * 0x03 - FSP INFO Header not found
	 * 0x04 - ImageBase does not equal CONFIG_FSP_LOC - Is the FSP rebased to
	 *        a different location, or does it need to be?
	 * 0x05 - FSP INFO Header signature "FSPH" not found
	 * 0x06 - FSP Image ID is not the expected ID.
	 */
	movb	$0xBA, %ah
	jmp	.Lhlt

halt2:
	/*
	 * Failures for postcode 0xBB - failed in the FSP:
	 *
	 * 0x00 - FSP_SUCCESS: Temp RAM was initialized successfully.
	 * 0x02 - FSP_INVALID_PARAMETER: Input parameters are invalid.
	 * 0x03 - FSP_UNSUPPORTED: The FSP calling conditions were not met.
	 * 0x07 - FSP_DEVICE_ERROR: Temp RAM initialization failed
	 * 0x0E - FSP_NOT_FOUND: No valid microcode was found in the microcode region.
	 * 0x14 - FSP_ALREADY_STARTED: Temp RAM initialization has been invoked
	 */
	movb	$0xBB, %ah
	jmp	.Lhlt

halt3:
	/*
	 * Failures for post code BC - failed in TempRamExit
	 *
	 * 0x00 - FSP_SUCCESS: Temp RAM Exit completed successfully.
	 * 0x02 - FSP_INVALID_PARAMETER: Input parameters are invalid.
	 * 0x03 - FSP_UNSUPPORTED: The FSP calling conditions were not met.
	 * 0x07 - FSP_DEVICE_ERROR: Temp RAM Exit failed.
	 */
	movb	$0xBC, %ah

.Lhlt:
	xchg	%al, %ah
#if IS_ENABLED(CONFIG_POST_IO)
	outb	%al, $CONFIG_POST_IO_PORT
#else
	post_code(POST_DEAD_CODE)
#endif
	movl	$LHLT_DELAY, %ecx
.Lhlt_Delay:
	outb	%al, $0xED
	loop	.Lhlt_Delay
	jmp	.Lhlt

/*
 * esp is set to this location so that the call into and return from the FSP
 * in find_fsp will work.
 */
	.align 4
fake_fsp_stack:
	.long	find_fsp_ret
	.long	CONFIG_FSP_LOC		/* FSP base address */

CAR_init_params:
	.long	CONFIG_CPU_MICROCODE_CBFS_LOC		/* Microcode Location */
	.long	CONFIG_CPU_MICROCODE_CBFS_LEN		/* Microcode Length */
	.long	0xFFFFFFFF - CONFIG_CBFS_SIZE + 1	/* Firmware Location */
	.long	CONFIG_CBFS_SIZE				/* Total Firmware Length */

CAR_init_stack:
	.long	CAR_init_done
	.long	CAR_init_params
