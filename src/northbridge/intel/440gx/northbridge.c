#include <mem.h>
#include <part/sizeram.h>
#include <pci.h>
#include <cpu/p5/io.h>
#include <printk.h>

void
dumpramregs(struct pci_dev *pcidev)
{
	int i, j;

	for(i = 0x50; i < 0x90; i += 16) {
		printk_err("%02x: ", i);
		for(j = 0; j < 16; j++) {
			unsigned char val;
			pci_read_config_byte(pcidev, i+j, &val);
			printk_err("%02x ", val);
		}
		printk_err("\n");
	}

	for(i = 0xc0; i < 0xe0; i += 16) {
		printk_err("%02x: ", i);
		for(j = 0; j < 16; j++) {
			unsigned char val;
			pci_read_config_byte(pcidev, i+j, &val);
			printk_err("%02x ", val);
		}
		printk_err("\n");
	}


}
struct mem_range *sizeram(void)
{
	static struct mem_range mem[4];
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
	dumpramregs(pcidev);

	mem[0].basek = 0;
	mem[0].sizek = 640;
	mem[1].basek = 1024;
	mem[1].sizek = totalmem - 1024;
	mem[2].basek = 0;
	mem[2].sizek = 0;
	return &mem;
}


#ifdef HAVE_FRAMEBUFFER

void framebuffer_on()
{
	outl(0x8000a004, 0xcf8);
	outb(0x03, 0xcfc);
}
#endif
