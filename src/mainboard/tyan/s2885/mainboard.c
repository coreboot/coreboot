#include <console/console.h>
#include <device/device.h>
#include <device/chip.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "../../../northbridge/amd/amdk8/northbridge.h"
#include "chip.h"

unsigned long initial_apicid[CONFIG_MAX_CPUS] =
{
	0, 1
};

static struct device_operations mainboard_operations = {
        .read_resources   = root_dev_read_resources,
        .set_resources    = root_dev_set_resources,
        .enable_resources = enable_childrens_resources,
        .init             = 0,
        .scan_bus         = amdk8_scan_root_bus,
        .enable           = 0,
};

static void enumerate(struct chip *chip)
{
        struct chip *child;

	if (chip->control && chip->control->name) {
		printk_debug("Enumerating: %s\n", chip->control->name);
	}

	/* update device operation for dynamic root */
        dev_root.ops = &mainboard_operations;
        chip->dev = &dev_root;
        chip->bus = 0;
        for (child = chip->children; child; child = child->next) {
		child->bus = &dev_root.link[0];
        }
}

struct chip_control mainboard_tyan_s2885_control = {
        .enumerate = enumerate,
        .name      = "Tyan s2885 mainboard ",
};
