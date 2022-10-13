/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _ARCH_EXCEPTION_H
#define _ARCH_EXCEPTION_H

#include <arch/transition.h>
#include <types.h>

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
enum cb_err exception_handler_register(uint64_t vid, struct exception_handler *h);

/*
 * Unregister a handler from the vector id. Return 0 on success, < 0 on error.
 * Note that the unregistration is not thread/interrupt safe.
 */
enum cb_err exception_handler_unregister(uint64_t vid, struct exception_handler *h);

#endif
