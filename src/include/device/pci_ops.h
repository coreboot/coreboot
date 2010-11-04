#ifndef PCI_OPS_H
#define PCI_OPS_H

#include <stdint.h>
#include <device/device.h>
#include <arch/pci_ops.h>

u8 pci_read_config8(device_t dev, unsigned int where);
u16 pci_read_config16(device_t dev, unsigned int where);
u32 pci_read_config32(device_t dev, unsigned int where);
void pci_write_config8(device_t dev, unsigned int where, u8 val);
void pci_write_config16(device_t dev, unsigned int where, u16 val);
void pci_write_config32(device_t dev, unsigned int where, u32 val);

#if CONFIG_MMCONF_SUPPORT
u8 pci_mmio_read_config8(device_t dev, unsigned int where);
u16 pci_mmio_read_config16(device_t dev, unsigned int where);
u32 pci_mmio_read_config32(device_t dev, unsigned int where);
void pci_mmio_write_config8(device_t dev, unsigned int where, u8 val);
void pci_mmio_write_config16(device_t dev, unsigned int where, u16 val);
void pci_mmio_write_config32(device_t dev, unsigned int where, u32 val);
#endif

/* This function lives in pci_ops_auto.c */
const struct pci_bus_operations *pci_remember_direct(void);

#endif /* PCI_OPS_H */
