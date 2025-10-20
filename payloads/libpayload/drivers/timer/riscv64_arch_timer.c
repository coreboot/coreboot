/* SPDX-License-Identifier: GPL-2.0-only */

#include <libpayload.h>

uint64_t timer_hz(void)
{
	/* FIXME */
	return 1000000;
}

uint64_t timer_raw_value(void)
{
	uint64_t value;
	asm volatile ("csrr %0, time" : "=r" (value));
	return value;
}
