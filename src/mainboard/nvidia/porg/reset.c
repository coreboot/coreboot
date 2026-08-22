/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <reset.h>
#include <soc/addressmap.h>
#include <soc/pmc.h>

void do_board_reset(void)
{
	struct tegra_pmc_regs *const pmc = (void *)(uintptr_t)TEGRA_PMC_BASE;

	setbits32(&pmc->cntrl, PMC_CNTRL_MAIN_RST);
}
