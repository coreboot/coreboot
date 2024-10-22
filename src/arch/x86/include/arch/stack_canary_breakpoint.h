/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _STACK_CANARY_BREAKPOINT_H_
#define _STACK_CANARY_BREAKPOINT_H_

#if CONFIG(DEBUG_STACK_OVERFLOW_BREAKPOINTS) && \
    (CONFIG(DEBUG_STACK_OVERFLOW_BREAKPOINTS_IN_ALL_STAGES) || ENV_RAMSTAGE)

/* Places a data breakpoint at stack canary. */
void stack_canary_breakpoint_init(void);
void stack_canary_breakpoint_disable(void);
#else
static inline void stack_canary_breakpoint_init(void)
{
	/* Not implemented */
}
static inline void stack_canary_breakpoint_disable(void)
{
	/* Not implemented */
}
#endif
#endif /* _STACK_CANARY_BREAKPOINT_H_ */
