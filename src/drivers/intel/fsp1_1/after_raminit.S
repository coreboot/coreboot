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
 */

#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/post_code.h>

.extern fih_car
/*
 * This is the common entry point after DRAM has been initialized.
 */
	/*
	 * eax:  New stack address
	 */

	/* Switch to the stack in RAM */
	movl	%eax, %esp

	post_code(POST_FSP_TEMP_RAM_EXIT)

	/* Calculate TempRamExit entry into FSP */
	movl	fih_car, %ebp
	mov	0x40(%ebp), %eax
	add	0x1c(%ebp), %eax

	/* Build the call frame */
	pushl	$0

	/* Call TempRamExit */
	call	*%eax
	add	$4, %esp
	cmp	$0, %eax
	jz	1f
	/*
	 * Failures for post code BC - failed in TempRamExit
	 *
	 * 0x00 - FSP_SUCCESS: Temp RAM Exit completed successfully.
	 * 0x02 - FSP_INVALID_PARAMETER: Input parameters are invalid.
	 * 0x03 - FSP_UNSUPPORTED: The FSP calling conditions were not met.
	 * 0x07 - FSP_DEVICE_ERROR: Temp RAM Exit failed.
	 */
	movb	$0xBC, %ah
	jmp	.Lhlt

1:
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
	movl	$MTRR_PHYS_BASE(0), %ecx
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
	movl	$MTRR_PHYS_BASE(0), %ecx
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
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	orl	$MTRR_DEF_TYPE_EN, %eax
	wrmsr

	post_code(0x3b)

	/* Invalidate the cache again. */
	invd

	post_code(0x3c)

__main:
	post_code(POST_PREPARE_RAMSTAGE)
	cld			/* Clear direction flag. */
	call	after_cache_as_ram

