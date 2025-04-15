/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/dcc.h>

void dcc_init(void)
{
	/* DSU */
	clrbits32p(BUS_PLLDIV_CFG1, GENMASK(20, 16));
	/* LCPU */
	clrbits32p(CPU_PLLDIV_0_CFG1, GENMASK(20, 16));
	/* MCPU */
	clrsetbits32p(CPU_PLLDIV_1_CFG1, GENMASK(20, 16), BIT(20) | GENMASK(18, 16));
	/* BCPU */
	clrsetbits32p(CPU_PLLDIV_2_CFG1, GENMASK(20, 16), GENMASK(17, 16) | GENMASK(20, 19));

	printk(BIOS_DEBUG, "[DCC] DSU=%#x, LCPU=%#x, MCPU=%#x, BCPU=%#x\n",
	       (read32p(BUS_PLLDIV_CFG1) >> 16) & 0x1F,
	       (read32p(CPU_PLLDIV_0_CFG1) >> 16) & 0x1F,
	       (read32p(CPU_PLLDIV_1_CFG1) >> 16) & 0x1F,
	       (read32p(CPU_PLLDIV_2_CFG1) >> 16) & 0x1F);
}
