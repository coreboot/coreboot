#include <pci.h>
#include <cpu/p5/io.h>
#include <printk.h>

void
dumpramregs(struct pci_dev *pcidev)
{

}
unsigned long sizeram()
{
	unsigned long totalmem;
	unsigned char banks;

        struct pci_dev *pcidev;

	/* pci_find_device is way overkill for the host bridge! 
	 * Plus the BX & GX have different device numbers so it
	 * prevents code sharing.
	 */
	pcidev = pci_find_slot(0, PCI_DEVFN(0,0));
        //pci_read_config_byte(pcidev, 0x67, &banks);

	dumpramregs(pcidev);

	return totalmem;
}


