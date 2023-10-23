/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/graphics.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/resource.h>
#include <fsp/graphics.h>

void fsp_graphics_init(struct device *const dev)
{
	struct resource *res = probe_resource(dev, PCI_BASE_ADDRESS_0);

	if (res && res->base)
		fsp_report_framebuffer_info(res->base, LB_FB_ORIENTATION_NORMAL);
	else
		printk(BIOS_ERR, "%s: Unable to find resource for %s\n",
				__func__, dev_path(dev));
}
