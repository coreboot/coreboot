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

#ifndef _ARCH_EXCEPTION_H
#define _ARCH_EXCEPTION_H

#include <arch/transition.h>

/* Initialize the exception handling on the current CPU. */
void exception_init(void);

/* Initialize VBAR and SP_EL3. */
void exception_init_asm(void *exception_stack_end);

/*
 * Order matters for handling return values. The larger the value the higher
 * the precedence.
 */
enum {
	EXC_RET_IGNORED,
	EXC_RET_ABORT,
	EXC_RET_HANDLED,
	EXC_RET_HANDLED_DUMP_STATE,
};

struct exception_handler {
	int (*handler)(struct exc_state *state, uint64_t vector_id);
	struct exception_handler *next;
};


/*
 * Register a handler provided with the associated vector id. Returns 0 on
 * success, < 0 on error. Note that registration is not thread/interrupt safe.
 */
int exception_handler_register(uint64_t vid, struct exception_handler *h);

/*
 * Unregister a handler from the vector id. Return 0 on success, < 0 on error.
 * Note that the unregistration is not thread/interrupt safe.
 */
int exception_handler_unregister(uint64_t vid, struct exception_handler *h);

#endif
