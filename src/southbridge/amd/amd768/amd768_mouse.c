#include <pci.h>
#include <pci_ids.h>
#include <southbridge/amd/amd768.h>

void amd768_mouse_sends_irq12(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7443, 0);	
	if (dev != NULL) {
		unsigned short word;
		/* Setup so irq12 is sent by the ps2 mouse port. */
		pci_read_config_word(dev, 0x46, &word);
		pci_write_config_word(dev, 0x46, word | (1<<9));
	}
}
