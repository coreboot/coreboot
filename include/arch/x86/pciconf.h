#ifndef ARCH_X86_PCICONF_H
#define ARCH_X86_PCICONF_H

#ifndef PCI_CONF_REG_INDEX

// These are defined in the PCI spec, and hence are theoretically
// inclusive of ANYTHING that uses a PCI bus. 
#define	PCI_CONF_REG_INDEX	0xcf8
#define	PCI_CONF_REG_DATA	0xcfc

/* WTF for now */
#if 0
#if PCI_IO_CFG_EXT == 0
#define CONFIG_ADDR(bus,devfn,where) (((bus) << 16) | ((devfn) << 8) | (where))
#else
#define CONFIG_ADDR(bus,devfn,where) (((bus) << 16) | ((devfn) << 8) | (where & 0xff) | ((where & 0xf00)<<16) )
#endif
#endif
#define CONFIG_ADDR(bus,devfn,where) (((bus) << 16) | ((devfn) << 8) | (where))
#endif

#endif /* ARCH_X86_PCICONF_H */
