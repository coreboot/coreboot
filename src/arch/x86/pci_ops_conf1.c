/*
 * This file is part of the coreboot project.
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

#include <arch/io.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <arch/pci_io_cfg.h>

/*
 * Functions for accessing PCI configuration space with type 1 accesses
 */

const struct pci_bus_operations pci_cf8_conf1 = {
	.read8 = pci_io_read_config8,
	.read16 = pci_io_read_config16,
	.read32 = pci_io_read_config32,
	.write8 = pci_io_write_config8,
	.write16 = pci_io_write_config16,
	.write32 = pci_io_write_config32,
};
