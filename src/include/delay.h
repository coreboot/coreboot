/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>

#if CONFIG_UDELAY_LAPIC_FIXED_FSB != 0
static inline u32 get_timer_fsb(void)
{
	return CONFIG_UDELAY_LAPIC_FIXED_FSB;
}
#else
u32 get_timer_fsb(void);
#endif

void init_timer(void);

void udelay(unsigned int usecs);
void mdelay(unsigned int msecs);
void delay(unsigned int secs);
#endif /* DELAY_H */
