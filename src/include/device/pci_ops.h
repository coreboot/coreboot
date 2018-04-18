#ifndef PCI_OPS_H
#define PCI_OPS_H

#include <stdint.h>
#include <device/device.h>
#include <arch/pci_ops.h>

#ifndef __SIMPLE_DEVICE__
u8 pci_read_config8(struct device *dev, unsigned int where);
u16 pci_read_config16(struct device *dev, unsigned int where);
u32 pci_read_config32(struct device *dev, unsigned int where);
void pci_write_config8(struct device *dev, unsigned int where, u8 val);
void pci_write_config16(struct device *dev, unsigned int where, u16 val);
void pci_write_config32(struct device *dev, unsigned int where, u32 val);

#endif

/*
 * Use device_t here as the functions are to be used with either
 * __SIMPLE_DEVICE__ defined or undefined.
 */
static inline __attribute__((always_inline))
void pci_or_config8(device_t dev, unsigned int where, u8 ormask)
{
	u8 value = pci_read_config8(dev, where);
	pci_write_config8(dev, where, value | ormask);
}

static inline __attribute__((always_inline))
void pci_or_config16(device_t dev, unsigned int where, u16 ormask)
{
	u16 value = pci_read_config16(dev, where);
	pci_write_config16(dev, where, value | ormask);
}

static inline __attribute__((always_inline))
void pci_or_config32(device_t dev, unsigned int where, u32 ormask)
{
	u32 value = pci_read_config32(dev, where);
	pci_write_config32(dev, where, value | ormask);
}

static inline __attribute__((always_inline))
void pci_update_config8(device_t dev, int reg, u8 mask, u8 or)
{
	u8 reg8;

	reg8 = pci_read_config8(dev, reg);
	reg8 &= mask;
	reg8 |= or;
	pci_write_config8(dev, reg, reg8);
}

static inline __attribute__((always_inline))
void pci_update_config16(device_t dev, int reg, u16 mask, u16 or)
{
	u16 reg16;

	reg16 = pci_read_config16(dev, reg);
	reg16 &= mask;
	reg16 |= or;
	pci_write_config16(dev, reg, reg16);
}

static inline __attribute__((always_inline))
void pci_update_config32(device_t dev, int reg, u32 mask, u32 or)
{
	u32 reg32;

	reg32 = pci_read_config32(dev, reg);
	reg32 &= mask;
	reg32 |= or;
	pci_write_config32(dev, reg, reg32);
}

const struct pci_bus_operations *pci_bus_default_ops(struct device *dev);

#endif /* PCI_OPS_H */
