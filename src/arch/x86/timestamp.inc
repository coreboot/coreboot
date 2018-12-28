/*
 * This file is part of the coreboot project.
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

/*
 * Store the initial timestamp for booting in mmx registers. This works
 * because the bootblock isn't being compiled with MMX support so mm1 and
 * mm2 will be preserved into romstage.
 */
	.code32

.global stash_timestamp
stash_timestamp:

	/* Save the BIST value */
	movl	%eax, %ebx

	finit
	rdtsc
	movd	%ebx, %mm0
	movd	%eax, %mm1
	movd	%edx, %mm2

	/* Restore the BIST value to %eax */
	movl	%ebx, %eax
