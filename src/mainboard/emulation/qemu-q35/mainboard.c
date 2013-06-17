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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/keyboard.h>
#include <arch/io.h>
#include <console/console.h>

#define Q35_PAM0            0x90
#define Q35_PCIEXBAR_ADDR   0xb0000000

static const unsigned char qemu_q35_irqs[] = {
	10, 10, 11, 11,
	10, 10, 11, 11,
};

static void qemu_nb_init(device_t dev)
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
	pc_keyboard_init(0);

	/* setup IRQ routing for pci slots */
	for (i = 0; i < 25; i++)
		pci_assign_irqs(0, i, qemu_q35_irqs + (i % 4));
	/* setup IRQ routing southbridge devices */
	for (i = 25; i < 32; i++)
		pci_assign_irqs(0, i, qemu_q35_irqs);

	/* setup mmconfig */
	pci_write_config32(dev, 0x60, Q35_PCIEXBAR_ADDR | 1);
}

static void qemu_nb_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	/* reserve mmconfig */
	fixed_mem_resource(dev, 2, Q35_PCIEXBAR_ADDR >> 10, 0x10000000 >> 10,
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
