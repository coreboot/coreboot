#include <pci.h>
#include <pci_ids.h>
#include <southbridge/intel/82801.h>
#include "82801.h"

void ich2_enable_serial_irqs(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801BA_1F0, 0);
	if (!dev) {
		return;
	}
	pci_write_config_byte(dev, SERIRQ_CNTL, (1 << 7)|(1 << 6)|((21 - 17) << 2)|(0 << 0));
}

void ich2_lpc_route_dma(unsigned char mask)
{
	struct pci_dev *dev;
	u16 word;
	int i;
	dev = pci_find_device(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801BA_1F0, 0);
	if (!dev) {
		return;
	}
	pci_read_config_word(dev, PCI_DMA_CFG, &word);
	word &= ((1 << 10) - (1 << 8));
	for(i = 0; i < 8; i++) {
		if (i == 4) 
			continue;
		word |= ((mask & (1 << i))? 3:1) << (i*2);
	}
	pci_write_config_word(dev, PCI_DMA_CFG, word);
}
