/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/graphics.h>
#include <amdblocks/vbt.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/graphics.h>

void fsp_graphics_init(struct device *const dev)
{
	struct resource *res = probe_resource(dev, PCI_BASE_ADDRESS_0);

	if (res && res->base)
		fsp_report_framebuffer_info(res->base, LB_FB_ORIENTATION_NORMAL);
	else
		printk(BIOS_ERR, "%s: Unable to find resource for %s\n",
				__func__, dev_path(dev));

	/*
	 * Calculate and set checksum for VBIOS data if FSP GOP driver used,
	 * Since GOP driver modifies ATOMBIOS tables at end of BS_DEV_RESOURCES.
	 * While Linux does not verify the checksum the Windows kernel driver does.
	 */
	struct rom_header *vbios = (struct rom_header *)vbt_get();
	if (!vbios || !vbios->size) {
		printk(BIOS_ERR, "%s: No VGA BIOS loaded for %s\n",
				__func__, dev_path(dev));
		return;
	}

	uint8_t *data = (uint8_t *)vbios;

	/* Clear existing checksum before recalculating */
	data[VFCT_VBIOS_CHECKSUM_OFFSET] = 0;
	data[VFCT_VBIOS_CHECKSUM_OFFSET] =
			acpi_checksum(data, vbios->size * 512);
}
