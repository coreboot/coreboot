#ifndef _PCICONF_H
#define _PCICONF_H

/*      
 * Direct access to PCI hardware...
 */     
extern uint8_t pci_ppc_read_config8(unsigned char, int, int);
extern uint16_t pci_ppc_read_config16(unsigned char, int, int);
extern uint32_t pci_ppc_read_config32(unsigned char, int, int);
extern int pci_ppc_write_config8(unsigned char, int, int, uint8_t);
extern int pci_ppc_write_config16(unsigned char, int, int, uint16_t);
extern int pci_ppc_write_config32(unsigned char, int, int, uint32_t);

#define CONFIG_CMD(bus,devfn,where) \
                ((bus << 16) | (devfn << 8) | (where & ~3) | 0x80000000)

#endif /* _PCICONF_H */
