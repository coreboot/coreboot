/*
 * viatool - dump all registers on a VIA CPU + chipset based system.
 *
 * Copyright (C) 2013 Alexandru Gagniuc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * a long with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <viatool.h>

struct quirk {
	int pci_domain;
	int pci_bus;
	int pci_dev;
	int pci_func;
	int pci_vendor_id;
	int pci_device_id;
	int (*quirk_func)(struct pci_dev *dev);
};

struct quirk_list {
	int pci_vendor_id;
	int pci_device_id;
	/* NULL-terminated list of quirks */
	struct quirk *dev_quirks;
};

