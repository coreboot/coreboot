/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <vendorcode/amd/opensil/opensil.h>

#include "chip.h"

void opensil_mpio_global_config(void)
{
	printk(BIOS_NOTICE, "openSIL stub: %s\n", __func__);
}

void opensil_mpio_per_device_config(struct device *dev)
{
	printk(BIOS_NOTICE, "openSIL stub: %s\n", __func__);
}
