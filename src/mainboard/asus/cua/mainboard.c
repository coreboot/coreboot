#include <printk.h>
#include <pci.h>

#include <cpu/p5/io.h>

void mainboard_fixup()
{
	struct pci_dev *pm_pcidev, *host_bridge_pcidev, *nic_pcidev;
	unsigned smbus_io, pm_io;
	unsigned int i, j;
	printk("intel_mainboard_fixup()\n");

#if 0
	// put in the right values for acer stuff
	pm_pcidev = pci_find_device(0x8086, 0x7113, 0);
	nic_pcidev = pci_find_device(0x8086, 0x1229, 0);
	host_bridge_pcidev = pci_find_slot(0, PCI_DEVFN(0,0));
#endif
}

void final_mainboard_fixup()
{
    void final_northbridge_fixup(void);

    final_northbridge_fixup();
}
