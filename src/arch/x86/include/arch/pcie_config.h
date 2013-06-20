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

#define DEFAULT_PCIEXBAR	CONFIG_MMCONF_BASE_ADDRESS

static inline __attribute__ ((always_inline))
u8 pcie_read_config8(simple_device_t dev, unsigned int where)
{
	unsigned long addr;
	addr = DEFAULT_PCIEXBAR | dev | where;
	return read8(addr);
}

static inline __attribute__ ((always_inline))
u16 pcie_read_config16(simple_device_t dev, unsigned int where)
{
	unsigned long addr;
	addr = DEFAULT_PCIEXBAR | dev | where;
	return read16(addr);
}

static inline __attribute__ ((always_inline))
u32 pcie_read_config32(simple_device_t dev, unsigned int where)
{
	unsigned long addr;
	addr = DEFAULT_PCIEXBAR | dev | where;
	return read32(addr);
}

static inline __attribute__ ((always_inline))
void pcie_write_config8(simple_device_t dev, unsigned int where, u8 value)
{
	unsigned long addr;
	addr = DEFAULT_PCIEXBAR | dev | where;
	write8(addr, value);
}

static inline __attribute__ ((always_inline))
void pcie_write_config16(simple_device_t dev, unsigned int where, u16 value)
{
	unsigned long addr;
	addr = DEFAULT_PCIEXBAR | dev | where;
	write16(addr, value);
}

static inline __attribute__ ((always_inline))
void pcie_write_config32(simple_device_t dev, unsigned int where, u32 value)
{
	unsigned long addr;
	addr = DEFAULT_PCIEXBAR | dev | where;
	write32(addr, value);
}

static inline __attribute__ ((always_inline))
void pcie_or_config8(simple_device_t dev, unsigned int where, u8 ormask)
{
	u8 value = pcie_read_config8(dev, where);
	pcie_write_config8(dev, where, value | ormask);
}

static inline __attribute__ ((always_inline))
void pcie_or_config16(simple_device_t dev, unsigned int where, u16 ormask)
{
	u16 value = pcie_read_config16(dev, where);
	pcie_write_config16(dev, where, value | ormask);
}

static inline __attribute__ ((always_inline))
void pcie_or_config32(simple_device_t dev, unsigned int where, u32 ormask)
{
	u32 value = pcie_read_config32(dev, where);
	pcie_write_config32(dev, where, value | ormask);
}
