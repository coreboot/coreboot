#include <types.h>
#include <console.h>
#include <io.h>
#include <lib.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <arch/x86/cpu.h>
#include <statictree.h>
#include <config.h>
#include "cn700.h"

/*
 * This is the AGP 3.0 and PCI bridge @B0 Device 1 Func 0. When using AGP 3.0, the
 * config in this device takes presidence. We configure both just to be safe.
 */
static void pci_bridge_init(struct device *dev)
{
	printk(BIOS_DEBUG, "Setting up AGP bridge device\n");

	pci_write_config16(dev, 0x4, 0x0007);

	/* Secondary Bus Number */
	pci_write_config8(dev, 0x19, 0x01);
	/* Subordinate Bus Number */
	pci_write_config8(dev, 0x1a, 0x01);
	/* I/O Base */
	pci_write_config8(dev, 0x1c, 0xd0);
	/* I/O Limit */
	pci_write_config8(dev, 0x1d, 0xd0);

	/* Memory Base */
	pci_write_config16(dev, 0x20, 0xfb00);
	/* Memory Limit */
	pci_write_config16(dev, 0x22, 0xfcf0);
	/* Prefetchable Memory Base */
	pci_write_config16(dev, 0x24, 0xf400);
	/* Prefetchable Memory Limit */
	pci_write_config16(dev, 0x26, 0xf7f0);
	/* Enable VGA Compatible Memory/IO Range */
	pci_write_config8(dev, 0x3e, 0x08);

	/* Second PCI Bus Control (see datasheet) */
	pci_write_config8(dev, 0x40, 0x83);
	pci_write_config8(dev, 0x41, 0x43);
	pci_write_config8(dev, 0x42, 0xe2);
	pci_write_config8(dev, 0x43, 0x44);
	pci_write_config8(dev, 0x44, 0x34);
	pci_write_config8(dev, 0x45, 0x72);
}

struct device_operations cn700_pci_bridge = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_VIA,
				.device = PCI_DEVICE_ID_VIA_CN700_BRIDGE}}},
	.constructor			= default_device_constructor,
	.phase3_scan			= pci_scan_bridge,
	.phase4_read_resources		= pci_dev_read_resources,
	//.phase4_set_resources		= pci_dev_set_resources,
	//.phase5_enable_resources	= pci_dev_enable_resources,
	.phase6_init			= pci_bridge_init,
};
