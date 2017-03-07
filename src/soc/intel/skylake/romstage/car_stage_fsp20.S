/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corporation
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
 */

#include <rules.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/post_code.h>

.section ".text"
.global car_stage_entry

car_stage_entry:

	/* Enter the C code */
	call	car_stage_c_entry

/*
 * Car teardown
 */
	/*
	 * eax:  New stack address
	 */

	/* Switch to the stack in RAM */
	movl	%eax, %esp

	/* chipset_teardown_car() is expected to disable cache-as-ram. */
	call	chipset_teardown_car

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

__main:
	post_code(POST_PREPARE_RAMSTAGE)
	cld			/* Clear direction flag. */

	call	copy_and_run
