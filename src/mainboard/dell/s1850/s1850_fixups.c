#include <arch/romcc_io.h>

static void mch_reset(void)
{
        return;
}



static void mainboard_set_e7520_pll(unsigned bits)
{
	return; 
}


static void mainboard_set_e7520_leds(void)
{
	return; 
}

static void mainboard_set_ich5(void)
{
	/* coma is 0x3f8 , comb is 0x2f8*/
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xe0, 0x10);
	/* enable decoding of various devices */
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0xe6, 0x140f);
	/* 1M flash */
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xe3, 0xc0);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xf0, 0x0);
	/* disable certain devices -- see data sheet -- this is from
	 * dell settings via lspci 
	 * Note that they leave SMBUS disabled -- 8f6f. 
	 * we leave it enabled and visible in config space -- 8f66
	 */
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0xf2, 0x8f66);
}




