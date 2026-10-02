/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <device/mmio.h>
#include <soc/pci_devs.h>

#define MMC_CAP_BYP		0x810
#define  MMC_CAP_BYP_EN		0x5A
#define MMC_CAP_BYP_REG1	0x814
#define  MMC_CAP_BYP_SDR50	(1 << 13)
#define  MMC_CAP_BYP_SDR104	(1 << 14)
#define  MMC_CAP_BYP_DDR50	(1 << 15)

/* Disable SDR104 and SDR50 mode while keeping DDR50 mode enabled. */
static void disable_sdr_modes(struct resource *res)
{
	write32(res2mmio(res, MMC_CAP_BYP, 0), MMC_CAP_BYP_EN);
	clrsetbits32(res2mmio(res, MMC_CAP_BYP_REG1, 0),
			MMC_CAP_BYP_SDR104 | MMC_CAP_BYP_SDR50,
			MMC_CAP_BYP_DDR50);
}

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
