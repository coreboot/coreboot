/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __HALT_H__
#define __HALT_H__

#include <arch/hlt.h>
#include <commonlib/bsd/stdlib.h>

static inline __noreturn void halt(void)
{
	abort();
}

/* Power off the system. */
void poweroff(void);

#endif /* __HALT_H__ */
