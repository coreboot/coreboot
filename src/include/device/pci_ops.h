#ifndef PCI_OPS_H
#define PCI_OPS_H

#include <stdint.h>
struct device;

int pci_read_config_byte(struct device *dev, uint8_t where, uint8_t *val);
int pci_read_config_word(struct device *dev, uint8_t where, uint16_t *val);
int pci_read_config_dword(struct device *dev, uint8_t where, uint32_t *val);
int pci_write_config_byte(struct device *dev, uint8_t where, uint8_t val);
int pci_write_config_word(struct device *dev, uint8_t where, uint16_t val);
int pci_write_config_dword(struct device *dev, uint8_t where, uint32_t val);
void pci_set_method(void);

#endif /* PCI_OPS_H */
