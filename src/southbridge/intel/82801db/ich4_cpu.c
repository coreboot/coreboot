#include <pci.h>
#include <pci_ids.h>
#include <southbridge/intel/82801.h>
#include <printk.h>
#include <part/hard_reset.h>
#include <part/fallback_boot.h>
#include "82801.h"

void ich3_set_cpu_multiplier(unsigned multiplier)
{
	u32 dword, old_dword;
	struct pci_dev *dev;
	unsigned old_multiplier;
	dev = pci_find_device(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801CA_1F0, 0);
	if (!dev) {
		printk_err("Cannot find device %08x:%08x\n",
			PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801CA_1F0);
		return;
	}
	pci_read_config_dword(dev, GEN_STS, &old_dword);
	dword = old_dword;
	dword &= ~((1 << 12) - (1 << 8));
#if 0
	dword |= (multiplier & 0xf) << 8;
#else
	dword |= (0x0c) << 8;
#endif
	if (dword != old_dword) {
		dword |= (1<<1);
		pci_write_config_dword(dev, GEN_STS, dword);
		printk_info("Rebooting to change the cpu multiplier\n");
		boot_successful();
		hard_reset();
	}
	dword |= (1<<1);
	pci_write_config_dword(dev, GEN_STS, dword);
}
