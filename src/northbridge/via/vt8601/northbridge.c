#include <printk.h>
#include <pci.h>

unsigned long sizeram()
{
	unsigned long totalmem;
	unsigned char bank, mem, prevmem;
	// fix me later -- there are two more banks at 0x56 and 0x57
	unsigned long firstbank = 0x5a, lastbank = 0x5f;
	
        struct pci_dev *pcidev;

	pcidev = pci_find_slot(0, PCI_DEVFN(0,0));

	if (! pcidev)
		return 0;

	for(totalmem = mem = prevmem = 0, bank = firstbank; 
	    bank <= lastbank; bank++) {
		pci_read_config_byte(pcidev, bank, &mem);
		totalmem += (mem - prevmem) * 8 * 1024;
		prevmem = mem;
	}

	printk("sizeram: returning 0x%x KB\n", totalmem);
#if 0
	printk("sizeram: NOT returning 0x%x KB\n", totalmem);
	printk("sizeram: there are still some SPD problems ... \n");
	totalmem = 64 * 1024;
	printk("sizeram: SO we return only 0x%x KB\n", totalmem);
#endif
	return totalmem;
}
