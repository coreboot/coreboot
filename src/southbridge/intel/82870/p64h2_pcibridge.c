#include <pci.h>
#include <arch/io.h>
#include <printk.h>
#include <pci_ids.h>
#include <southbridge/intel/82801.h>
#include "82870.h"

void p64h2_setup_pcibridge(void)
{
	struct pci_dev *dev;
	u32 dword;
	u16 word;
	u8 byte;
	int i;


	for(i=0,dev=0;i<4;i++) {  
		/* find the next pci bridge p64h2 - 1460 */
		dev = pci_find_device(PCI_VENDOR_ID_INTEL, 
				PCI_DEVICE_ID_INTEL_82870_1F0, dev); 
		if (!dev) {
		    if(i<2)		/* there should be at least 2 */
	  		printk_debug("*** ERROR Southbridge device %x not found\n",
				PCI_DEVICE_ID_INTEL_82870_1F0);
		    break;
		}
		/* The purpose of changes to HCCR, ACNF, and MTT is to speed up the 
		   PCI bus for cards having high speed transfers. */
		dword = 0xc2040002;
		pci_write_config_dword(dev, HCCR, dword);
		dword = 0x0000c3bf;
		pci_write_config_dword(dev, ACNF, dword);
		byte = 0x08;
		pci_write_config_byte(dev, MTT, byte);

	}

}
