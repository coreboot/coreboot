#include <arch/io.h>

static void bootblock_northbridge_init(void)
{
	pci_io_write_config32(PCI_DEV(0xff, 0x00, 1), 0x50, DEFAULT_PCIEXBAR | 1);
	pci_io_write_config32(PCI_DEV(0xff, 0x00, 1), 0x54, 0);
}
