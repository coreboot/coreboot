#include <printk.h>
#include <pci.h>
#include <pciconf.h>


unsigned long sizeram()
{
	unsigned long totalmem;
	unsigned char bank, mem, prevmem;
	// fix me later -- there are two more banks at 0x56 and 0x57
	unsigned long firstbank = 0x5a, lastbank = 0x5f;
	u8 sma_status, sma_size, sma_size_bits;	
	
        struct pci_dev *pcidev;

	pcidev = pci_find_slot(0, PCI_DEVFN(0,0));

	if (! pcidev)
		return 0;

	pci_read_config_byte(pcidev, 0xfb, &sma_status);
	sma_size_bits = (sma_status >> 4) & 0x03;
	if (sma_size_bits > 3)
		sma_size = 0;
	else
		sma_size = 0x01 << sma_size_bits;

	for(totalmem = mem = prevmem = 0, bank = firstbank; 
	    bank <= lastbank; bank++) {
		pci_read_config_byte(pcidev, bank, &mem);
		// sanity check. If the mem value is < prevmem, 
		// that is an error, so skip this step. 
		if (mem < prevmem) {
			printk_err("ERROR: bank 0x%x, mem 0x%x TOO SMALL\n",
				bank, prevmem);
			printk_err("Should be >= 0x%x\n", prevmem);
		} else 
			totalmem += (mem - prevmem) * 8;
		prevmem = mem;
	}
	
	totalmem -= sma_size;
	totalmem *= 1024;

	printk_info("sizeram: returning 0x%x KB\n", totalmem);
	printk_info("sizeram: NOT returning 0x%x KB\n", totalmem);
	printk_info("sizeram: there are still some SPD problems ... \n");
	totalmem = 64 * 1024;
	printk_info("sizeram: SO we return only 0x%x KB\n", totalmem);
#if 0
#endif
	return totalmem;
}


/*
unsigned long sizeram()
{
	return 0;
}
*/

#ifdef HAVE_FRAMEBUFFER
void framebuffer_on()
{
	unsigned long devfn;
	u16 command;

	devfn = PCI_DEVFN(0, 1);
	pcibios_read_config_word(0, devfn, 0x3e, &command);
	command |= 0x08;
	pcibios_write_config_word(0, devfn, 0x3e, command);
}
#endif
