/* Copyright 2003 Tyan */

/* Author: Yinghai Lu 
 *
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


static void ti_firewire_init(struct device *dev)
{
	uint16_t word;

        word = pci_read_config16(dev, 0x4);
        word |= ((1 << 2) |(1<<4)); // Command: 3--> 17
        pci_write_config16(dev, 0x4, word);

}

static struct device_operations ti_firewire_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = ti_firewire_init,
	.scan_bus         = 0,
};

static struct pci_driver ti_firewire_driver __pci_driver = {
        .ops    = &ti_firewire_ops,
        .vendor = 0x104c,
        .device = 0x8023,
};
 
