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
/*
 * Functions for accessing PCI configuration space with type 1 accesses
 */

#if !IS_ENABLED(CONFIG_PCI_IO_CFG_EXT)
#define CONFIG_CMD(bus, devfn, where)	(0x80000000 | (bus << 16) | \
					(devfn << 8) | (where & ~3))
#else
#define CONFIG_CMD(bus, devfn, where)	(0x80000000 | (bus << 16) | \
					(devfn << 8) | ((where & 0xff) & ~3) |\
					((where & 0xf00)<<16))
#endif

static uint8_t pci_conf1_read_config8(struct bus *pbus, int bus, int devfn,
				      int where)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	return inb(0xCFC + (where & 3));
}

static uint16_t pci_conf1_read_config16(struct bus *pbus, int bus, int devfn,
					int where)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	return inw(0xCFC + (where & 2));
}

static uint32_t pci_conf1_read_config32(struct bus *pbus, int bus, int devfn,
					int where)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	return inl(0xCFC);
}

static void pci_conf1_write_config8(struct bus *pbus, int bus, int devfn,
				    int where, uint8_t value)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	outb(value, 0xCFC + (where & 3));
}

static void pci_conf1_write_config16(struct bus *pbus, int bus, int devfn,
				     int where, uint16_t value)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	outw(value, 0xCFC + (where & 2));
}

static void pci_conf1_write_config32(struct bus *pbus, int bus, int devfn,
				     int where, uint32_t value)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	outl(value, 0xCFC);
}

#undef CONFIG_CMD

const struct pci_bus_operations pci_cf8_conf1 = {
	.read8 = pci_conf1_read_config8,
	.read16 = pci_conf1_read_config16,
	.read32 = pci_conf1_read_config32,
	.write8 = pci_conf1_write_config8,
	.write16 = pci_conf1_write_config16,
	.write32 = pci_conf1_write_config32,
};
