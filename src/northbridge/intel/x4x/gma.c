/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <commonlib/helpers.h>
#include <drivers/intel/gma/intel_bios.h>
#include <drivers/intel/gma/edid.h>
#include <drivers/intel/gma/i915.h>
#include <drivers/intel/gma/opregion.h>
#include <drivers/intel/gma/libgfxinit.h>
#include <pc80/vga.h>

#include "chip.h"
#include "drivers/intel/gma/i915_reg.h"
#include "x4x.h"

#define BASE_FREQUENCY 96000

static void gma_func0_init(struct device *dev)
{
	intel_gma_init_igd_opregion();

	if (!CONFIG(NO_GFX_INIT))
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	/* configure GMBUSFREQ */
	pci_update_config16(dev, 0xcc, ~0x1ff, 0xbc);

	int vga_disable = (pci_read_config16(dev, D0F0_GGC) & 2) >> 1;

	if (CONFIG(MAINBOARD_USE_LIBGFXINIT)) {
		if (vga_disable) {
			printk(BIOS_INFO,
			       "IGD is not decoding legacy VGA MEM and IO: skipping NATIVE graphic init\n");
		} else {
			int lightup_ok;
			gma_gfxinit(&lightup_ok);
		}
	} else {
		pci_dev_init(dev);
	}
}

static void gma_func0_disable(struct device *dev)
{
	struct device *dev_host = pcidev_on_root(0, 0);

	/* VGA cycles to discrete GPU */
	pci_or_config16(dev_host, D0F0_GGC, 1 << 1);
}

static void gma_generate_ssdt(const struct device *device)
{
	const struct northbridge_intel_x4x_config *chip = device->chip_info;

	drivers_intel_gma_displays_ssdt_generate(&chip->gfx);
}

static const char *gma_acpi_name(const struct device *dev)
{
	return "GFX0";
}

static struct device_operations gma_func0_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.acpi_fill_ssdt		= gma_generate_ssdt,
	.init			= gma_func0_init,
	.ops_pci		= &pci_dev_ops_pci,
	.vga_disable		= gma_func0_disable,
	.acpi_name		= gma_acpi_name,
};

static const unsigned short pci_device_ids[] = {
	0x2e02, /* Eaglelake */
	0x2e12, /* Q43/Q45 */
	0x2e22, /* G43/G45 */
	0x2e32, /* G41 */
	0x2e42, /* B43 */
	0x2e92, /* B43_I */
	0
};

static const struct pci_driver gma __pci_driver = {
	.ops = &gma_func0_ops,
	.vendor = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
