#ifndef PCI_CONF_REG_INDEX
// These are defined in the PCI spec, and hence are theoretically
// inclusive of ANYTHING that uses a PCI bus. 
#define	PCI_CONF_REG_INDEX	0xcf8
#define	PCI_CONF_REG_DATA	0xcfc
#define CONFIG_ADDR(bus,devfn,where) (((bus) << 16) | ((devfn) << 8) | (where))
#endif
