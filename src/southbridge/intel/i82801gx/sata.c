/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include "chip.h"
#include "i82801gx.h"
#include "sata.h"

static u8 get_ich7_sata_ports(void)
{
	struct device *lpc;

	lpc = pcidev_on_root(31, 0);

	switch (pci_read_config16(lpc, PCI_DEVICE_ID)) {
	case 0x27b0:
	case 0x27b8:
		return 0xf;
	case 0x27b9:
	case 0x27bd:
		return 0x5;
	case 0x27bc:
		return 0x3;
	default:
		printk(BIOS_ERR, "i82801gx_sata: error: cannot determine port config\n");
		return 0;
	}
}

void sata_enable(struct device *dev)
{
	/* Get the chip configuration */
	struct southbridge_intel_i82801gx_config *config = dev->chip_info;

	if (config->sata_mode == SATA_MODE_AHCI) {
		/* Check if the southbridge supports AHCI */
		struct device *lpc_dev = pcidev_on_root(31, 0);
		if (!lpc_dev) {
			/* According to the PCI spec function 0 on a bus:device
			   needs to be active for other functions to be enabled.
			   Since SATA is on the same bus:device as the LPC
			   bridge, it makes little sense to continue. */
			die("Couldn't find the LPC device!\n");
		}

		const bool ahci_supported = !(pci_read_config32(lpc_dev, FDVCT)
					      & AHCI_UNSUPPORTED);

		if (!ahci_supported) {
			/* Fallback to IDE PLAIN for sata for the rest of the initialization */
			config->sata_mode = SATA_MODE_IDE_PLAIN;
			printk(BIOS_DEBUG, "AHCI not supported, falling back to plain mode.\n");
		}

	}

	if (config->sata_mode == SATA_MODE_AHCI) {
		/* Set map to ahci */
		pci_update_config8(dev, SATA_MAP, (u8)~0xc3, 0x40);
	} else {
		/* Set map to ide */
		pci_and_config8(dev, SATA_MAP, (u8)~0xc3);
	}
	/* At this point, the new pci id will appear on the bus */
}

