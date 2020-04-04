/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __SOC_CAVIUM_CN81XX_TIMER_H__
#define __SOC_CAVIUM_CN81XX_TIMER_H__

#include <stdint.h>
#include <types.h>

/* Watchdog functions */
void watchdog_set(const size_t index, unsigned int timeout_ms);
void watchdog_poke(const size_t index);
void watchdog_disable(const size_t index);
int watchdog_is_running(const size_t index);

/* Timer functions */
void soc_timer_init(void);

#endif	/* __SOC_CAVIUM_CN81XX_TIMER_H__ */
