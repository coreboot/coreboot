#include <mem.h>
#include <part/sizeram.h>
#include <printk.h>
#include <pci.h>
#include <pciconf.h>


static unsigned long __sizeram(void)
{
	unsigned long totalmem;
	unsigned char bank, mem, prevmem;
	// fix me later -- there are two more banks at 0x56 and 0x57
	unsigned long firstbank = 0x5a, lastbank = 0x5d;
	u8 sma_status, sma_size, sma_size_bits;	
	u8 val;
	
        struct pci_dev *pcidev;

	pcidev = pci_find_slot(0, PCI_DEVFN(0,0));

	if (! pcidev)
		return 0;

	pci_read_config_byte(pcidev, 0xe1, &sma_status);
	sma_size_bits = (sma_status >> 4) & 0x07;
	if (sma_size_bits > 7)
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
			totalmem += (mem - prevmem) * 16;
		prevmem = mem;
	}
	
	totalmem -= sma_size;
	totalmem *= 1024;

	return totalmem;
}

struct mem_range *sizeram(void)
{
	static struct mem_range mem[3];
	mem[0].basek = 0;
	mem[0].sizek = 640;
	mem[1].basek = 1024;
	mem[1].sizek = __sizeram();
	mem[2].basek = 0;
	mem[2].sizek = 0;
	if (mem[1].sizek == 0) {
		mem[1].sizek = 64*1024;
	}
	mem[1].sizek -= mem[1].basek;
	return mem;
}

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

/*
 * This fixup is based on capturing values from an Award bios.  Without
 * this fixup the DMA write performance is awful (i.e. hdparm -t /dev/hda is
 * slower than normal, ethernet drops packets).
 * Apparently these registers govern some sort of bus master behavior.
 */
void northbridge_fixup(void)
{
	struct pci_dev *pcidev = pci_find_slot(0, PCI_DEVFN(0,0));

	if (pcidev) {
		pci_write_config_byte(pcidev, 0x70, 0xc0);
		pci_write_config_byte(pcidev, 0x71, 0x88);
		pci_write_config_byte(pcidev, 0x75, 0x01);
		pci_write_config_byte(pcidev, 0x76, 0x52);
	}
}

