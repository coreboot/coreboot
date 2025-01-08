/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <bootstate.h>
#include <device/pci_ids.h>
#include <gpio.h>
#include <intelblocks/pcr.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>

#define HOSTCTRL2		0x3E
#define  HOSTCTRL2_PRESET	(1 << 15)
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

static void finalize_boot(void *unused)
{
	/* Set coreboot ready LED. */
	gpio_output(GPP_F20, 1);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, finalize_boot, NULL);
