
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#include <arch/io.h>
#include "../../../northbridge/amd/amdk8/northbridge.h"
#include "chip.h"

static void mainboard_init(device_t dev)
{
	root_dev_init(dev);
}

static struct device_operations mainboard_operations = {
	.read_resources   = root_dev_read_resources,
	.set_resources    = root_dev_set_resources,
	.enable_resources = root_dev_enable_resources,
	.init             = mainboard_init,
	.scan_bus         = root_dev_scan_bus,
	.enable = 0,
};

static void enable_dev(struct device *dev)
{
	dev->ops = &mainboard_operations;
}

struct chip_operations mainboard_amd_solo_ops = {
	.name = "AMD Solo7 mainboard ",
	.enable_dev = enable_dev,
};
