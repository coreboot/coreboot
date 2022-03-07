/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/io.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/smbus.h>
#include "chip.h"
#include "i82371eb.h"

static void pwrmgt_enable(struct device *dev)
{
	struct southbridge_intel_i82371eb_config *sb = dev->chip_info;
	u32 reg, gpo = sb->gpo;

	/* Sets the base address of power management ports. */
	pci_write_config16(dev, PMBA, DEFAULT_PMBASE | 1);

	/* Set Power Management IO Space Enable bit */
	u8 val = pci_read_config8(dev, PMREGMISC);
	pci_write_config8(dev, PMREGMISC, val | 1);

	/* set global control:
	 * bit25 (lid_pol): 1=invert lid polarity
	 * bit24 (sm_freeze): 1=freeze idle and standby timers
	 * bit16 (end of smi): 0=disable smi assertion (cleared by hw)
	 * bits8-15,26: global standby timer initial count 127 * 4minutes
	 * bit2  (thrm_pol): 1=active low THRM#
	 * bit0  (smi_en): 1=disable smi generation upon smi event
	 */
	reg = (sb->lid_polarity<<25)|
	      (1<<24)|
	      (0xff<<8)|
	      (sb->thrm_polarity<<2);
	outl(reg, DEFAULT_PMBASE + GLBCTL);

	/* set processor control:
	 * bit12 (stpclk_en): 1=enable stopping of host clk on lvl3
	 * bit11 (sleep_en): 1=enable slp# assertion on lvl3
	 * bit9 (cc_en): 1=enable clk control with lvl2 and lvl3 regs
	 */
	outl(0, DEFAULT_PMBASE + PCNTRL);

	/* disable smi event enables */
	outw(0, DEFAULT_PMBASE + GLBEN);
	outl(0, DEFAULT_PMBASE + DEVCTL);

	/* set default gpo value.
	 * power-on default is 0x7fffbfffh */
	if (gpo) {
		/* only 8bit access allowed */
		outb(gpo        & 0xff, DEFAULT_PMBASE + GPO0);
		outb((gpo >>  8) & 0xff, DEFAULT_PMBASE + GPO1);
		outb((gpo >> 16) & 0xff, DEFAULT_PMBASE + GPO2);
		outb((gpo >> 24) & 0xff, DEFAULT_PMBASE + GPO3);
	} else {
		printk(BIOS_SPEW,
		       "%s: gpo default missing in devicetree.cb!\n", __func__);
	}

	/* Clear status events. */
	outw(0xffff,     DEFAULT_PMBASE + PMSTS);
	outw(0xffff,     DEFAULT_PMBASE + GPSTS);
	outw(0xffff,     DEFAULT_PMBASE + GLBSTS);
	outl(0xffffffff, DEFAULT_PMBASE + DEVSTS);

	/* set PMCNTRL default */
	outw(SUS_TYP_S0|SCI_EN, DEFAULT_PMBASE + PMCNTRL);
}

static void pwrmgt_read_resources(struct device *dev)
{
	struct resource *res;

	pci_dev_read_resources(dev);

	res = new_resource(dev, 1);
	res->base = DEFAULT_PMBASE;
	res->size = 0x0040;
	res->limit = 0xffff;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED |
	             IORESOURCE_RESERVE;

	res = new_resource(dev, 2);
	res->base = SMBUS_IO_BASE;
	res->size = 0x0010;
	res->limit = 0xffff;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED |
	             IORESOURCE_RESERVE;
}

static const struct smbus_bus_operations lops_smbus_bus = {
};

static const struct device_operations smbus_ops = {
	.read_resources		= pwrmgt_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.scan_bus		= scan_smbus,
	.enable			= pwrmgt_enable,
	.ops_pci		= 0, /* No subsystem IDs on 82371EB! */
	.ops_smbus_bus		= &lops_smbus_bus,
};

/* Note: There's no SMBus on 82371FB/SB/MX and 82437MX. */

/* Intel 82371AB/EB/MB */
static const struct pci_driver smbus_driver __pci_driver = {
	.ops	= &smbus_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= PCI_DID_INTEL_82371AB_SMB_ACPI,
};
