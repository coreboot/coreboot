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
 * http://www.coreboot.org/Payload_API
 */

.align 4
.text

.globl powerpc_do_exec
        .type powerpc_do_exec,@function

powerpc_do_exec:
#if 0
	pushl %ebp
	movl %esp, %ebp
	pushl %eax

	/* Put the run address in %eax */
	movl 8(%ebp), %eax

	/* Save off the rest of the registers */

	pushl %esi
	pushl %ecx
	pushl %ebp

	/* Push the argc and argv pointers on to the stack */

	movl 12(%ebp), %esi
	movl 16(%ebp), %ecx

	pushl %esi
	pushl %ecx

	/* Move a "magic" number on the stack - the other
	 * payload will use this as a clue that the argc
	 * and argv are sane
	 */

	movl  $12345678, %ecx
	pushl %ecx

	/* Jump to the code */
	call *%eax

	/* %eax has the return value */

	/* Skip over the argc/argv stuff still on the stack */
	addl $12, %esp

	/* Get back %ebp */
	popl %ebp

	/* Get the pointer to the return value
	 * and save the return value in it
         */

	movl 20(%ebp), %ecx
	movl %eax, (%eax)

	/* Get the rest of the saved registers */
	popl %ecx
	popl %esi
	popl %eax

	/* Restore the stack pointer */
	movl %ebp,%esp
	popl %ebp
	ret
#endif
