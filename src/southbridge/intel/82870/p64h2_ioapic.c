#include <pci.h>
#include <arch/io.h>
#include <printk.h>
#include <pci_ids.h>
#include <southbridge/intel/82801.h>
#include "82870.h"

void p64h2_enable_ioapic(void)
{
	struct pci_dev *dev;
	u32 dword;
	u16 word;
	int i,addr;

        volatile u32 *ioapic_a;	/* io apic io memory space command address */
        volatile u32 *ioapic_d;	/* io apic io memory space data address */

	for(i=0,dev=0;i<4;i++) {  
		if(i<3)			/* io apic address numbers are 3,4,5,&8 */ 
			addr=i+3;
		else
			addr=i+5;
		/* find the next pci io apic p64h2 - 1461 */
		dev = pci_find_device(PCI_VENDOR_ID_INTEL, 
				PCI_DEVICE_ID_INTEL_82870_1E0, dev); 
		if (!dev) {
		    if(i<2)		/* there should be at least 2 */
	  		printk_debug("*** ERROR Southbridge device %x not found\n",
				PCI_DEVICE_ID_INTEL_82870_1E0);
		    break;
		}
		/* Read the MBAR address for setting up the io apic in io memory space */
		pci_read_config_dword(dev, PCI_BASE_ADDRESS_0, &dword);
		ioapic_a = (u32 *) dword;
		ioapic_d = ioapic_a +0x04;
		printk_debug("IOAPIC %d at %02x:%02x.%01x  MBAR = %x DataAddr = %x\n",
			addr, dev->bus->secondary,
			PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn),
			ioapic_a, ioapic_d);
		word = 0x0146;
		pci_write_config_word(dev, PCICMD, word);
		dword = 0x358015d9;
		pci_write_config_dword(dev, SUBSYS, dword);
#if 0
		/* writing to the ABAR is removed here because there is a hardware
		error on the super micro boards, and it is not needed. */
		dword = (u32)ioapic_a;
		word = 0x8000 + ((dword >>8)&0x0fff);
		pci_write_config_word(dev, ABAR, word);
#endif
		/* Set up the io apic for the p64h2 - 1461 */		
	        *ioapic_a=0;
	        *ioapic_d=(addr<<24); /* Set the address number */
	        *ioapic_a=3;
	        *ioapic_d=1;	/* Enable the io apic */

		/* This code test the setup to see if we really found the io apic */
	        *ioapic_a=0;
	        dword=*ioapic_d;
	        printk_debug("PCI %d apic id = %x\n",addr,dword);
	        if(dword!=(addr<<24))
	                for(;;);
	        *ioapic_a=3;
	        dword=*ioapic_d;
	        printk_debug("PCI %d apic DT = %x\n",addr,dword);
	        if(dword!=1)
	                for(;;);

	}

}
