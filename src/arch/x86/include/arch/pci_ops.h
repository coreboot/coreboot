#ifndef ARCH_I386_PCI_OPS_H
#define ARCH_I386_PCI_OPS_H

#ifndef __SIMPLE_DEVICE__

extern const struct pci_bus_operations pci_cf8_conf1;

#if CONFIG_MMCONF_SUPPORT
extern const struct pci_bus_operations pci_ops_mmconf;
#endif

const struct pci_bus_operations *pci_bus_default_ops(device_t dev);

#endif

#endif /* ARCH_I386_PCI_OPS_H */
