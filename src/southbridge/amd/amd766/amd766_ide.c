#include <pci.h>
#include <pci_ids.h>
#include <southbridge/amd/amd766.h>
#include <ide.h>

void amd766_enable_ide(int enable_a, int enable_b)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7411, 0);
	if (dev != NULL) {
		/* Enable ide devices so the linux ide driver will work */
		u16 word;
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
		pci_write_config_word(dev, 0x40, word);
		/* The AMD766 has a bug where the BM DMA address must be
		 * 256 byte aligned while it is only 16 bytes long.
		 * Hard code this to a valid address below 0x1000
		 * where automatic port address assignment starts.
		 */
		pci_write_config_dword(dev, 0x20, 0xf01);
#if 0
		if (enable_a) {
			ide_probe(0x1f0, 0);
		}
		if (enable_b) {
			ide_probe(0x170, 2);
		}
#endif
#if 0
		if (enable_a) {
			ide_spinup(0x1f0, 0);
		}
		if (enable_b) {
			ide_spinup(0x170, 2);
		}
#endif
	}
}
