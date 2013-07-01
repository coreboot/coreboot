#include <arch/io.h>

static void bootblock_northbridge_init(void)
{
	/*
	 * The "io" variant of the config access is explicitly used to
	 * setup the PCIEXBAR because CONFIG_MMCONF_SUPPORT_DEFAULT is set to
	 * to true. That way all subsequent non-explicit config accesses use
	 * MCFG. This code also assumes that bootblock_northbridge_init() is
	 * the first thing called in the non-asm boot block code. The final
	 * assumption is that no assembly code is using the
	 * CONFIG_MMCONF_SUPPORT_DEFAULT option to do PCI config acceses.
	 *
	 * The PCIEXBAR is assumed to live in the memory mapped IO space under
	 * 4GiB.
	 */

	/* setup PCIe MMCONF base address */
	pci_io_write_config32(PCI_DEV(0, 16, 0), 0x64,
			   CONFIG_MMCONF_BASE_ADDRESS >> 16);
}
