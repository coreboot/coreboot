#include <pci_ids.h>
#include <pci.h>
#include <cpu/p5/io.h>
#include <part/nvram.h>

void nvram_on(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7440, 0);
	if (dev != NULL) {
		u8 segen;
		pci_read_config_byte(dev, 0x43, &segen);
		/* Enable 4MB rom access at 0xFFC00000 - 0xFFFFFFFF */
		segen |= 0x80;
		pci_write_config_byte(dev, 0x43, segen);
	}
}
