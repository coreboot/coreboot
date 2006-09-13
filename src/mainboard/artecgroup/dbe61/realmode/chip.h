#ifndef PCI_REALMODE_H
#define PCI_REALMODE_H

struct drivers_pci_realmode_config 
{
       unsigned long rom_address;
};
//struct chip_operations;
extern struct chip_operations drivers_pci_realmode_ops;

#endif 
