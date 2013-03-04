/*
 * This file is part of the libpayload project.
 *
 * Copyright 2013 Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

exception_stack:
	.align 5
	.skip 0x2000, 0xa5
exception_stack_end:
	.word	exception_stack_end

exception_handler:
	.word 0


	.align 6
	.arm
	.global exception_table
exception_table:
	b	1f
	b	2f
	b	3f
	b	4f
	b	5f
	b	6f
	b	7f
	b	8f

1:
	ldr	sp, _not_used
	b	exception_common
2:
	ldr	sp, _undefined_instruction
	b	exception_common
3:
	ldr	sp, _software_interrupt
	b	exception_common
4:
	ldr	sp, _prefetch_abort
	b	exception_common
5:
	ldr	sp, _data_abort
	b	exception_common
6:
	ldr	sp, _not_used
	b	exception_common
7:
	ldr	sp, _irq
	b	exception_common
8:
	ldr	sp, _fiq
	b	exception_common

exception_common:
	str	sp, exception_handler
	ldr	sp, exception_stack_end
	push	{ lr }
	sub	sp, sp, $8
	push	{ r0 - r12 }
	mov	r0, sp
	mov	lr, pc
	ldr	pc, exception_handler
	pop	{ r0 - r12 }
	add	sp, sp, $8
	ldm	sp!, { pc }^


_undefined_instruction:	.word exception_undefined_instruction
_software_interrupt:	.word exception_software_interrupt
_prefetch_abort:	.word exception_prefetch_abort
_data_abort:		.word exception_data_abort
_not_used:		.word exception_not_used
_irq:			.word exception_irq
_fiq:			.word exception_fiq

	.thumb
	.global set_vbar
	.thumb_func
set_vbar:
	mcr	p15, 0, r0, c12, c0, 0
	bx	lr

	.global exception_test
	.thumb_func
exception_test:
	mov	r1, $1
	mov	r0, pc
	add	r0, $3
	ldr	r1, [r1]
	bx	lr

