#include <pci.h>
#include <cpu/p5/io.h>

unsigned long sizeram()
{
	/*
	 * This is written for BX but should work also for GX.
	 */
	unsigned long totalmem;
	unsigned char banks;

        struct pci_dev *pcidev;

	/* pci_find_device is way overkill for the host bridge! 
	 * Plus the BX & GX have different device numbers so it
	 * prevents code sharing.
	 */
	pcidev = pci_find_slot(0, PCI_DEVFN(0,0));
        pci_read_config_byte(pcidev, 0x67, &banks);

	totalmem = (unsigned long) banks *8 * 1024;

	if (banks == 0) {
		totalmem = 0x80000000UL;
	}

	return totalmem;
}


#ifdef HAVE_FRAMEBUFFER

void framebuffer_on()
{
	outl(0x8000a004, 0xcf8);
	outb(0x03, 0xcfc);
}
#endif
