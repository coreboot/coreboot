#ifndef PCI_RTL8139_H
#define PCI_RTL8139_H

struct drivers_pci_rtl8139_config 
{
       uint8_t nic_irq;	// RTL8139 NIC
};
//struct chip_operations;
extern struct chip_operations drivers_pci_rtl8139_ops;

#endif 
