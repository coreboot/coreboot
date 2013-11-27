#include <pci.h>
#include <arch/io.h>
#include <printk.h>
#

void p64h2_pci_parity_enable(void)
{
	uint8_t reg;

	/* 2SERREN - SERR enable for PCI bridge secondary device  */
	/* 2PEREN  - Parity error for PCI bridge secondary device  */
	pcibios_read_config_byte(1, ((29 << 3) + (0 << 0)), 0x3e, &reg);
	reg |= ((1 << 1) + (1 << 0));
	pcibios_write_config_byte(1, ((29 << 3) + (0 << 0)), 0x3e, reg);

	/* 2SERREN - SERR enable for PCI bridge secondary device  */
	/* 2PEREN  - Parity error for PCI bridge secondary device  */
	pcibios_read_config_byte(1, ((31 << 3) + (0 << 0)), 0x3e, &reg);
	reg |= ((1 << 1) + (1 << 0));
	pcibios_write_config_byte(1, ((31 << 3) + (0 << 0)), 0x3e, reg);

	return;
}


