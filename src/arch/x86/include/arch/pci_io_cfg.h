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

#ifndef _PCI_IO_CFG_H
#define _PCI_IO_CFG_H

#include <compiler.h>
#include <arch/io.h>

static __always_inline
unsigned int pci_io_encode_addr(pci_devfn_t dev, unsigned int where)
{
	if (IS_ENABLED(CONFIG_PCI_IO_CFG_EXT)) {
		// seg == 0
		return dev >> 4 | (where & 0xff) | ((where & 0xf00) << 16);
	} else {
		return dev >> 4 | where;
	}
}

static __always_inline
uint8_t pci_io_read_config8(pci_devfn_t dev, unsigned int where)
{
	unsigned int addr = pci_io_encode_addr(dev, where);
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inb(0xCFC + (addr & 3));
}

static __always_inline
uint16_t pci_io_read_config16(pci_devfn_t dev, unsigned int where)
{
	unsigned int addr = pci_io_encode_addr(dev, where);
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inw(0xCFC + (addr & 2));
}

static __always_inline
uint32_t pci_io_read_config32(pci_devfn_t dev, unsigned int where)
{
	unsigned int addr = pci_io_encode_addr(dev, where);
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inl(0xCFC);
}

static __always_inline
void pci_io_write_config8(pci_devfn_t dev, unsigned int where, uint8_t value)
{
	unsigned int addr = pci_io_encode_addr(dev, where);
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outb(value, 0xCFC + (addr & 3));
}

static __always_inline
void pci_io_write_config16(pci_devfn_t dev, unsigned int where, uint16_t value)
{
	unsigned int addr = pci_io_encode_addr(dev, where);
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outw(value, 0xCFC + (addr & 2));
}

static __always_inline
void pci_io_write_config32(pci_devfn_t dev, unsigned int where, uint32_t value)
{
	unsigned int addr = pci_io_encode_addr(dev, where);
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outl(value, 0xCFC);
}

#endif /* _PCI_IO_CFG_H */
