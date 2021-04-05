/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci_ops.h>
#include <intelblocks/systemagent.h>
#include <soc/iomap.h>
#include <soc/p2sb.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/systemagent.h>
#include "chip.h"

static void systemagent_vtd_init(void)
{
	const struct device *const igd_dev = pcidev_path_on_root(SA_DEVFN_IGD);

	const bool vtd_capable =
		!(pci_read_config32(SA_DEV_ROOT, CAPID0_A) & VTD_DISABLE);
	if (!vtd_capable)
		return;

	/* Configure P2SB VT-d originators (HPET and IOAPIC) */
	pci_write_config16(PCH_DEV_P2SB, PCH_P2SB_HBDF, V_DEFAULT_HBDF);
	pci_write_config16(PCH_DEV_P2SB, PCH_P2SB_IBDF, V_DEFAULT_IBDF);

	if (igd_dev && igd_dev->enabled)
		sa_set_mch_bar(&soc_gfxvt_mmio_descriptor, 1);

	sa_set_mch_bar(&soc_vtvc0_mmio_descriptor, 1);
}

void systemagent_early_init(void)
{
	static const struct sa_mmio_descriptor soc_fixed_pci_resources[] = {
		{ MCHBAR, MCH_BASE_ADDRESS, MCH_BASE_SIZE, "MCHBAR" },
		{ DMIBAR, DMI_BASE_ADDRESS, DMI_BASE_SIZE, "DMIBAR" },
		{ EPBAR, EP_BASE_ADDRESS, EP_BASE_SIZE, "EPBAR" },
	};

	static const struct sa_mmio_descriptor soc_fixed_mch_resources[] = {
		{ GDXCBAR, GDXC_BASE_ADDRESS, GDXC_BASE_SIZE, "GDXCBAR" },
		{ EDRAMBAR, EDRAM_BASE_ADDRESS, EDRAM_BASE_SIZE, "EDRAMBAR" },
	};

	/* Set Fixed MMIO address into PCI configuration space */
	sa_set_pci_bar(soc_fixed_pci_resources,
			ARRAY_SIZE(soc_fixed_pci_resources));
	/* Set Fixed MMIO address into MCH base address */
	sa_set_mch_bar(soc_fixed_mch_resources,
			ARRAY_SIZE(soc_fixed_mch_resources));

	systemagent_vtd_init();

	/* Enable PAM registers */
	enable_pam_region();
}
