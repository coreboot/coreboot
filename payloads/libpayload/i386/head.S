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
	
	.global _entry, _leave
	.text
	.align 4

/* Our entry point - assume that the CPU is in
 *  32 bit protected mode and all segments are in a
 *  flat model.  Thats our operating mode, so we won't
 *  change anything
 */
	
_entry:
	call _init

	/* We're back - go back to the bootloader */
	ret

/* This function saves off the previous stack and
   switches us to our own execution enviornment
*/	

_init:
	/* No interrupts, please */
	cli

	/* Get the current stack pointer */
	movl %esp, %esi

	movl _istack, %ebx

	/* lret needs %cs in the stack, so copy it over */
	movw %cs, 4(%ebx)

	/* Exchange the current stack pointer for the one in
	   the initial stack (which happens to be the new
	   stack pointer) */

	xchgl %esi, 16(%ebx)

	/* Set the new stack pointer */
	movl %esi, %esp

	/* Return into the main entry function
	   and go
	 */

	lret

_leave:
	movl _istack, %ebx

	/* Restore the stack pointer from the storage area */
	movl 16(%ebx), %esp

	/* Return to the original context */
	lret

