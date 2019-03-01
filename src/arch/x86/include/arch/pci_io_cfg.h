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

#include <stdint.h>
#include <arch/io.h>
#include <device/pci_type.h>

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

#if !IS_ENABLED(CONFIG_MMCONF_SUPPORT)

/* Avoid name collisions as different stages have different signature
 * for these functions. The _s_ stands for simple, fundamental IO or
 * MMIO variant.
 */

static __always_inline
uint8_t pci_s_read_config8(pci_devfn_t dev, unsigned int where)
{
	return pci_io_read_config8(dev, where);
}

static __always_inline
uint16_t pci_s_read_config16(pci_devfn_t dev, unsigned int where)
{
	return pci_io_read_config16(dev, where);
}

static __always_inline
uint32_t pci_s_read_config32(pci_devfn_t dev, unsigned int where)
{
	return pci_io_read_config32(dev, where);
}

static __always_inline
void pci_s_write_config8(pci_devfn_t dev, unsigned int where, uint8_t value)
{
	pci_io_write_config8(dev, where, value);
}

static __always_inline
void pci_s_write_config16(pci_devfn_t dev, unsigned int where, uint16_t value)
{
	pci_io_write_config16(dev, where, value);
}

static __always_inline
void pci_s_write_config32(pci_devfn_t dev, unsigned int where, uint32_t value)
{
	pci_io_write_config32(dev, where, value);
}

#endif

#endif /* _PCI_IO_CFG_H */
