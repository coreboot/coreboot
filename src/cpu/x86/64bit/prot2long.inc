/* SPDX-License-Identifier: GPL-2.0-only */

.text
.code32
/*
 * Macro to wrap a x86_64 function to be called from x86_32 code.
 * This assumes that 0-3 32bit arguments are passed to the
 * calling function.
 *
 * In order to preserve ESP without setting up a stack frame
 * pass the function to call in EAX. The macro prepends "__prot2lm_"
 * to the wrapped function name.
 */
.macro prot2lm_wrapper func2call:req
	.global __prot2lm_\func2call
__prot2lm_\func2call :
	/* Get function to call */
	mov	$\func2call, %eax

	/*
	 * Jump to function instead of call.
	 * It will return directly to caller.
	 */
	jmp	long_mode_call_3arg

	/* Not reachable */
.endm
