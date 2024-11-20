/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <device/pci_ids.h>
#include <gpio.h>
#include <intelblocks/pcr.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>

#define HOSTCTRL2		0x3E
#define  HOSTCTRL2_PRESET	(1 << 15)
#define SD_CAP_BYP		0x810
#define  SD_CAP_BYP_EN		0x5A
#define SD_CAP_BYP_REG1		0x814
#define  SD_CAP_BYP_SDR50	(1 << 13)
#define  SD_CAP_BYP_SDR104	(1 << 14)
#define  SD_CAP_BYP_DDR50	(1 << 15)

void variant_mainboard_final(void)
{
	struct device *dev;

	/* Limit SD card speed to DDR50 mode to avoid SDR104/SDR50 modes due to
	   layout limitations. */
	dev = pcidev_path_on_root(PCH_DEVFN_SDCARD);
	if (dev) {
		struct resource *res = probe_resource(dev, PCI_BASE_ADDRESS_0);
		if (!res)
			return;
		write32(res2mmio(res, SD_CAP_BYP, 0), SD_CAP_BYP_EN);
		clrsetbits32(res2mmio(res, SD_CAP_BYP_REG1, 0),
				SD_CAP_BYP_SDR104 | SD_CAP_BYP_SDR50,
				SD_CAP_BYP_DDR50);

		/* Use preset driver strength from preset value registers. */
		clrsetbits16(res2mmio(res, HOSTCTRL2, 0), 0, HOSTCTRL2_PRESET);
	}
}
