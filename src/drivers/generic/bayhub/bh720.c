/* SPDX-License-Identifier: GPL-2.0-only */

/* Driver for BayHub Technology BH720 PCI to eMMC 5.0 HS200 bridge */

#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/path.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include "chip.h"
#include "bh720.h"

static void bh720_program_hs200_mode(struct device *dev)
{
	u32 sdbar;
	u32 bh720_pcr_data;

	sdbar = pci_read_config32(dev, PCI_BASE_ADDRESS_1);

	/* Enable Memory Access Function */
	write32((void *)(sdbar + BH720_MEM_ACCESS_EN), 0x40000000);
	write32((void *)(sdbar + BH720_MEM_RW_DATA), 0x80000000);
	write32((void *)(sdbar + BH720_MEM_RW_ADR), 0x800000D0);

	/* Set EMMC VCCQ 1.8V PCR 0x308[4] */
	write32((void *)(sdbar + BH720_MEM_RW_ADR),
		BH720_MEM_RW_READ | BH720_PCR_EMMC_SETTING);
	bh720_pcr_data = read32((void *)(sdbar + BH720_MEM_RW_DATA));
	write32((void *)(sdbar + BH720_MEM_RW_DATA),
		bh720_pcr_data | BH720_PCR_EMMC_SETTING_1_8V);
	write32((void *)(sdbar + BH720_MEM_RW_ADR),
		BH720_MEM_RW_WRITE | BH720_PCR_EMMC_SETTING);

	/* Set Base clock to 200MHz(PCR 0x304[31:16] = 0x2510) */
	write32((void *)(sdbar + BH720_MEM_RW_ADR),
		BH720_MEM_RW_READ | BH720_PCR_DrvStrength_PLL);
	bh720_pcr_data = read32((void *)(sdbar + BH720_MEM_RW_DATA));
	bh720_pcr_data &= 0x0000FFFF;
	bh720_pcr_data |= 0x2510 << 16;
	write32((void *)(sdbar + BH720_MEM_RW_DATA), bh720_pcr_data);
	write32((void *)(sdbar + BH720_MEM_RW_ADR),
		BH720_MEM_RW_WRITE | BH720_PCR_DrvStrength_PLL);

	/* Use PLL Base clock PCR 0x3E4[22] = 1 */
	write32((void *)(sdbar + BH720_MEM_RW_ADR),
		BH720_MEM_RW_READ | BH720_PCR_CSR);
	bh720_pcr_data = read32((void *)(sdbar + BH720_MEM_RW_DATA));
	write32((void *)(sdbar + BH720_MEM_RW_DATA),
		bh720_pcr_data | BH720_PCR_CSR_EMMC_MODE_SEL);
	write32((void *)(sdbar + BH720_MEM_RW_ADR),
		BH720_MEM_RW_WRITE | BH720_PCR_CSR);

	/* Disable Memory Access */
	write32((void *)(sdbar + BH720_MEM_RW_DATA), 0x80000001);
	write32((void *)(sdbar + BH720_MEM_RW_ADR), 0x800000D0);
	write32((void *)(sdbar + BH720_MEM_ACCESS_EN), 0x80000000);
}

static void bh720_init(struct device *dev)
{
	struct drivers_generic_bayhub_config *config = dev->chip_info;

	pci_dev_init(dev);

	if (config && config->power_saving) {
		/*
		 * This procedure for enabling power-saving mode is from the
		 * BayHub BIOS Implementation Guideline document.
		 */
		pci_write_config32(dev, BH720_PROTECT,
				   BH720_PROTECT_OFF | BH720_PROTECT_LOCK_OFF);
		pci_or_config32(dev, BH720_RTD3_L1, BH720_RTD3_L1_DISABLE_L1);
		pci_or_config32(dev, BH720_LINK_CTRL,
				BH720_LINK_CTRL_L0_ENABLE |
				BH720_LINK_CTRL_L1_ENABLE);
		pci_or_config32(dev, BH720_LINK_CTRL, BH720_LINK_CTRL_CLKREQ);
		pci_update_config32(dev, BH720_MISC2, ~BH720_MISC2_ASPM_DISABLE,
				    BH720_MISC2_APSM_CLKREQ_L1 |
				    BH720_MISC2_APSM_PHY_L1);
		pci_write_config32(dev, BH720_PROTECT,
				   BH720_PROTECT_ON | BH720_PROTECT_LOCK_ON);

		printk(BIOS_INFO, "BayHub BH720: Power-saving enabled (link_ctrl=%#x)\n",
		       pci_read_config32(dev, BH720_LINK_CTRL));
	}

	if (config && !config->disable_hs200_mode)
		bh720_program_hs200_mode(dev);

	if (config && config->vih_tuning_value) {
		/* Tune VIH */
		u32 bh720_pcr_data;
		pci_write_config32(dev, BH720_PROTECT,
			BH720_PROTECT_OFF | BH720_PROTECT_LOCK_OFF);
		bh720_pcr_data = pci_read_config32(dev, BH720_PCR_DrvStrength_PLL);
		bh720_pcr_data &= 0xFFFFFF00;
		bh720_pcr_data |= config->vih_tuning_value;
		pci_write_config32(dev, BH720_PCR_DrvStrength_PLL, bh720_pcr_data);
		pci_write_config32(dev, BH720_PROTECT,
			BH720_PROTECT_ON | BH720_PROTECT_LOCK_ON);
	}
}

static struct device_operations bh720_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.ops_pci		= &pci_dev_ops_pci,
	.init			= bh720_init,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_O2_BH720,
	0
};

static const struct pci_driver bayhub_bh720 __pci_driver = {
	.ops		= &bh720_ops,
	.vendor		= PCI_VENDOR_ID_O2,
	.devices	= pci_device_ids,
};

struct chip_operations drivers_generic_bayhub_ops = {
	CHIP_NAME("BayHub Technology BH720 PCI to eMMC 5.0 HS200 bridge")
};
