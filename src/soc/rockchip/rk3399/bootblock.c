/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <bootblock_common.h>
#include <soc/grf.h>
#include <soc/mmu_operations.h>
#include <soc/clock.h>

void bootblock_soc_init(void)
{
	rkclk_init();

	/* all ddr range non-secure */
	write32(&rk3399_pmusgrf->ddr_rgn_con[16], 0xff << 16 | 0);

	/* tzma_rosize = 0, all sram non-secure */
	write32(&rk3399_pmusgrf->soc_con4, 0x3ff << 16 | 0);

	/* emmc master secure */
	write32(&rk3399_pmusgrf->soc_con7, 1 << 23 | 1 << 24 | 0 << 8 | 0 << 7);

	/* glb_slv_secure_bypass */
	write32(&rk3399_pmusgrf->pmu_slv_con0, 1 << 16 | 1);
}
