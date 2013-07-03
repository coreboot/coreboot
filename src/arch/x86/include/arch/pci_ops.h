#ifndef ARCH_I386_PCI_OPS_H
#define ARCH_I386_PCI_OPS_H

extern const struct pci_bus_operations pci_cf8_conf1;

#if CONFIG_MMCONF_SUPPORT
extern const struct pci_bus_operations pci_ops_mmconf;
#endif

#if CONFIG_MMCONF_SUPPORT_DEFAULT
#define pci_bus_default_ops &pci_ops_mmconf
#else
#define pci_bus_default_ops &pci_cf8_conf1
#endif

static inline const struct pci_bus_operations *pci_config_default(void)
{
#if CONFIG_MMCONF_SUPPORT_DEFAULT
	return &pci_ops_mmconf;
#else
	return &pci_cf8_conf1;
#endif
}

static inline void pci_set_method(device_t dev)
{
	dev->ops->ops_pci_bus = pci_config_default();
}
#endif /* ARCH_I386_PCI_OPS_H */
