/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Intel ICH8-M (82801HBM) D31:F1 IDE controller driver
 *
 * The ICH8-M has a dedicated PATA IDE controller at D31:F1 (PCI ID 0x2850),
 * separate from the SATA controller at D31:F2.  On the ThinkPad X61 this
 * controller drives the UltraBay device slot (optical drive).
 *
 * Ported from southbridge/intel/i82801gx/ide.c (ICH7).
 * Register layout is identical; only the PCI device ID differs.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include "chip.h"
#include "i82801hx.h"

static void ide_init(struct device *dev)
{
	u16 ideTimingConfig;
	u32 reg32;
	bool enable_primary, enable_secondary;

	const struct southbridge_intel_i82801hx_config *config = dev->chip_info;

	printk(BIOS_DEBUG, "i82801hx_ide: initializing...");

	if (config == NULL) {
		printk(BIOS_ERR, "\ni82801hx_ide: not in devicetree.cb, using safe defaults\n");
		enable_primary = true;
		enable_secondary = false;
	} else {
		enable_primary = config->ide_enable_primary;
		enable_secondary = config->ide_enable_secondary;
	}

	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_IO | PCI_COMMAND_MASTER);

	/* Native Capable, but not enabled in compatibility mode */
	pci_write_config8(dev, 0x09, 0x8a);

	/* ---- Primary channel ---- */
	ideTimingConfig = pci_read_config16(dev, IDE_TIM_PRI);
	ideTimingConfig &= ~IDE_DECODE_ENABLE;
	ideTimingConfig |= IDE_SITRE;
	if (enable_primary) {
		ideTimingConfig |= IDE_DECODE_ENABLE;
		ideTimingConfig |= IDE_ISP_3_CLOCKS;
		ideTimingConfig |= IDE_RCT_1_CLOCKS;
		ideTimingConfig |= IDE_IE0;
		ideTimingConfig |= IDE_TIME0;
		printk(BIOS_DEBUG, " IDE0");
	}
	pci_write_config16(dev, IDE_TIM_PRI, ideTimingConfig);

	/* ---- Secondary channel ---- */
	ideTimingConfig = pci_read_config16(dev, IDE_TIM_SEC);
	ideTimingConfig &= ~IDE_DECODE_ENABLE;
	ideTimingConfig |= IDE_SITRE;
	if (enable_secondary) {
		ideTimingConfig |= IDE_DECODE_ENABLE;
		ideTimingConfig |= IDE_ISP_3_CLOCKS;
		ideTimingConfig |= IDE_RCT_1_CLOCKS;
		ideTimingConfig |= IDE_IE0;
		ideTimingConfig |= IDE_TIME0;
		printk(BIOS_DEBUG, " IDE1");
	}
	pci_write_config16(dev, IDE_TIM_SEC, ideTimingConfig);

	/* ---- IDE I/O Configuration (DMA / signal drive strength) ---- */
	reg32 = 0;
	if (enable_primary)
		reg32 |= SIG_MODE_PRI_NORMAL | FAST_PCB0 | PCB0 | FAST_PCB1 | PCB1;
	if (enable_secondary)
		reg32 |= SIG_MODE_SEC_NORMAL | FAST_SCB0 | SCB0 | FAST_SCB1 | SCB1;
	pci_write_config32(dev, IDE_CONFIG, reg32);

	/* Interrupt line - set to 0xff (unrouted; OS assigns via PIRQ) */
	pci_write_config8(dev, PCI_INTERRUPT_LINE, 0xff);

	printk(BIOS_DEBUG, "\n");
}

static struct device_operations ide_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= ide_init,
	.ops_pci		= &pci_dev_ops_pci,
};

/* 82801HM/HEM (ICH8M / ICH8M-E) IDE controller */
static const struct pci_driver i82801hx_ide __pci_driver = {
	.ops	= &ide_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= PCI_DID_INTEL_82801HBM_IDE,
};
