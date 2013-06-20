/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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

#ifndef _PCI_MMIO_CFG_H
#define _PCI_MMIO_CFG_H

#include <arch/io.h>

#if CONFIG_MMCONF_SUPPORT
#define DEFAULT_PCIEXBAR	CONFIG_MMCONF_BASE_ADDRESS

static inline __attribute__ ((always_inline))
u8 pcie_read_config8(pci_devfn_t dev, unsigned int where)
{
	unsigned long addr;
	addr = DEFAULT_PCIEXBAR | dev | where;
	return read8(addr);
}

static inline __attribute__ ((always_inline))
u16 pcie_read_config16(pci_devfn_t dev, unsigned int where)
{
	unsigned long addr;
	addr = DEFAULT_PCIEXBAR | dev | (where & ~1);
	return read16(addr);
}

static inline __attribute__ ((always_inline))
u32 pcie_read_config32(pci_devfn_t dev, unsigned int where)
{
	unsigned long addr;
	addr = DEFAULT_PCIEXBAR | dev | (where & ~3);
	return read32(addr);
}

static inline __attribute__ ((always_inline))
void pcie_write_config8(pci_devfn_t dev, unsigned int where, u8 value)
{
	unsigned long addr;
	addr = DEFAULT_PCIEXBAR | dev | where;
	write8(addr, value);
}

static inline __attribute__ ((always_inline))
void pcie_write_config16(pci_devfn_t dev, unsigned int where, u16 value)
{
	unsigned long addr;
	addr = DEFAULT_PCIEXBAR | dev | (where & ~1);
	write16(addr, value);
}

static inline __attribute__ ((always_inline))
void pcie_write_config32(pci_devfn_t dev, unsigned int where, u32 value)
{
	unsigned long addr;
	addr = DEFAULT_PCIEXBAR | dev | (where & ~3);
	write32(addr, value);
}

static inline __attribute__ ((always_inline))
void pcie_or_config8(pci_devfn_t dev, unsigned int where, u8 ormask)
{
	u8 value = pcie_read_config8(dev, where);
	pcie_write_config8(dev, where, value | ormask);
}

static inline __attribute__ ((always_inline))
void pcie_or_config16(pci_devfn_t dev, unsigned int where, u16 ormask)
{
	u16 value = pcie_read_config16(dev, where);
	pcie_write_config16(dev, where, value | ormask);
}

static inline __attribute__ ((always_inline))
void pcie_or_config32(pci_devfn_t dev, unsigned int where, u32 ormask)
{
	u32 value = pcie_read_config32(dev, where);
	pcie_write_config32(dev, where, value | ormask);
}

#define pci_mmio_read_config8 	pcie_read_config8
#define pci_mmio_read_config16	pcie_read_config16
#define pci_mmio_read_config32	pcie_read_config32

#define pci_mmio_write_config8 	pcie_write_config8
#define pci_mmio_write_config16	pcie_write_config16
#define pci_mmio_write_config32	pcie_write_config32

#if CONFIG_MMCONF_SUPPORT_DEFAULT
#define pci_read_config8 	pcie_read_config8
#define pci_read_config16	pcie_read_config16
#define pci_read_config32	pcie_read_config32

#define pci_write_config8 	pcie_write_config8
#define pci_write_config16	pcie_write_config16
#define pci_write_config32	pcie_write_config32
#endif

#endif /* CONFIG_MMCONF_SUPPORT */
#endif /* _PCI_MMIO_CFG_H */
