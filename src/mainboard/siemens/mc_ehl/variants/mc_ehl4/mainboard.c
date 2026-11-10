/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <device/pci_ids.h>
#include <gpio.h>
#include <intelblocks/pcr.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>

#define HOSTCTRL2		0x3E
#define  HOSTCTRL2_PRESET	(1 << 15)

void variant_mainboard_final(void)
{
	struct device *dev;

	dev = pcidev_path_on_root(PCH_DEVFN_SDCARD);
	if (dev) {
		struct resource *res = probe_resource(dev, PCI_BASE_ADDRESS_0);
		if (res) {
			disable_sdr_modes(res);

			/* Use preset driver strength from preset value
			   registers. */
			clrsetbits16(res2mmio(res, HOSTCTRL2, 0), 0,
					HOSTCTRL2_PRESET);
		}
	}
}
