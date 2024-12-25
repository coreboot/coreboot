/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <soc/emi.h>
#include <soc/irq2axi.h>
#include <soc/mt6316.h>
#include <soc/mt6363.h>
#include <soc/mt6373.h>
#include <soc/mt6685.h>
#include <soc/mtk_pwrsel.h>
#include <soc/pcie.h>
#include <soc/pmif.h>

void platform_romstage_main(void)
{
	irq2axi_disable();
	pwrsel_init();
	mtk_pmif_init();
	mt6363_init();
	mt6363_enable_vtref18(true);
	mt6363_set_vtref18_voltage(1800000);
	mt6316_init();
	mt6373_init();
	mt6685_init();
	mtk_dram_init();

	if (CONFIG(PCI))
		mtk_pcie_deassert_perst();
}
