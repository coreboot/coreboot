#include <pci.h>
#include <pci_ids.h>
#include <southbridge/intel/82801.h>
#include <printk.h>
#include "82801.h"

void ich2_set_cpu_multiplier(unsigned multiplier)
{
	u32 dword;
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801BA_1F0, 0);
	if (!dev) {
		printk_err("Cannot find device %08x:%08x\n",
			PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801BA_1F0);
		return;
	}
	pci_read_config_dword(dev, GEN_STS, &dword);
	dword &= ~((1 << 12) - (1 << 8));
	dword |= (multiplier & 0xf) << 8;
	pci_write_config_dword(dev, GEN_STS, dword);
}
