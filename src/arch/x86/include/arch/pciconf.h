#ifndef PCI_CONF_REG_INDEX

// These are defined in the PCI spec, and hence are theoretically
// inclusive of ANYTHING that uses a PCI bus.
#define	PCI_CONF_REG_INDEX	0xcf8
#define	PCI_CONF_REG_DATA	0xcfc

#if !CONFIG_PCI_IO_CFG_EXT
#define CONFIG_ADDR(bus,devfn,where) (((bus) << 16) | ((devfn) << 8) | (where))
#else
#define CONFIG_ADDR(bus,devfn,where) (((bus) << 16) | ((devfn) << 8) | (where & 0xff) | ((where & 0xf00)<<16) )
#endif

#endif
