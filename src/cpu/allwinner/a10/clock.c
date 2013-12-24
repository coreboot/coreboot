/*
 * Helpers for clock control and gating on Allwinner CPUs
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "clock.h"

#include <arch/io.h>

/**
 * \brief Enable the clock source for the peripheral
 *
 * @param[in] periph peripheral and clock type to enable @see a1x_clken
 */
void a1x_periph_clock_enable(enum a1x_clken periph)
{
	void *addr;
	u32 reg32;

	addr = (void *)A1X_CCM_BASE  + (periph >> 5);
	reg32 = read32(addr);
	reg32 |= 1 << (periph & 0x1f);
	write32(reg32, addr);
}

/**
 * \brief Disable the clock source for the peripheral
 *
 * @param[in] periph peripheral and clock type to enable @see a1x_clken
 */
void a1x_periph_clock_disable(enum a1x_clken periph)
{
	void *addr;
	u32 reg32;

	addr = (void *)A1X_CCM_BASE  + (periph >> 5);
	reg32 = read32(addr);
	reg32 &= ~(1 << (periph & 0x1f));
	write32(reg32, addr);
}
