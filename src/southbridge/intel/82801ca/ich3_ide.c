#include <pci.h>
#include <pci_ids.h>
#include <southbridge/intel/82801.h>

void ich3_enable_ide(int enable_a, int enable_b)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801CA_1F1, 0);
	if (dev != NULL) {
		/* Enable ide devices so the linux ide driver will work */
		u16 word;
		/* enable ide0 */
		pci_read_config_word(dev, 0x40, &word);
		word &= ~(1 << 15);
		word |= ((!!enable_a) << 15);
		pci_write_config_word(dev, 0x40, word);
		/* enable ide1 */
		pci_read_config_word(dev, 0x42, &word);
		word &= ~(1 << 15);
		word |= ((!!enable_b) << 15);
		pci_write_config_word(dev, 0x42, word);
	}

}
