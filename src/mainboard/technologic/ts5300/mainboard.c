#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#include <arch/io.h>
#include "chip.h"

static int
mainboard_scan_bus(device_t root, int maxbus) 
{
	int retval;
	printk_spew("%s: root %p maxbus %d\n", __FUNCTION__, root, maxbus);
	retval = pci_scan_bus(root->bus, 0, 0xff, maxbus);
	printk_spew("DONE %s: return %d\n", __FUNCTION__, maxbus);
	return maxbus;
}

static struct device_operations mainboard_operations = {
	.read_resources   = root_dev_read_resources,
	.set_resources    = root_dev_set_resources,
	.enable_resources = root_dev_enable_resources,
	.init             = root_dev_init,
	.scan_bus         = mainboard_scan_bus,
};

static void enable_dev(device_t dev)
{
	dev->ops = &mainbaord_operations;
}

struct chip_operations mainboard_technologic_ts5300_control = {
	.enable_dev = enable_dev, 
	.name       = "Technologic Systems TS5300 mainboard ",
};

