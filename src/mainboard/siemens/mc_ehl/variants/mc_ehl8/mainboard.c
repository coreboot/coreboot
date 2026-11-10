/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <soc/pci_devs.h>

void variant_mainboard_final(void)
{
	struct device *dev;

	dev = pcidev_path_on_root(PCH_DEVFN_EMMC);
	if (dev) {
		struct resource *res = probe_resource(dev, PCI_BASE_ADDRESS_0);
		if (res)
			disable_sdr_modes(res);
	}
}
