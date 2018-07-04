/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Stefan Reinauer <stefan.reinauer@coreboot.org>
 * Copyright (C) 2010 Kevin O'Connor <kevin@koconnor.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/keyboard.h>
#include <arch/io.h>
#include <console/console.h>
#include <drivers/intel/gma/i915.h>

#define Q35_PAM0            0x90

static const unsigned char qemu_q35_irqs[] = {
	10, 10, 11, 11,
	10, 10, 11, 11,
};

struct i915_gpu_controller_info gfx_controller_info = {
	.ndid = 3,
	.did = {
		0x80000100, 0x80000240, 0x80000410, 0x80000410, 0x00000005
	}
};

const struct i915_gpu_controller_info *
intel_gma_get_controller_info(void)
{
	return &gfx_controller_info;
}

static void qemu_nb_init(struct device *dev)
{
	/* Map memory at 0xc0000 - 0xfffff */
	int i;
	uint8_t v = pci_read_config8(dev, Q35_PAM0);
	v |= 0x30;
	pci_write_config8(dev, Q35_PAM0, v);
	pci_write_config8(dev, Q35_PAM0 + 1, 0x33);
	pci_write_config8(dev, Q35_PAM0 + 2, 0x33);
	pci_write_config8(dev, Q35_PAM0 + 3, 0x33);
	pci_write_config8(dev, Q35_PAM0 + 4, 0x33);
	pci_write_config8(dev, Q35_PAM0 + 5, 0x33);
	pci_write_config8(dev, Q35_PAM0 + 6, 0x33);

	/* This sneaked in here, because Qemu does not
	 * emulate a SuperIO chip
	 */
	pc_keyboard_init(NO_AUX_DEVICE);

	/* setup IRQ routing for pci slots */
	for (i = 0; i < 25; i++)
		pci_assign_irqs(0, i, qemu_q35_irqs + (i % 4));
	/* setup IRQ routing southbridge devices */
	for (i = 25; i < 32; i++)
		pci_assign_irqs(0, i, qemu_q35_irqs);
}

static void qemu_nb_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	/* reserve mmconfig */
	fixed_mem_resource(dev, 2, CONFIG_MMCONF_BASE_ADDRESS >> 10, 0x10000000 >> 10,
			   IORESOURCE_RESERVE);
}


static struct device_operations nb_operations = {
	.read_resources   = qemu_nb_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = qemu_nb_init,
	.ops_pci          = 0,
};

static const struct pci_driver nb_driver __pci_driver = {
	.ops = &nb_operations,
	.vendor = 0x8086,
	.device = 0x29c0,
};
