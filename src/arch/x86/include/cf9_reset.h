/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef X86_CF9_RESET_H
#define X86_CF9_RESET_H

/* Reset control port */
#define RST_CNT			0xcf9
#define FULL_RST		(1 << 3)
#define RST_CPU			(1 << 2)
#define SYS_RST			(1 << 1)

/* Implement the bare reset, i.e. write to cf9. */
void do_system_reset(void);
void do_full_reset(void);

/* Called by functions below before reset. */
#if CONFIG(HAVE_CF9_RESET_PREPARE)
void cf9_reset_prepare(void);
#else
static inline void cf9_reset_prepare(void) {}
#endif

/* Prepare for reset, run do_*_reset(), halt. */
__noreturn void system_reset(void);
__noreturn void full_reset(void);

#endif	/* X86_CF9_RESET_H */
