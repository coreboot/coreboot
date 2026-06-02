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

#define GEN2_TUNING_RP		(PCI_DEV(0, 0x1c, 0x02))
#define GEN2_LCTL2_LSTS2_REG	0x70
#define  GEN2_LCTL2_LSTS2_SD	(1 << 6)

#define GEN3_TUNING_BUS		1
#define GEN3_TUNING_MMIO_BASE	0xfe800000
#define GEN3_TUNING_RP		(PCI_DEV(0, 0x1c, 0x04))
#define GEN3_TUNING_DEV		(PCI_DEV(GEN3_TUNING_BUS, 0, 0))
#define GEN3_EQ_CONTROL_REG	0x8A8
#define  GEN3_EQ_PRESET_7	0x05008071

/* Disable SDR104 and SDR50 mode while keeping DDR50 mode enabled. */
static void disable_sdr_modes(struct resource *res)
{
	write32(res2mmio(res, MMC_CAP_BYP, 0), MMC_CAP_BYP_EN);
	clrsetbits32(res2mmio(res, MMC_CAP_BYP_REG1, 0),
			MMC_CAP_BYP_SDR104 | MMC_CAP_BYP_SDR50,
			MMC_CAP_BYP_DDR50);
}

void variant_mainboard_init(void)
{
	/* First set up root port bridge to get access to Gen3 device. */
	pci_s_write_config8(GEN3_TUNING_RP, PCI_SECONDARY_BUS, GEN3_TUNING_BUS);
	pci_s_write_config8(GEN3_TUNING_RP, PCI_SUBORDINATE_BUS, GEN3_TUNING_BUS);
	/* Define an address window to forward to the secondary bus. */
	pci_s_write_config32(GEN3_TUNING_RP, PCI_MEMORY_BASE,
			(GEN3_TUNING_MMIO_BASE | (GEN3_TUNING_MMIO_BASE >> 16)));
	/* Enable memory mapped transfers. */
	pci_s_write_config32(GEN3_TUNING_RP, PCI_COMMAND, PCI_COMMAND_MEMORY);
	/* Now the device is visible on bus 1, dev 0, function 0. */
	pci_s_write_config32(GEN3_TUNING_DEV, PCI_BASE_ADDRESS_0, GEN3_TUNING_MMIO_BASE);
	pci_s_write_config32(GEN3_TUNING_DEV, PCI_COMMAND, PCI_COMMAND_MEMORY);

	/* Selection of Preset 7 for transmitter of DSP for Gen3-Link. */
	pci_s_write_config32(GEN3_TUNING_DEV, GEN3_EQ_CONTROL_REG, GEN3_EQ_PRESET_7);

	/* Now delete all the changes made in config space so that the PCI enumerator can do
	   its work. */
	pci_s_write_config32(GEN3_TUNING_DEV, PCI_BASE_ADDRESS_0, 0);
	pci_s_write_config32(GEN3_TUNING_DEV, PCI_COMMAND, 0);
	pci_s_write_config32(GEN3_TUNING_RP, PCI_PRIMARY_BUS, 0x0);
	pci_s_write_config32(GEN3_TUNING_RP, PCI_MEMORY_BASE, 0);
	pci_s_write_config32(GEN3_TUNING_RP, PCI_COMMAND, 0);

	/* Select -3.5 dB de-emphasis for Gen2 (5.0 GT/s) */
	pci_s_write_config32(GEN2_TUNING_RP, GEN2_LCTL2_LSTS2_REG,
			pci_s_read_config32(GEN2_TUNING_RP, GEN2_LCTL2_LSTS2_REG) |
			GEN2_LCTL2_LSTS2_SD);
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

	dev = pcidev_path_on_root(PCH_DEVFN_EMMC);
	if (dev) {
		struct resource *res = probe_resource(dev, PCI_BASE_ADDRESS_0);
		if (res)
			disable_sdr_modes(res);
	}
}

static void finalize_boot(void *unused)
{
	/* Set coreboot ready LED. */
	gpio_output(GPP_F20, 1);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, finalize_boot, NULL);
