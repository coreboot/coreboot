#include <pci.h>
#include <pci_ids.h>
#include "82801.h"
void ich2_enable_serial_irqs(void)
{
	struct pci_dev *dev;
	u32 dword;
	dev = pci_find_device(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801BA_1F0, 0);
	if (!dev) {
		return;
	}
	pci_write_config_byte(dev, SERIRQ_CNTL, (1 << 7)|(1 << 6)|((21 - 17) << 2)|(0 << 0));
}
