#include <mem.h>
#include <part/sizeram.h>
#include <pci.h>


struct mem_range *sizeram(void)
{
	static struct mem_range mem[3];
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

	mem[0].basek = 0;
	mem[0].sizek = 640;
	mem[1].basek = 1024;
	mem[1].sizek = totalmem - mem[1].basek;
	mem[2].basek = 0;
	mem[2].sizek = 0;

	return &mem;
}


