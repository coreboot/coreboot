#ifndef ARCH_I386_PCI_OPS_H
#define ARCH_I386_PCI_OPS_H

const struct pci_bus_operations pci_cf8_conf1;
const struct pci_bus_operations pci_cf8_conf2;

void pci_set_method(device_t dev);

#endif /* ARCH_I386_PCI_OPS_H */
