#ifndef E7505_DEBUG_H
#define E7505_DEBUG_H

void print_debug_pci_dev(unsigned dev);
void print_pci_devices(void);
void dump_pci_device(unsigned dev);
void dump_pci_devices(void);
void dump_pci_devices_on_bus(unsigned busn);
void dump_spd_registers(const struct mem_controller *ctrl);
void dump_smbus_registers(void);
void dump_io_resources(unsigned port);
void dump_mem(unsigned start, unsigned end);

#endif

