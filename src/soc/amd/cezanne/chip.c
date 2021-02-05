/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/device.h>
#include <fsp/api.h>
#include <soc/southbridge.h>
#include <types.h>
#include "chip.h"

static struct device_operations pci_domain_ops = {
	.read_resources	= pci_domain_read_resources,
	.set_resources	= pci_domain_set_resources,
	.scan_bus	= pci_domain_scan_bus,
};

static void enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	switch (dev->path.type) {
	case DEVICE_PATH_DOMAIN:
		dev->ops = &pci_domain_ops;
		break;
	default:
		break;
	}
}

static void soc_init(void *chip_info)
{
	fsp_silicon_init(acpi_is_wakeup_s3());

	fch_init(chip_info);
}

static void soc_final(void *chip_info)
{
	fch_final(chip_info);
}

struct chip_operations soc_amd_cezanne_ops = {
	CHIP_NAME("AMD Cezanne SoC")
	.enable_dev = enable_dev,
	.init = soc_init,
	.final = soc_final
};
