#include <pci.h>
#include <arch/io.h>
#include <printk.h>
#include <pci_ids.h>
#include <southbridge/intel/82801.h>
#include "82801.h"

void ich2_enable_ioapic(void)
{
	struct pci_dev *dev;
	u32 dword;
	dev = pci_find_device(PCI_VENDOR_ID_INTEL, 
			PCI_DEVICE_ID_INTEL_82801BA_1F0, 0);
	if (!dev) {
  		printk_debug("*** ERROR Southbridge device %x not found\n",
			PCI_DEVICE_ID_INTEL_82801BA_1F0);
		return;
	}
	pci_read_config_dword(dev, GEN_CNTL, &dword);
	dword |= (3 << 7);
	pci_write_config_dword(dev, GEN_CNTL, dword);
  		printk_debug("ioapic southbridge enabled %x\n",dword);
}
