#include <resource.h>
#include <pci.h>

struct pci_bus pci_root = {
	.children = &bus1;
};
struct pci_dev dev0_2_0 = { 
	.bus = &pci_root,
	.devfn = PCI_DEVFN(2,0),
};
struct pci_bus bus1 = {
	.parent = &pci_root,
	.self = &dev0_2_0,
};
struct pci_dev dev1_1c_0 = {
	.bus = &bus1,
	.devfn = PCI_DEVFN(0x1c, 0),
	/* Hang ioapic information off of here */
};
struct pci_dev dev1_1e_0 = {
	.bus = &bus1,
	.devfn = PCI_DEVFN(0x1e, 0),
	/* Hang ioapic information off of here */
};

