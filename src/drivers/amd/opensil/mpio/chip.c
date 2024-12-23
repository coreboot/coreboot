/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <vendorcode/amd/opensil/opensil.h>

#include "chip.h"
#include "../opensil.h"

struct chip_operations drivers_amd_opensil_mpio_ops = {
	.name = "AMD MPIO",
};

void configure_mpio(void)
{
	opensil_mpio_global_config();

	/* Find all devices with this chip that are directly below the chip */
	for (struct device *dev = &dev_root; dev; dev = dev->next)
		if (dev->chip_ops == &drivers_amd_opensil_mpio_ops &&
		    dev->chip_info != dev->upstream->dev->chip_info)
			opensil_mpio_per_device_config(dev);
}
