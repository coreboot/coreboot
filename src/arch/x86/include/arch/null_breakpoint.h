/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _NULL_BREAKPOINT_H_
#define _NULL_BREAKPOINT_H_

#if CONFIG(DEBUG_NULL_DEREF_BREAKPOINTS) && \
    (CONFIG(DEBUG_NULL_DEREF_BREAKPOINTS_IN_ALL_STAGES) || ENV_RAMSTAGE)

/* Places data and instructions breakpoints at address zero. */
void null_breakpoint_init(void);
void null_breakpoint_disable(void);
#else
static inline void null_breakpoint_init(void)
{
	/* Not implemented */
}
static inline void null_breakpoint_disable(void)
{
	/* Not implemented */
}
#endif
#endif /* _NULL_BREAKPOINT_H_ */
