#ifndef ARCH_I386_PCI_OPS_H
#define ARCH_I386_PCI_OPS_H

extern const struct pci_bus_operations pci_cf8_conf1;

#if CONFIG_MMCONF_SUPPORT
extern const struct pci_bus_operations pci_ops_mmconf;
#endif

static inline const struct pci_bus_operations *pci_config_default(void)
{
	return &pci_cf8_conf1;
}
#endif /* ARCH_I386_PCI_OPS_H */
