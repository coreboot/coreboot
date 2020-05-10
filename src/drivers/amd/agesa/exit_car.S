/* SPDX-License-Identifier: GPL-2.0-only */

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
