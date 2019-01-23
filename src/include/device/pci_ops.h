/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2009 coresystems GmbH
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

#ifndef PCI_OPS_H
#define PCI_OPS_H

#include <stdint.h>
#include <device/device.h>
#include <device/pci_type.h>
#include <arch/pci_ops.h>

#ifdef __SIMPLE_DEVICE__

/* Avoid name collisions as different stages have different signature
 * for these functions. The _s_ stands for simple, fundamental IO or
 * MMIO variant.
 */
#define pci_read_config8 pci_s_read_config8
#define pci_read_config16 pci_s_read_config16
#define pci_read_config32 pci_s_read_config32
#define pci_write_config8 pci_s_write_config8
#define pci_write_config16 pci_s_write_config16
#define pci_write_config32 pci_s_write_config32
#else

#include <device/pci.h>

const struct pci_bus_operations *pci_bus_default_ops(void);

static __always_inline const struct pci_bus_operations *pci_bus_ops(void)
{
	return pci_bus_default_ops();
}

void __noreturn pcidev_die(void);

static __always_inline void pcidev_assert(const struct device *dev)
{
	if (!dev)
		pcidev_die();
}

static __always_inline
u8 pci_read_config8(struct device *dev, unsigned int where)
{
	pcidev_assert(dev);
	return pci_bus_ops()->read8(dev, where);
}

static __always_inline
u16 pci_read_config16(struct device *dev, unsigned int where)
{
	pcidev_assert(dev);
	return pci_bus_ops()->read16(dev, where);
}

static __always_inline
u32 pci_read_config32(struct device *dev, unsigned int where)
{
	pcidev_assert(dev);
	return pci_bus_ops()->read32(dev, where);
}

static __always_inline
void pci_write_config8(struct device *dev, unsigned int where, u8 val)
{
	pcidev_assert(dev);
	pci_bus_ops()->write8(dev, where, val);
}

static __always_inline
void pci_write_config16(struct device *dev, unsigned int where, u16 val)
{
	pcidev_assert(dev);
	pci_bus_ops()->write16(dev, where, val);
}

static __always_inline
void pci_write_config32(struct device *dev, unsigned int where, u32 val)
{
	pcidev_assert(dev);
	pci_bus_ops()->write32(dev, where, val);
}

#endif

#ifdef __SIMPLE_DEVICE__
static __always_inline
void pci_or_config8(pci_devfn_t dev, unsigned int where, u8 ormask)
#else
static __always_inline
void pci_or_config8(struct device *dev, unsigned int where, u8 ormask)
#endif
{
	u8 value = pci_read_config8(dev, where);
	pci_write_config8(dev, where, value | ormask);
}

#ifdef __SIMPLE_DEVICE__
static __always_inline
void pci_or_config16(pci_devfn_t dev, unsigned int where, u16 ormask)
#else
static __always_inline
void pci_or_config16(struct device *dev, unsigned int where, u16 ormask)
#endif
{
	u16 value = pci_read_config16(dev, where);
	pci_write_config16(dev, where, value | ormask);
}

#ifdef __SIMPLE_DEVICE__
static __always_inline
void pci_or_config32(pci_devfn_t dev, unsigned int where, u32 ormask)
#else
static __always_inline
void pci_or_config32(struct device *dev, unsigned int where, u32 ormask)
#endif
{
	u32 value = pci_read_config32(dev, where);
	pci_write_config32(dev, where, value | ormask);
}

#ifdef __SIMPLE_DEVICE__
static __always_inline
void pci_update_config8(pci_devfn_t dev, int reg, u8 mask, u8 or)
#else
static __always_inline
void pci_update_config8(struct device *dev, int reg, u8 mask, u8 or)
#endif
{
	u8 reg8;

	reg8 = pci_read_config8(dev, reg);
	reg8 &= mask;
	reg8 |= or;
	pci_write_config8(dev, reg, reg8);
}

#ifdef __SIMPLE_DEVICE__
static __always_inline
void pci_update_config16(pci_devfn_t dev, int reg, u16 mask, u16 or)
#else
static __always_inline
void pci_update_config16(struct device *dev, int reg, u16 mask, u16 or)
#endif
{
	u16 reg16;

	reg16 = pci_read_config16(dev, reg);
	reg16 &= mask;
	reg16 |= or;
	pci_write_config16(dev, reg, reg16);
}

#ifdef __SIMPLE_DEVICE__
static __always_inline
void pci_update_config32(pci_devfn_t dev, int reg, u32 mask, u32 or)
#else
static __always_inline
void pci_update_config32(struct device *dev, int reg, u32 mask, u32 or)
#endif
{
	u32 reg32;

	reg32 = pci_read_config32(dev, reg);
	reg32 &= mask;
	reg32 |= or;
	pci_write_config32(dev, reg, reg32);
}

#endif /* PCI_OPS_H */
