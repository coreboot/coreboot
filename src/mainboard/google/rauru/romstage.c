/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <soc/emi.h>
#include <soc/irq2axi.h>
#include <soc/pcie.h>

void platform_romstage_main(void)
{
	irq2axi_disable();
	mtk_dram_init();

	if (CONFIG(PCI))
		mtk_pcie_deassert_perst();
}
