#include <pci.h>
#include <pci_ids.h>
#include <arch/io.h>
#include <southbridge/amd/amd768.h>
#include <ide.h>

void amd768_enable_ide(int enable_a, int enable_b)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7441, 0);
	if (dev != NULL) {
		/* Enable ide devices so the linux ide driver will work */
		u16 word;
		u8  byte;
		pci_read_config_word(dev, 0x40, &word);
		/* Ensure prefetch is disabled */
		word &= ~((1 << 15) | (1 << 13));
		if (enable_b) {
			/* Enable secondary ide interface */
			word |= (1<<0);
		}
		if (enable_a) {
			/* Enable primary ide interface */
			word |= (1<<1);
		}

		word |= (1<<12);
		word |= (1<<14);

		pci_write_config_word(dev, 0x40, word);

		word = 0x0f;
		pci_write_config_word(dev, 0x42, word);

		/* The AMD768 has a bug where the BM DMA address must be
		 * 256 byte aligned while it is only 16 bytes long.
		 * Hard code this to a valid address below 0x1000
		 * where automatic port address assignment starts.
		 */
		pci_write_config_dword(dev, 0x20, 0xf01);

		pci_write_config_dword(dev, 0x48, 0x205e5e5e);
		word = 0x06a;
		pci_write_config_word(dev, 0x4c, word);

	} else
		printk_alert("Could not locate IDE controller\n");

}
