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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/keyboard.h>
#include <arch/io.h>

/* not sure how these are routed in qemu */
static const unsigned char enetIrqs[4] = { 11, 0, 0, 0 };

static void qemu_nb_init(device_t dev)
{
	/* Map memory at 0xc0000 - 0xfffff */
	int i;
	uint8_t v = pci_read_config8(dev, 0x59);
	v |= 0x30;
	pci_write_config8(dev, 0x59, v);
	for (i=0; i<6; i++)
	pci_write_config8(dev, 0x5a + i, 0x33);

	/* This sneaked in here, because Qemu does not
	 * emulate a SuperIO chip
	 */
	pc_keyboard_init(0);

	/* The PIRQ table is not working well for interrupt routing purposes.
	 * so we'll just set the IRQ directly.
	 */
	printk(BIOS_INFO, "Setting up ethernet...\n");
	pci_assign_irqs(0, 3, enetIrqs);
}

static struct device_operations nb_operations = {
 	.read_resources   = pci_dev_read_resources,
 	.set_resources    = pci_dev_set_resources,
 	.enable_resources = pci_dev_enable_resources,
 	.init             = qemu_nb_init,
	.ops_pci          = 0,
};

static const struct pci_driver nb_driver __pci_driver = {
	.ops = &nb_operations,
	.vendor = 0x8086,
	.device = 0x1237,
};
