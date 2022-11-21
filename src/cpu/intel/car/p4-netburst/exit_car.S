/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/intel/post_codes.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/post_code.h>

.code32
.global chipset_teardown_car

chipset_teardown_car:
	pop %esp

	post_code(POST_POSTCAR_DISABLE_CACHE)

	/* Disable cache. */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0

	post_code(POST_POSTCAR_DISABLE_DEF_MTRR)

	/* Disable MTRR. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	andl	$(~MTRR_DEF_TYPE_EN), %eax
	wrmsr

	post_code(POST_POSTCAR_TEARDOWN_DONE)

	/* Return to caller. */
	jmp	*%esp
