/*
 * Copyright 2004 Tyan Computer
 *  by yhlu@tyan.com
 */

#include <console/console.h>

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "chip.h"

static void onboard_enable(device_t dev) 
{
	struct drivers_pci_onboard_config *conf;
        conf = dev->chip_info;
	dev->rom_address = conf->rom_address;
}

struct chip_operations drivers_pci_onboard_ops = {
#if CONFIG_CHIP_NAME == 1
	CHIP_NAME("Onboard PCI")
#endif
	.enable_dev = onboard_enable,
};
