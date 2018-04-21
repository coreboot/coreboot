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

	.section ".text._idt", "ax", @progbits
#ifdef __x86_64__
	.code64
#else
	.code32
#endif
.global vec0
vec0:
	push	$0 /* error code */
	push	$0 /* vector */
	jmp int_hand
vec1:
	push	$0 /* error code */
	push	$1 /* vector */
	jmp int_hand

vec2:
	push	$0 /* error code */
	push	$2 /* vector */
	jmp int_hand

vec3:
	push	$0 /* error code */
	push	$3 /* vector */
	jmp	int_hand

vec4:
	push	$0 /* error code */
	push	$4 /* vector */
	jmp	int_hand

vec5:
	push	$0 /* error code */
	push	$5 /* vector */
	jmp	int_hand

vec6:
	push	$0 /* error code */
	push	$6 /* vector */
	jmp	int_hand

vec7:
	push	$0 /* error code */
	push	$7 /* vector */
	jmp	int_hand

vec8:
	/* error code */
	push	$8 /* vector */
	jmp	int_hand
	.word	0x9090

vec9:
	push	$0 /* error code */
	push	$9 /* vector */
	jmp int_hand

vec10:
	/* error code */
	push	$10 /* vector */
	jmp	int_hand
	.word	0x9090

vec11:
	/* error code */
	push	$11 /* vector */
	jmp	int_hand
	.word	0x9090

vec12:
	/* error code */
	push	$12 /* vector */
	jmp	int_hand
	.word	0x9090

vec13:
	/* error code */
	push	$13 /* vector */
	jmp	int_hand
	.word	0x9090

vec14:
	/* error code */
	push	$14 /* vector */
	jmp	int_hand
	.word	0x9090

vec15:
	push	$0 /* error code */
	push	$15 /* vector */
	jmp	int_hand

vec16:
	push	$0 /* error code */
	push	$16 /* vector */
	jmp	int_hand

vec17:
	/* error code */
	push	$17 /* vector */
	jmp	int_hand
	.word	0x9090

vec18:
	push	$0 /* error code */
	push	$18 /* vector */
	jmp	int_hand

vec19:
	push	$0 /* error code */
	push	$19 /* vector */
	jmp	int_hand

.global int_hand
int_hand:
	/* At this point, on x86-32, on the stack there is:
	 *  0(%esp) vector
	 *  4(%esp) error code
	 *  8(%esp) eip
	 * 12(%esp) cs
	 * 16(%esp) eflags
	 */
#ifdef __x86_64__
	push	%rdi
	push	%rsi
	push	%rbp
	/* Original stack pointer */
	lea	32(%rsp), %rbp
	push	%rbp
	push	%rbx
	push	%rdx
	push	%rcx
	push	%rax

	push	%rsp	/* Pointer to structure on the stack */
	call	x86_exception
	pop	%rax	/* Drop the pointer */

	pop	%rax
	pop	%rcx
	pop	%rdx
	pop	%rbx
	pop	%rbp	/* Ignore saved %rsp value */
	pop	%rbp
	pop	%rsi
	pop	%rdi

	add	$8, %rsp /* pop of the vector and error code */
#else
	pushl	%edi
	pushl	%esi
	pushl	%ebp

	/* Original stack pointer */
	leal	32(%esp), %ebp
	pushl	%ebp
	pushl	%ebx
	pushl	%edx
	pushl	%ecx
	pushl	%eax

	/* Save pointer to eregs structure */
	movl	%esp, %ebp
	/* Align stack to 16 bytes. */
	andl	$0xfffffff0, %esp
	/* Save original stack pointer while keeping stack alignment. This
	   value is also the eregs argument x86_exception(). */
	sub	$12, %esp
	pushl	%ebp	/* Pointer to structure on the stack */
	call	x86_exception
	pop	%esp	/* Unwind the stack alignment and argument passing. */

	popl	%eax
	popl	%ecx
	popl	%edx
	popl	%ebx
	popl	%ebp	/* Ignore saved %esp value */
	popl	%ebp
	popl	%esi
	popl	%edi

	addl	$8, %esp /* pop of the vector and error code */
#endif

	iret
