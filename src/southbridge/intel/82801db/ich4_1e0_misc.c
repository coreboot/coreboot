#include <pci.h>
#include <arch/io.h>
#include <printk.h>
#include <pci_ids.h>
#include <southbridge/intel/82801.h>
#include "82801.h"

void ich3_1e0_misc(void)
{
	struct pci_dev *dev;

	dev = pci_find_device(PCI_VENDOR_ID_INTEL, 
			PCI_DEVICE_ID_INTEL_82801CA_1E0, 0);
	if (!dev) {
  		printk_debug("*** ERROR Southbridge device %x not found\n",
			PCI_DEVICE_ID_INTEL_82801CA_1E0);
		return;
	}
#if 0
	pci_write_config_word(dev, CMD, 0x0147);
	pci_write_config_byte(dev, SMLT, 0x40);
	pci_write_config_byte(dev, IOBASE, 0x20);
	pci_write_config_byte(dev, IOLIM, 0x20);
	pci_write_config_word(dev, MEMBASE, 0xd410);
	pci_write_config_word(dev, MEMLIM, 0xd5f0);
	pci_write_config_byte(dev, CNF, 0x02);
	pci_write_config_byte(dev, MTT, 0x40);
	pci_write_config_byte(dev, PCI_MAST_STS, 0x86);
#endif

}
