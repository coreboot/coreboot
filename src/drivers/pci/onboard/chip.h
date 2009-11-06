#ifndef PCI_ONBOARD_H
#define PCI_ONBOARD_H

struct drivers_pci_onboard_config 
{
       unsigned long rom_address;
};
struct chip_operations;
extern struct chip_operations drivers_pci_onboard_ops;

#endif 
