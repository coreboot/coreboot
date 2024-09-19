/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <soc/pcie.h>

void platform_romstage_main(void)
{
	/* TODO: add romstage main function */

	if (CONFIG(PCI))
		mtk_pcie_deassert_perst();
}
