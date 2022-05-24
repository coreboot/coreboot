/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <edid.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/vga.h>

#include "../common/aspeed_coreboot.h"
#include "../common/ast_drv.h"

static void aspeed_ast2050_read_resources(struct device *dev)
{
	/* Reserve VGA regions */
	mmio_resource_kb(dev, 3, 0xa0000 >> 10, 0x1ffff >> 10);

	/* Run standard resource read routine */
	pci_dev_read_resources(dev);
}

static void aspeed_ast2050_init(struct device *dev)
{
	struct drm_device drm_dev;

	drm_dev.pdev = dev;

	printk(BIOS_INFO, "ASpeed AST2050: initializing video device\n");
	ast_driver_load(&drm_dev, 0);

	/* Unlock extended configuration registers */
	outb(0x80, 0x3d4); outb(0xa8, 0x3d5);

	/* Set CRT Request Threshold */
	outb(0xa6, 0x3d4); outb(0x2f, 0x3d5);
	outb(0xa7, 0x3d4); outb(0x3f, 0x3d5);

	if (CONFIG(VGA_TEXT_FRAMEBUFFER)) {
		/* Initialize standard VGA text mode */
		vga_io_init();

		vga_textmode_init();
		printk(BIOS_INFO, "ASpeed VGA text mode initialized\n");

		/* if we don't have console, at least print something... */
		vga_line_write(0, "ASpeed VGA text mode initialized");
	} else if (CONFIG(GENERIC_LINEAR_FRAMEBUFFER)) {
		ast_driver_framebuffer_init(&drm_dev, 0);
		printk(BIOS_INFO, "ASpeed high resolution framebuffer initialized\n");
	}
}

static struct device_operations aspeed_ast2050_ops  = {
	.read_resources   = aspeed_ast2050_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = aspeed_ast2050_init,
};

static const struct pci_driver aspeed_ast2050_driver __pci_driver = {
	.ops    = &aspeed_ast2050_ops,
	.vendor = PCI_VID_ASPEED,
	.device = PCI_DID_ASPEED_AST2050_VGA,
};
