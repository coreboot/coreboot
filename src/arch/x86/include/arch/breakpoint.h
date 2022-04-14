/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _BREAKPOINT_H_
#define _BREAKPOINT_H_

#include <arch/registers.h>
#include <types.h>

#if CONFIG(DEBUG_HW_BREAKPOINTS) && \
	(CONFIG(DEBUG_HW_BREAKPOINTS_IN_ALL_STAGES) || ENV_RAMSTAGE)
struct breakpoint_handle {
	int bp;
};

typedef int (*breakpoint_handler)(struct breakpoint_handle, struct eregs *info);

enum breakpoint_result {
	BREAKPOINT_RES_OK = 0,
	BREAKPOINT_RES_NONE_AVAILABLE = -1,
	BREAKPOINT_RES_INVALID_HANDLE = -2,
	BREAKPOINT_RES_INVALID_LENGTH = -3
};

enum breakpoint_type {
	BREAKPOINT_TYPE_INSTRUCTION = 0x0,
	BREAKPOINT_TYPE_DATA_WRITE = 0x1,
	BREAKPOINT_TYPE_IO = 0x2,
	BREAKPOINT_TYPE_DATA_RW = 0x3,
};

/* Creates an instruction breakpoint at the given address. */
enum breakpoint_result breakpoint_create_instruction(struct breakpoint_handle *out_handle,
						     void *virt_addr);
/* Creates a data breakpoint at the given address for len bytes. */
enum breakpoint_result breakpoint_create_data(struct breakpoint_handle *out_handle,
					      void *virt_addr, size_t len, bool write_only);
/* Removes a given breakpoint. */
enum breakpoint_result breakpoint_remove(struct breakpoint_handle handle);
/* Enables or disables a given breakpoint. */
enum breakpoint_result breakpoint_enable(struct breakpoint_handle handle, bool enabled);
/* Returns the type of a breakpoint. */
enum breakpoint_result breakpoint_get_type(struct breakpoint_handle handle,
					   enum breakpoint_type *type);
/*
 * Sets a handler function to be called when the breakpoint is hit. The handler should return 0
 * to continue or any other value to halt execution as a fatal error.
 */
enum breakpoint_result breakpoint_set_handler(struct breakpoint_handle handle,
					      breakpoint_handler handler);
/* Called by x86_exception to dispatch breakpoint exceptions to the correct handler. */
int breakpoint_dispatch_handler(struct eregs *info);
#else
static inline int breakpoint_dispatch_handler(struct eregs *info)
{
	/* Not implemented */
	return 0;
}
#endif
#endif /* _BREAKPOINT_H_ */