static void sata_init(struct device *dev)
{
	u32 reg32;
	u8 ports;

	/* Get the chip configuration */
	const struct southbridge_intel_i82801gx_config *config = dev->chip_info;

	printk(BIOS_DEBUG, "i82801gx_sata: initializing...\n");

	if (config == NULL) {
		printk(BIOS_ERR, "i82801gx_sata: error: device not in devicetree.cb!\n");
		return;
	}

	/* Get ICH7 SATA port config */
	ports = get_ich7_sata_ports();

	/* Enable BARs */
	pci_write_config16(dev, PCI_COMMAND,
			   PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO);

	switch (config->sata_mode) {
	case SATA_MODE_IDE_LEGACY_COMBINED:
		printk(BIOS_DEBUG, "SATA controller in combined mode.\n");
		/* No AHCI: clear AHCI base */
		pci_write_config32(dev, PCI_BASE_ADDRESS_5, 0);

		/* And without AHCI BAR no memory decoding */
		pci_and_config16(dev, PCI_COMMAND, ~PCI_COMMAND_MEMORY);

		pci_write_config8(dev, 0x09, 0x80);

		/* Set timings */
		pci_write_config16(dev, IDE_TIM_PRI, IDE_DECODE_ENABLE |
				IDE_ISP_5_CLOCKS | IDE_RCT_4_CLOCKS);
		pci_write_config16(dev, IDE_TIM_SEC, IDE_DECODE_ENABLE |
				IDE_ISP_3_CLOCKS | IDE_RCT_1_CLOCKS |
				IDE_PPE0 | IDE_IE0 | IDE_TIME0);

		/* Sync DMA */
		pci_write_config16(dev, IDE_SDMA_CNT, IDE_SSDE0);
		pci_write_config16(dev, IDE_SDMA_TIM, 0x0200);

		/* Set IDE I/O Configuration */
		reg32 = SIG_MODE_PRI_NORMAL | FAST_PCB1 | FAST_PCB0 | PCB1 | PCB0;
		pci_write_config32(dev, IDE_CONFIG, reg32);

		/* Combine IDE - SATA configuration */
		pci_write_config8(dev, SATA_MAP, 0x02);

		/* Restrict ports - 0 and 2 only available */
		ports &= 0x5;
		break;
	case SATA_MODE_AHCI:
		printk(BIOS_DEBUG, "SATA controller in AHCI mode.\n");
		/* Allow both Legacy and Native mode */
		pci_write_config8(dev, 0x09, 0x8f);

		/* Set Interrupt Line */
		/* Interrupt Pin is set by D31IP.PIP */
		pci_write_config8(dev, INTR_LN, 0x0a);

		struct resource *ahci_res = probe_resource(dev, PCI_BASE_ADDRESS_5);
		if (ahci_res != NULL)
			/* write AHCI GHC_PI register */
			write32(res2mmio(ahci_res, 0xc, 0), config->sata_ports_implemented);
		break;
	default:
	case SATA_MODE_IDE_PLAIN:
		printk(BIOS_DEBUG, "SATA controller in plain mode.\n");
		/* Set Sata Controller Mode. No Mapping(?) */
		pci_write_config8(dev, SATA_MAP, 0x00);

		/* No AHCI: clear AHCI base */
		pci_write_config32(dev, PCI_BASE_ADDRESS_5, 0x00000000);

		/* And without AHCI BAR no memory decoding */
		pci_and_config16(dev, PCI_COMMAND, ~PCI_COMMAND_MEMORY);

		/* Native mode capable on both primary and secondary (0xa)
		 * or'ed with enabled (0x50) = 0xf
		 */
		pci_write_config8(dev, 0x09, 0x8f);

		/* Set Interrupt Line */
		/* Interrupt Pin is set by D31IP.PIP */
		pci_write_config8(dev, INTR_LN, 0xff);

		/* Set timings */
		pci_write_config16(dev, IDE_TIM_PRI, IDE_DECODE_ENABLE |
				IDE_ISP_3_CLOCKS | IDE_RCT_1_CLOCKS |
				IDE_PPE0 | IDE_IE0 | IDE_TIME0);
		pci_write_config16(dev, IDE_TIM_SEC, IDE_DECODE_ENABLE |
				IDE_SITRE | IDE_ISP_3_CLOCKS |
				IDE_RCT_1_CLOCKS | IDE_IE0 | IDE_TIME0);

		/* Sync DMA */
		pci_write_config16(dev, IDE_SDMA_CNT, IDE_SSDE0 | IDE_PSDE0);
		pci_write_config16(dev, IDE_SDMA_TIM, 0x0201);

		/* Set IDE I/O Configuration */
		reg32 = SIG_MODE_PRI_NORMAL | FAST_PCB1 | FAST_PCB0 | PCB1 | PCB0;
		pci_write_config32(dev, IDE_CONFIG, reg32);
		break;
	}

	/* Set port control */
	pci_write_config8(dev, SATA_PCS, ports);

	/* Enable clock gating for unused ports and set initialization reg */
	pci_write_config32(dev, SATA_IR, SIF3(ports) | SIF2 | SIF1 | SCRE);

	/* All configurations need this SATA initialization sequence */
	pci_write_config8(dev, 0xa0, 0x40);
	pci_write_config8(dev, 0xa6, 0x22);
	pci_write_config8(dev, 0xa0, 0x78);
	pci_write_config8(dev, 0xa6, 0x22);
	pci_write_config8(dev, 0xa0, 0x88);
	pci_update_config32(dev, 0xa4, 0xc0c0c0c0, 0x1b1b1212);
	pci_write_config8(dev, 0xa0, 0x8c);
	pci_update_config32(dev, 0xa4, 0xc0c0ff00, 0x121200aa);
	pci_write_config8(dev, 0xa0, 0x00);

	pci_write_config8(dev, PCI_INTERRUPT_LINE, 0);

	/* Sata Initialization Register */
	pci_or_config32(dev, SATA_IR, SCRD); // due to some bug
}

static struct device_operations sata_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= sata_init,
	.enable			= i82801gx_enable,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short sata_ids[] = {
	0x27c0, /* Desktop Non-AHCI and Non-RAID Mode: 82801GB/GR/GDH (ICH7/ICH7R/ICH7DH) */
	0x27c1, /* Desktop AHCI Mode: 82801GB/GR/GDH (ICH7/ICH7R/ICH7DH) */
	0x27c4, /* Mobile Non-AHCI and Non-RAID Mode: 82801GBM/GHM (ICH7-M/ICH7-M DH) */
	0x27c5, /* Mobile AHCI Mode: 82801GBM/GHM (ICH7-M/ICH7-M DH) */
	/* NOTE: Any of the below are not properly supported yet. */
	0x27c3, /* Desktop RAID mode: 82801GB/GR/GDH (ICH7/ICH7R/ICH7DH) */
	0x27c6, /* ICH7M DH Raid Mode: 82801GHM (ICH7-M DH) */
	0
};

static const struct pci_driver i82801gx_sata_driver __pci_driver = {
	.ops		= &sata_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= sata_ids,
};
