/* SPDX-License-Identifier: GPL-2.0-or-later */

/* TODO: Check if this really works for all of the southbridges. */

#include <stdint.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include "chip.h"
#include "i82371eb.h"

/**
 * Initialize the IDE controller.
 *
 * Depending on the configuration variables 'ide0_enable' and 'ide1_enable'
 * enable or disable the primary and secondary IDE interface, respectively.
 *
 * Depending on the configuration variable 'ide_legacy_enable' enable or
 * disable access to the legacy IDE ports and the PCI Bus Master IDE I/O
 * registers (this is required for e.g. FILO).
 *
 * @param dev The device to use.
 */
static void ide_init_enable(struct device *dev)
{
	u16 reg16;
	struct southbridge_intel_i82371eb_config *conf = dev->chip_info;

	/* Enable/disable the primary IDE interface. */
	reg16 = pci_read_config16(dev, IDETIM_PRI);
	reg16 = ONOFF(conf->ide0_enable, reg16, IDE_DECODE_ENABLE);
	pci_write_config16(dev, IDETIM_PRI, reg16);
	printk(BIOS_DEBUG, "IDE: %s IDE interface: %s\n", "Primary",
		     conf->ide0_enable ? "on" : "off");

	/* Enable/disable the secondary IDE interface. */
	reg16 = pci_read_config16(dev, IDETIM_SEC);
	reg16 = ONOFF(conf->ide1_enable, reg16, IDE_DECODE_ENABLE);
	pci_write_config16(dev, IDETIM_SEC, reg16);
	printk(BIOS_DEBUG, "IDE: %s IDE interface: %s\n", "Secondary",
		     conf->ide1_enable ? "on" : "off");

	/* Enable access to the legacy IDE ports (both primary and secondary),
	 * and the PCI Bus Master IDE I/O registers.
	 * Only do this if at least one IDE interface is enabled.
	 */
	if (conf->ide0_enable || conf->ide1_enable) {
		reg16 = pci_read_config16(dev, PCI_COMMAND);
		reg16 = ONOFF(conf->ide_legacy_enable, reg16,
			      (PCI_COMMAND_IO | PCI_COMMAND_MASTER));
		pci_write_config16(dev, PCI_COMMAND, reg16);
		printk(BIOS_DEBUG, "IDE: Access to legacy IDE ports: %s\n",
			     conf->ide_legacy_enable ? "on" : "off");
	}
}

/**
 * Initialize the Ultra DMA/33 support of the IDE controller.
 *
 * Depending on the configuration variables 'ide0_drive0_udma33_enable',
 * 'ide0_drive1_udma33_enable', 'ide1_drive0_udma33_enable', and
 * 'ide1_drive1_udma33_enable' enable or disable Ultra DMA/33 support for
 * the respective IDE controller and drive.
 *
 * Only do that if the respective controller is actually enabled, of course.
 *
 * @param dev The device to use.
 */
static void ide_init_udma33(struct device *dev)
{
	u8 reg8;
	struct southbridge_intel_i82371eb_config *conf = dev->chip_info;

	/* Enable/disable UDMA/33 operation (primary IDE interface). */
	if (conf->ide0_enable) {
		reg8 = pci_read_config8(dev, UDMACTL);
		reg8 = ONOFF(conf->ide0_drive0_udma33_enable, reg8, PSDE0);
		reg8 = ONOFF(conf->ide0_drive1_udma33_enable, reg8, PSDE1);
		pci_write_config8(dev, UDMACTL, reg8);

		printk(BIOS_DEBUG, "IDE: %s, drive %d: UDMA/33: %s\n",
			     "Primary IDE interface", 0,
			     conf->ide0_drive0_udma33_enable ? "on" : "off");
		printk(BIOS_DEBUG, "IDE: %s, drive %d: UDMA/33: %s\n",
			     "Primary IDE interface", 1,
			     conf->ide0_drive1_udma33_enable ? "on" : "off");
	}

	/* Enable/disable Ultra DMA/33 operation (secondary IDE interface). */
	if (conf->ide1_enable) {
		reg8 = pci_read_config8(dev, UDMACTL);
		reg8 = ONOFF(conf->ide1_drive0_udma33_enable, reg8, SSDE0);
		reg8 = ONOFF(conf->ide1_drive1_udma33_enable, reg8, SSDE1);
		pci_write_config8(dev, UDMACTL, reg8);

		printk(BIOS_DEBUG, "IDE: %s, drive %d: UDMA/33: %s\n",
			     "Secondary IDE interface", 0,
			     conf->ide1_drive0_udma33_enable ? "on" : "off");
		printk(BIOS_DEBUG, "IDE: %s, drive %d: UDMA/33: %s\n",
			     "Secondary IDE interface", 1,
			     conf->ide1_drive1_udma33_enable ? "on" : "off");
	}
}

/**
 * IDE init for the Intel 82371FB/SB IDE controller.
 *
 * These devices do not support UDMA/33, so don't attempt to enable it.
 *
 * @param dev The device to use.
 */
static void ide_init_i82371fb_sb(struct device *dev)
{
	ide_init_enable(dev);
}

/**
 * IDE init for the Intel 82371AB/EB/MB IDE controller.
 *
 * @param dev The device to use.
 */
static void ide_init_i82371ab_eb_mb(struct device *dev)
{
	ide_init_enable(dev);
	ide_init_udma33(dev);
}

/* Intel 82371FB/SB */
static const struct device_operations ide_ops_fb_sb = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= ide_init_i82371fb_sb,
	.ops_pci		= 0, /* No subsystem IDs on 82371XX! */
};

/* Intel 82371AB/EB/MB */
static const struct device_operations ide_ops_ab_eb_mb = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= ide_init_i82371ab_eb_mb,
	.ops_pci		= 0, /* No subsystem IDs on 82371XX! */
};

/* Intel 82371FB (PIIX) */
static const struct pci_driver ide_driver_fb __pci_driver = {
	.ops	= &ide_ops_fb_sb,
	.vendor	= PCI_VID_INTEL,
	.device	= PCI_DID_INTEL_82371FB_IDE,
};

/* Intel 82371SB (PIIX3) */
static const struct pci_driver ide_driver_sb __pci_driver = {
	.ops	= &ide_ops_fb_sb,
	.vendor	= PCI_VID_INTEL,
	.device	= PCI_DID_INTEL_82371SB_IDE,
};

/* Intel 82371MX (MPIIX) */
static const struct pci_driver ide_driver_mx __pci_driver = {
	.ops	= &ide_ops_fb_sb,
	.vendor	= PCI_VID_INTEL,
	.device	= PCI_DID_INTEL_82371MX_ISA_IDE,
};

/* Intel 82437MX (part of the 430MX chipset) */
static const struct pci_driver ide_driver_82437mx __pci_driver = {
	.ops	= &ide_ops_fb_sb,
	.vendor	= PCI_VID_INTEL,
	.device	= PCI_DID_INTEL_82437MX_ISA_IDE,
};

/* Intel 82371AB/EB/MB */
static const struct pci_driver ide_driver_ab_eb_mb __pci_driver = {
	.ops	= &ide_ops_ab_eb_mb,
	.vendor	= PCI_VID_INTEL,
	.device	= PCI_DID_INTEL_82371AB_IDE,
};
