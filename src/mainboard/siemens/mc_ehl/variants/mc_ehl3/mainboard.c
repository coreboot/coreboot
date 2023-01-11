/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <bootstate.h>
#include <device/pci_ops.h>
#include <gpio.h>
#include <soc/pci_devs.h>

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

	/* Limit SD-Card speed to DDR50 mode to avoid SDR104/SDR50 modes due to
	   layout limitations. */
	dev = pcidev_path_on_root(PCH_DEVFN_SDCARD);
	if (dev) {
		uint32_t reg;
		uint16_t reg16;
		struct resource *res = probe_resource(dev, PCI_BASE_ADDRESS_0);
		if (!res)
			return;
		write32(res2mmio(res, SD_CAP_BYP, 0), SD_CAP_BYP_EN);
		reg = read32(res2mmio(res, SD_CAP_BYP_REG1, 0));
		/* Disable SDR104 and SDR50 mode while keeping DDR50 mode enabled. */
		reg &= ~(SD_CAP_BYP_SDR104 | SD_CAP_BYP_SDR50);
		reg |= SD_CAP_BYP_DDR50;
		write32(res2mmio(res, SD_CAP_BYP_REG1, 0), reg);

		/* Use preset driver strength from preset value registers. */
		reg16 = read16(res2mmio(res, HOSTCTRL2, 0));
		reg16 |= HOSTCTRL2_PRESET;
		write16(res2mmio(res, HOSTCTRL2, 0), reg16);
	}
}

static void finalize_boot(void *unused)
{
	/* Set coreboot ready LED. */
	gpio_output(GPP_F20, 1);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, finalize_boot, NULL);
