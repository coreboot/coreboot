/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

/* calling syntax:  i386_do_exec(long addr, int argc, char **argv, int *ret) */

/* This implements the payload API detailed here:
 * https://www.coreboot.org/Payload_API
 */

.align 4
.text

.global i386_do_exec
	.type i386_do_exec,@function

i386_do_exec:
	pushl %ebp
	movl %esp, %ebp

	/* Save the remaining callee preserved registers */
	pushl %ebx
	pushl %esi
	pushl %edi

	/* Push argc and argv on to the stack.
	 *
	 * We need to put a dummy value inbetween, as argc should be at offset
	 * 0x10, according to the payload API.
	 */
	pushl 12(%ebp)
	pushl $0
	pushl 16(%ebp)

	/* Push a "magic" number on the stack - the other payload will use this
	 * as a clue that the argc and argv values on the stack are sane.
	 */
	pushl $0x12345678

	/* Jump to the code */
	call *8(%ebp)
	/* %eax has the return value */

	/* Skip over the argc/argv stuff still on the stack.
	 * Don't assume %ebp is sane, here. Restore it from the stack.
	 */
	addl $0x10, %esp

	/* Restore the saved registers */
	popl %edi
	popl %esi
	popl %ebx
	popl %ebp

	/* Get pointer to return value and save the return value in it. */
	movl 16(%esp), %ecx
	movl %eax, (%ecx)

	ret
