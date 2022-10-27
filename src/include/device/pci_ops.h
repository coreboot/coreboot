/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef PCI_OPS_H
#define PCI_OPS_H

#include <stdint.h>
#include <device/device.h>
#include <device/pci_type.h>
#include <arch/pci_ops.h> /* IWYU pragma: export */

void __noreturn pcidev_die(void);

static __always_inline pci_devfn_t pcidev_bdf(const struct device *dev)
{
	return (dev->path.pci.devfn << 12) | (dev->bus->secondary << 20);
}

static __always_inline pci_devfn_t pcidev_assert(const struct device *dev)
{
	if (!dev)
		pcidev_die();
	return pcidev_bdf(dev);
}

#if defined(__SIMPLE_DEVICE__)
#define ENV_PCI_SIMPLE_DEVICE 1
#else
#define ENV_PCI_SIMPLE_DEVICE 0
#endif

#if ENV_PCI_SIMPLE_DEVICE

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

static __always_inline
u8 pci_read_config8(const struct device *dev, u16 reg)
{
	return pci_s_read_config8(PCI_BDF(dev), reg);
}

static __always_inline
u16 pci_read_config16(const struct device *dev, u16 reg)
{
	return pci_s_read_config16(PCI_BDF(dev), reg);
}

static __always_inline
u32 pci_read_config32(const struct device *dev, u16 reg)
{
	return pci_s_read_config32(PCI_BDF(dev), reg);
}

static __always_inline
void pci_write_config8(const struct device *dev, u16 reg, u8 val)
{
	pci_s_write_config8(PCI_BDF(dev), reg, val);
}

static __always_inline
void pci_write_config16(const struct device *dev, u16 reg, u16 val)
{
	pci_s_write_config16(PCI_BDF(dev), reg, val);
}

static __always_inline
void pci_write_config32(const struct device *dev, u16 reg, u32 val)
{
	pci_s_write_config32(PCI_BDF(dev), reg, val);
}

#endif

#if ENV_PCI_SIMPLE_DEVICE
static __always_inline
void pci_update_config8(pci_devfn_t dev, u16 reg, u8 mask, u8 or)
#else
static __always_inline
void pci_update_config8(const struct device *dev, u16 reg, u8 mask, u8 or)
#endif
{
	u8 reg8;

	reg8 = pci_read_config8(dev, reg);
	reg8 &= mask;
	reg8 |= or;
	pci_write_config8(dev, reg, reg8);
}

#if ENV_PCI_SIMPLE_DEVICE
static __always_inline
void pci_update_config16(pci_devfn_t dev, u16 reg, u16 mask, u16 or)
#else
static __always_inline
void pci_update_config16(const struct device *dev, u16 reg, u16 mask, u16 or)
#endif
{
	u16 reg16;

	reg16 = pci_read_config16(dev, reg);
	reg16 &= mask;
	reg16 |= or;
	pci_write_config16(dev, reg, reg16);
}

#if ENV_PCI_SIMPLE_DEVICE
static __always_inline
void pci_update_config32(pci_devfn_t dev, u16 reg, u32 mask, u32 or)
#else
static __always_inline
void pci_update_config32(const struct device *dev, u16 reg, u32 mask, u32 or)
#endif
{
	u32 reg32;

	reg32 = pci_read_config32(dev, reg);
	reg32 &= mask;
	reg32 |= or;
	pci_write_config32(dev, reg, reg32);
}

#if ENV_PCI_SIMPLE_DEVICE
static __always_inline
void pci_and_config8(pci_devfn_t dev, u16 reg, u8 andmask)
#else
static __always_inline
void pci_and_config8(const struct device *dev, u16 reg, u8 andmask)
#endif
{
	pci_update_config8(dev, reg, andmask, 0);
}

#if ENV_PCI_SIMPLE_DEVICE
static __always_inline
void pci_and_config16(pci_devfn_t dev, u16 reg, u16 andmask)
#else
static __always_inline
void pci_and_config16(const struct device *dev, u16 reg, u16 andmask)
#endif
{
	pci_update_config16(dev, reg, andmask, 0);
}

#if ENV_PCI_SIMPLE_DEVICE
static __always_inline
void pci_and_config32(pci_devfn_t dev, u16 reg, u32 andmask)
#else
static __always_inline
void pci_and_config32(const struct device *dev, u16 reg, u32 andmask)
#endif
{
	pci_update_config32(dev, reg, andmask, 0);
}

#if ENV_PCI_SIMPLE_DEVICE
static __always_inline
void pci_or_config8(pci_devfn_t dev, u16 reg, u8 ormask)
#else
static __always_inline
void pci_or_config8(const struct device *dev, u16 reg, u8 ormask)
#endif
{
	pci_update_config8(dev, reg, 0xff, ormask);
}

#if ENV_PCI_SIMPLE_DEVICE
static __always_inline
void pci_or_config16(pci_devfn_t dev, u16 reg, u16 ormask)
#else
static __always_inline
void pci_or_config16(const struct device *dev, u16 reg, u16 ormask)
#endif
{
	pci_update_config16(dev, reg, 0xffff, ormask);
}

#if ENV_PCI_SIMPLE_DEVICE
static __always_inline
void pci_or_config32(pci_devfn_t dev, u16 reg, u32 ormask)
#else
static __always_inline
void pci_or_config32(const struct device *dev, u16 reg, u32 ormask)
#endif
{
	pci_update_config32(dev, reg, 0xffffffff, ormask);
}

u16 pci_s_find_next_capability(pci_devfn_t dev, u16 cap, u16 last);
u16 pci_s_find_capability(pci_devfn_t dev, u16 cap);

static __always_inline
u16 pci_find_next_capability(const struct device *dev, u16 cap, u16 last)
{
	return pci_s_find_next_capability(PCI_BDF(dev), cap, last);
}

static __always_inline
u16 pci_find_capability(const struct device *dev, u16 cap)
{
	return pci_s_find_capability(PCI_BDF(dev), cap);
}

/*
 * Determine if the given PCI device is the source of wake from sleep by checking PME_STATUS and
 * PME_ENABLE bits in PM control and status register.
 *
 * Returns true if PCI device is wake source, false otherwise.
 */
bool pci_dev_is_wake_source(pci_devfn_t dev);

#endif /* PCI_OPS_H */
