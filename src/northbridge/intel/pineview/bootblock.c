#include <arch/io.h>
#define PCIEXBAR 0x60

static void bootblock_northbridge_init(void)
{
	pci_io_write_config32(PCI_DEV(0,0,0), PCIEXBAR,
		CONFIG_MMCONF_BASE_ADDRESS | 4 | 1);
}
