#include <pci.h>
#include <arch/io.h>
#include <printk.h>
#include <pci_ids.h>
#include <southbridge/intel/82801.h>
#include "82801.h"

void ich3_enable_ioapic(void)
{
	struct pci_dev *dev;
	u32 dword;
	volatile u32 *ioapic_sba = (volatile u32 *)0xfec00000;
	volatile u32 *ioapic_sbd = (volatile u32 *)0xfec00010;

	dev = pci_find_device(PCI_VENDOR_ID_INTEL, 
			PCI_DEVICE_ID_INTEL_82801CA_1F0, 0);
	if (!dev) {
  		printk_debug("*** ERROR Southbridge device %x not found\n",
			PCI_DEVICE_ID_INTEL_82801CA_1F0);
		return;
	}
	pci_read_config_dword(dev, GEN_CNTL, &dword);
	dword |= (3 << 7); /* enable ioapic */
	dword |= (1 <<13); /* coprocessor error enable */
	dword |= (1 << 1); /* delay transaction enable */
	dword |= (1 << 2); /* DMA collection buf enable */
	pci_write_config_dword(dev, GEN_CNTL, dword);
  		printk_debug("ioapic southbridge enabled %x\n",dword);
	*ioapic_sba=0;
	*ioapic_sbd=(2<<24);	
	*ioapic_sba=3;
	*ioapic_sbd=1;	
	*ioapic_sba=0;
	dword=*ioapic_sbd;
	printk_debug("Southbridge apic id = %x\n",dword);
	if(dword!=(2<<24))
		for(;;);
	*ioapic_sba=3;
	dword=*ioapic_sbd;
	printk_debug("Southbridge apic DT = %x\n",dword);
	if(dword!=1)
		for(;;);


}
