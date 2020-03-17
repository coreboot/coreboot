/*
 * This file is part of the coreboot project.
 *
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

#include <gcccar.inc>
#include <cpu/x86/cache.h>

.code32
.globl chipset_teardown_car

chipset_teardown_car:
	pop	%esp

	/* Disable cache */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0

	AMD_DISABLE_STACK

	/* enable cache */
	movl	%cr0, %eax
	andl	$(~(CR0_CD | CR0_NW)), %eax
	movl	%eax, %cr0

	jmp	*%esp
