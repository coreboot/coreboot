#ifndef PCI_OPS_H
#define PCI_OPS_H

#include <stdint.h>
#include <device/device.h>
#include <arch/pci_ops.h>

uint8_t  pci_read_config8(device_t dev, unsigned where);
uint16_t pci_read_config16(device_t dev, unsigned where);
uint32_t pci_read_config32(device_t dev, unsigned where);
void pci_write_config8(device_t dev, unsigned where, uint8_t val);
void pci_write_config16(device_t dev, unsigned where, uint16_t val);
void pci_write_config32(device_t dev, unsigned where, uint32_t val);

#if CONFIG_MMCONF_SUPPORT
uint8_t  pci_mmio_read_config8(device_t dev, unsigned where);
uint16_t pci_mmio_read_config16(device_t dev, unsigned where);
uint32_t pci_mmio_read_config32(device_t dev, unsigned where);
void pci_mmio_write_config8(device_t dev, unsigned where, uint8_t val);
void pci_mmio_write_config16(device_t dev, unsigned where, uint16_t val);
void pci_mmio_write_config32(device_t dev, unsigned where, uint32_t val);
#endif

#endif /* PCI_OPS_H */
