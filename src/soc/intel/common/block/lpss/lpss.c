/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <intelblocks/lpss.h>

/* Clock register */
#define LPSS_CLOCK_CTL_REG	0x200
#define LPSS_CNT_CLOCK_EN	1
#define LPSS_CNT_CLK_UPDATE	(1 << 31)
#define LPSS_CLOCK_DIV_N(n)	(((n) & 0x7fff) << 16)
#define LPSS_CLOCK_DIV_M(m)	(((m) & 0x7fff) << 1)

/* reset register  */
#define LPSS_RESET_CTL_REG	0x204

/*
 * Bit 1:0 controls LPSS controller reset.
 *
 * 00 ->LPSS Host Controller is in reset (Reset Asserted)
 * 01/10 ->Reserved
 * 11 ->LPSS Host Controller is NOT at reset (Reset Released)
 */

#define LPSS_CNT_RST_RELEASE	3

/* DMA Software Reset Control */
#define LPSS_DMA_RST_RELEASE	(1 << 2)

bool lpss_is_controller_in_reset(uintptr_t base)
{
	uint8_t *addr = (void *)base;
	uint32_t val = read32(addr + LPSS_RESET_CTL_REG);

	if (val == 0xFFFFFFFF)
		return true;

	return !(val & LPSS_CNT_RST_RELEASE);
}

void lpss_reset_release(uintptr_t base)
{
	uint8_t *addr = (void *)base;

	/* Take controller out of reset */
	write32(addr + LPSS_RESET_CTL_REG, LPSS_CNT_RST_RELEASE);
}

void lpss_clk_update(uintptr_t base, uint32_t clk_m_val, uint32_t clk_n_val)
{
	uint8_t *addr = (void *)base;
	uint32_t clk_sel;

	addr += LPSS_CLOCK_CTL_REG;
	clk_sel = LPSS_CLOCK_DIV_N(clk_n_val) | LPSS_CLOCK_DIV_M(clk_m_val);

	write32(addr, clk_sel | LPSS_CNT_CLK_UPDATE);
	write32(addr, clk_sel | LPSS_CNT_CLOCK_EN);
}
