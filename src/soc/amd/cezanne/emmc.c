/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/aoac.h>
#include <device/device.h>
#include <soc/aoac_defs.h>

static void emmc_read_resources(struct device *dev)
{
	mmio_resource_kb(dev, 0, dev->path.mmio.addr / KiB, 4);
}

static void emmc_enable(struct device *dev)
{
	if (!dev->enabled)
		power_off_aoac_device(FCH_AOAC_DEV_EMMC);
}

struct device_operations cezanne_emmc_mmio_ops = {
	.read_resources = emmc_read_resources,
	.set_resources = noop_set_resources,
	.scan_bus = scan_static_bus,
	.enable = emmc_enable,
};
