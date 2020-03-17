/*
 * This file is part of the coreboot project.
 *
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

#ifndef __SOC_MEDIATEK_MT8173_MIPI_H__
#define __SOC_MEDIATEK_MT8173_MIPI_H__

#include <soc/addressmap.h>
#include <types.h>

struct mipi_rx_regs {
	uint32_t rev0[19];
	uint32_t mipi_rx_ana4c;
	uint32_t mipi_rx_ana50;
	uint32_t rev1;
};

check_member(mipi_rx_regs, mipi_rx_ana4c, 0x4c);
check_member(mipi_rx_regs, rev1, 0x54);

static struct mipi_rx_regs *const mt8173_mipi = (void *)(ANA_MIPI_CS1_BASE);

#endif	/* __SOC_MEDIATEK_MT8173_MIPI_H__ */
