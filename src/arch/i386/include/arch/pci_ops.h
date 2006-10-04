#ifndef ARCH_I386_PCI_OPS_H
#define ARCH_I386_PCI_OPS_H

const struct pci_bus_operations pci_cf8_conf1;
const struct pci_bus_operations pci_cf8_conf2;

#if MMCONF_SUPPORT==1
const struct pci_bus_operations pci_ops_mmconf;
#endif

void pci_set_method(device_t dev);

#endif /* ARCH_I386_PCI_OPS_H */
