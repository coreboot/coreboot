#ifndef ARCH_I386_PCI_OPS_H
#define ARCH_I386_PCI_OPS_H

struct pci_ops {
	uint8_t (*read8)   (uint8_t bus, int devfn, int where);
	uint16_t (*read16) (uint8_t bus, int devfn, int where);
	uint32_t (*read32) (uint8_t bus, int devfn, int where);
	void (*write8)  (uint8_t bus, int devfn, int where, uint8_t val);
	void (*write16) (uint8_t bus, int devfn, int where, uint16_t val);
	void (*write32) (uint8_t bus, int devfn, int where, uint32_t val);
};
extern const struct pci_ops *conf;

void pci_set_method_conf1(void);
void pci_set_method_conf2(void);
void pci_set_method(void);

#endif /* ARCH_I386_PCI_OPS_H */
