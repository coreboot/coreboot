#ifndef PCI_OPS_H
#define PCI_OPS_H

#include <stdint.h>
#include <device/device.h>

uint8_t  pci_read_config8(device_t dev, unsigned where);
uint16_t pci_read_config16(device_t dev, unsigned where);
uint32_t pci_read_config32(device_t dev, unsigned where);
void pci_write_config8(device_t dev, unsigned where, uint8_t val);
void pci_write_config16(device_t dev, unsigned where, uint16_t val);
void pci_write_config32(device_t dev, unsigned where, uint32_t val);

void pci_set_method(void);

#endif /* PCI_OPS_H */
