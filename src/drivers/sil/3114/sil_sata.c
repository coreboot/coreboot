/* Copyright 2003-2004 Tyan Computer
 *  Yinghai Lu yhlu@tyan.com
 *
 * Copyright (C) 2010 Rudolf Marek <r.marek@assembler.cz>
 */

#include <delay.h>
#include <stdlib.h>
#include <string.h>
#include <arch/io.h>

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

static void si_sata_init(struct device *dev)
{
	uint32_t dword;
	/* enable change device id and class id */
	dword = pci_read_config32(dev,0x40);
	dword |= (1<<0);
	pci_write_config32(dev, 0x40, dword);
	/* Set IDE Class, Native mode, two drives per channel */
	dword = 0x01018f00;
	pci_write_config32(dev, 0x08, dword);
	/* disable change device id and class id*/
	dword = pci_read_config32(dev,0x40);
	dword &= ~(1<<0);
	pci_write_config32(dev, 0x40, dword);
	printk(BIOS_INFO, "SIL3114 set to IDE compatible mode\n");
}

static struct device_operations si_sata_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = si_sata_init,
	.scan_bus         = 0,
};

static const struct pci_driver si_sata_driver __pci_driver = {
        .ops    = &si_sata_ops,
        .vendor = 0x1095,
        .device = 0x3114,
};
