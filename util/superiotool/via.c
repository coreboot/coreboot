/*
 * This file is part of the superiotool project.
 *
 * Copyright (C) 2009 Carl-Daniel Hailfinger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include "superiotool.h"

#define DEVICE_ID_VT82C686_REG	0xe0
#define DEVICE_REV_VT82C686_REG	0xe1

static const struct superio_registers reg_table[] = {
	{0x3c, "VT82C686A/VT82C686B", {
		{EOT}}},
	{EOT}
};

static uint8_t vt82c686_conf = 0;

static int enter_conf_mode_via_vt82c686(void)
{
	struct pci_dev *dev;

	dev = pci_dev_find(0x1106, 0x0686);
	if (!dev) {
		if (verbose)
			printf("  PCI device 1106:0686 not found.\n");
		return 1;
	}

	vt82c686_conf = pci_read_byte(dev, 0x85);
	if (verbose)
		printf("  Super I/O %sabled, Super I/O configuration %sabled\n",
		       (vt82c686_conf & (1 << 0)) ? "en" : "dis",
		       (vt82c686_conf & (1 << 1)) ? "en" : "dis");

	/* If the Super I/O is not enabled, skip it. */
	if (!(vt82c686_conf & (1 << 0)))
		return 1;

	/* Enable Super I/O configuration mode. */
	pci_write_byte(dev, 0x85, vt82c686_conf | (1 << 1));

	return 0;
}

static void exit_conf_mode_via_vt82c686(void)
{
	struct pci_dev *dev;

	dev = pci_dev_find(0x1106, 0x0686);
	if (!dev) {
		printf("Bug: PCI device 1106:0686 not found during shutdown.\n"
		       "Please report to coreboot@coreboot.org.\n");
		return;
	}

	/* Restore (disable?) Super I/O configuration mode setting. */
	pci_write_byte(dev, 0x85, vt82c686_conf);
}

void probe_idregs_via(uint16_t port)
{
	uint16_t id;
	uint8_t rev;

	probing_for("VIA", "", port);

	if (enter_conf_mode_via_vt82c686())
		return;

	id = regval(port, DEVICE_ID_VT82C686_REG);
	rev = regval(port, DEVICE_REV_VT82C686_REG);

	if (superio_unknown(reg_table, id)) {
		if (verbose)
			printf(NOTFOUND "id=0x%04x, rev=0x%02x\n", id, rev);
		exit_conf_mode_via_vt82c686();
		return;
	}

	printf("Found VIA %s (id=0x%04x, rev=0x%02x) at 0x%x\n",
	       get_superio_name(reg_table, id), id, rev, port);
	chip_found = 1;

	exit_conf_mode_via_vt82c686();
}

void print_via_chips(void)
{
	print_vendor_chips("VIA", reg_table);
}
