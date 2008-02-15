#ifndef ARCH_X86_PCICONF_H
#define ARCH_X86_PCICONF_H

#ifndef PCI_CONF_REG_INDEX

// These are defined in the PCI spec, and hence are theoretically
// inclusive of ANYTHING that uses a PCI bus. 
#define	PCI_CONF_REG_INDEX	0xcf8
#define	PCI_CONF_REG_DATA	0xcfc

#endif

#endif /* ARCH_X86_PCICONF_H */
