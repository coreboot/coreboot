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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _PCI_IO_CFG_H
#define _PCI_IO_CFG_H

#include <arch/io.h>

static inline __attribute__((always_inline))
uint8_t pci_io_read_config8(pci_devfn_t dev, unsigned where)
{
	unsigned addr;
#if !CONFIG_PCI_IO_CFG_EXT
	addr = (dev>>4) | where;
#else
	addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16); //seg == 0
#endif
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inb(0xCFC + (addr & 3));
}

static inline __attribute__((always_inline))
uint16_t pci_io_read_config16(pci_devfn_t dev, unsigned where)
{
	unsigned addr;
#if !CONFIG_PCI_IO_CFG_EXT
        addr = (dev>>4) | where;
#else
        addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16);
#endif
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inw(0xCFC + (addr & 2));
}

static inline __attribute__((always_inline))
uint32_t pci_io_read_config32(pci_devfn_t dev, unsigned where)
{
	unsigned addr;
#if !CONFIG_PCI_IO_CFG_EXT
        addr = (dev>>4) | where;
#else
        addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16);
#endif
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inl(0xCFC);
}

static inline __attribute__((always_inline))
void pci_io_write_config8(pci_devfn_t dev, unsigned where, uint8_t value)
{
	unsigned addr;
#if !CONFIG_PCI_IO_CFG_EXT
        addr = (dev>>4) | where;
#else
        addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16);
#endif
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outb(value, 0xCFC + (addr & 3));
}

static inline __attribute__((always_inline))
void pci_io_write_config16(pci_devfn_t dev, unsigned where, uint16_t value)
{
        unsigned addr;
#if !CONFIG_PCI_IO_CFG_EXT
        addr = (dev>>4) | where;
#else
        addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16);
#endif
        outl(0x80000000 | (addr & ~3), 0xCF8);
        outw(value, 0xCFC + (addr & 2));
}

static inline __attribute__((always_inline))
void pci_io_write_config32(pci_devfn_t dev, unsigned where, uint32_t value)
{
	unsigned addr;
#if !CONFIG_PCI_IO_CFG_EXT
        addr = (dev>>4) | where;
#else
        addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16);
#endif
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outl(value, 0xCFC);
}

#if !CONFIG_MMCONF_SUPPORT_DEFAULT
#define pci_read_config8 	pci_io_read_config8
#define pci_read_config16	pci_io_read_config16
#define pci_read_config32	pci_io_read_config32

#define pci_write_config8 	pci_io_write_config8
#define pci_write_config16	pci_io_write_config16
#define pci_write_config32	pci_io_write_config32
#endif

#endif /* _PCI_IO_CFG_H */
