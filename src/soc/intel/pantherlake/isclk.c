/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelblocks/pcr.h>
#include <soc/iomap.h>
#include <soc/isclk.h>
#include <soc/pcr_ids.h>

#define ISCLK_PCR_BIOS_BUFFEN_H 0x8080

/* Disable PCIe clock source; clock_number: 0-based */
void soc_disable_pcie_clock_out(size_t clock_number)
{
	pcr_rmw32(PID_ISCLK, ISCLK_PCR_BIOS_BUFFEN_H, ~BIT(clock_number), 0);
}

/* Enable PCIe clock source; clock_number: 0-based */
void soc_enable_pcie_clock_out(size_t clock_number)
{
	pcr_or32(PID_ISCLK, ISCLK_PCR_BIOS_BUFFEN_H, BIT(clock_number));
}
