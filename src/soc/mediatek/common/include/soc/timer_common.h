/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_COMMON_TIMER_COMMON_H
#define SOC_MEDIATEK_COMMON_TIMER_COMMON_H

#include <types.h>

enum {
	GPT6_CON_EN		= BIT(0),
	GPT6_CON_CLR		= BIT(1),
	GPT6_MODE_FREERUN	= 3,
	GPT6_CLK_CLK6_SYS	= 0,
	GPT6_CLK_CLKDIV_DIV1	= 0,
};

/*
 * This is defined as weak no-ops that can be overridden by legacy SOCs. Some
 * legacy SOCs need specific settings before init timer. And we expect future
 * SOCs will not need it.
 */
void timer_prepare(void);

#endif
