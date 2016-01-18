#include <arch/io.h>
#define PCIEXBAR 0x60
#define MMCONF_256_BUSSES 16
#define ENABLE 1

static void bootblock_northbridge_init(void)
{
	pci_io_write_config32(PCI_DEV(0,0,0), PCIEXBAR,
		CONFIG_MMCONF_BASE_ADDRESS | MMCONF_256_BUSSES | ENABLE);
}
