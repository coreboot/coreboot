#include <pci.h>


unsigned long sizeram()
{
	/*
	 * Code for the BX/GX and TX are almost the same
	 */
	unsigned long totalmem;
	unsigned char banks;

        struct pci_dev *pcidev;

	/* pci_find_device is way overkill for the host bridge! 
	 */
	pcidev = pci_find_slot(0, PCI_DEVFN(0,0));
        pci_read_config_byte(pcidev, 0x65, &banks);

	/* RAM is in 4 MBytes granularity */
	totalmem = (unsigned long) banks * 4 * 1024;

	/*
	 * This is just a failsafe, we SHOULDN'T come
	 * here! It is hardlimited to 512M, more than the max amount
	 * supported by the 430TX chipset
	 */ 
	if (banks == 0) {
		totalmem = 0x20000000UL;
	}

	return totalmem;
}


