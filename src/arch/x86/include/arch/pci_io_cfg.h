/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _PCI_IO_CFG_H
#define _PCI_IO_CFG_H

#include <stdint.h>
#include <arch/io.h>
#include <device/pci_type.h>

#define PCI_IO_CONFIG_INDEX		0xcf8
#define PCI_IO_CONFIG_DATA		0xcfc
#define PCI_IO_CONFIG_PORT_COUNT	8
#define PCI_IO_CONFIG_LAST_PORT		(PCI_IO_CONFIG_INDEX + PCI_IO_CONFIG_PORT_COUNT - 1)

static __always_inline
uint32_t pci_io_encode_addr(pci_devfn_t dev, uint16_t reg)
{
	uint32_t addr = 1U << 31;

	addr |= dev >> 4;
	addr |= reg & 0xfc;

	if (CONFIG(PCI_IO_CFG_EXT))
		addr |= (reg & 0xf00) << 16;

	return addr;
}

static __always_inline
uint8_t pci_io_read_config8(pci_devfn_t dev, uint16_t reg)
{
	uint32_t addr = pci_io_encode_addr(dev, reg);
	outl(addr, PCI_IO_CONFIG_INDEX);
	return inb(PCI_IO_CONFIG_DATA + (reg & 3));
}

static __always_inline
uint16_t pci_io_read_config16(pci_devfn_t dev, uint16_t reg)
{
	uint32_t addr = pci_io_encode_addr(dev, reg);
	outl(addr, PCI_IO_CONFIG_INDEX);
	return inw(PCI_IO_CONFIG_DATA + (reg & 2));
}

static __always_inline
uint32_t pci_io_read_config32(pci_devfn_t dev, uint16_t reg)
{
	uint32_t addr = pci_io_encode_addr(dev, reg);
	outl(addr, PCI_IO_CONFIG_INDEX);
	return inl(PCI_IO_CONFIG_DATA);
}

static __always_inline
void pci_io_write_config8(pci_devfn_t dev, uint16_t reg, uint8_t value)
{
	uint32_t addr = pci_io_encode_addr(dev, reg);
	outl(addr, PCI_IO_CONFIG_INDEX);
	outb(value, PCI_IO_CONFIG_DATA + (reg & 3));
}

static __always_inline
void pci_io_write_config16(pci_devfn_t dev, uint16_t reg, uint16_t value)
{
	uint32_t addr = pci_io_encode_addr(dev, reg);
	outl(addr, PCI_IO_CONFIG_INDEX);
	outw(value, PCI_IO_CONFIG_DATA + (reg & 2));
}

static __always_inline
void pci_io_write_config32(pci_devfn_t dev, uint16_t reg, uint32_t value)
{
	uint32_t addr = pci_io_encode_addr(dev, reg);
	outl(addr, PCI_IO_CONFIG_INDEX);
	outl(value, PCI_IO_CONFIG_DATA);
}

#if !CONFIG(ECAM_MMCONF_SUPPORT)

/* Avoid name collisions as different stages have different signature
 * for these functions. The _s_ stands for simple, fundamental IO or
 * MMIO variant.
 */

static __always_inline
uint8_t pci_s_read_config8(pci_devfn_t dev, uint16_t reg)
{
	return pci_io_read_config8(dev, reg);
}

static __always_inline
uint16_t pci_s_read_config16(pci_devfn_t dev, uint16_t reg)
{
	return pci_io_read_config16(dev, reg);
}

static __always_inline
uint32_t pci_s_read_config32(pci_devfn_t dev, uint16_t reg)
{
	return pci_io_read_config32(dev, reg);
}

static __always_inline
void pci_s_write_config8(pci_devfn_t dev, uint16_t reg, uint8_t value)
{
	pci_io_write_config8(dev, reg, value);
}

static __always_inline
void pci_s_write_config16(pci_devfn_t dev, uint16_t reg, uint16_t value)
{
	pci_io_write_config16(dev, reg, value);
}

static __always_inline
void pci_s_write_config32(pci_devfn_t dev, uint16_t reg, uint32_t value)
{
	pci_io_write_config32(dev, reg, value);
}

#endif

#endif /* _PCI_IO_CFG_H */
