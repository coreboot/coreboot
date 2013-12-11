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

	.text

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
	mov	sp, $0
	b	exception_common
2:
	mov	sp, $1
	b	exception_common
3:
	mov	sp, $2
	b	exception_common
4:
	mov	sp, $3
	b	exception_common
5:
	mov	sp, $4
	b	exception_common
6:
	mov	sp, $5
	b	exception_common
7:
	mov	sp, $6
	b	exception_common
8:
	mov	sp, $7
	b	exception_common

exception_common:
	str	sp, exception_idx
	ldr	sp, exception_stack_end
	push	{ lr }
	stmfd	sp, { sp, lr }^
	sub	sp, sp, $8
	push	{ r0 - r12 }
	mov	r0, sp
	ldr	r1, exception_idx
	blx	exception_dispatch
	pop	{ r0 - r12 }
	add	sp, sp, $8
	ldmfd	sp!, { pc }^


	.align 2
	.global exception_stack_end
exception_stack_end:
	.word 0

exception_idx:
	.word 0

	.thumb
	.global set_vbar
	.thumb_func
set_vbar:
	mcr	p15, 0, r0, c12, c0, 0
	bx	lr

