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
	int i;
        volatile u32 *ioapic_1a=0xfec80000;
        volatile u32 *ioapic_1d=0xfec80010;
        volatile u32 *ioapic_2a=0xfec80400;
        volatile u32 *ioapic_2d=0xfec80410;



	for(i=0,dev=0;i<4;i++) {
		dev = pci_find_device(PCI_VENDOR_ID_INTEL, 
				PCI_DEVICE_ID_INTEL_82870_1F0, dev); 
		if (!dev) {
		    if(i<2)
	  		printk_debug("*** ERROR Southbridge device %x not found\n",
				PCI_DEVICE_ID_INTEL_82870_1F0);
		    break;
		}
//		pci_read_config_dword(dev, GEN_CNTL, &dword);
		word = 0x0146;
		pci_write_config_word(dev, PCICMD, word);
		dword = 0x358015d9;
		pci_write_config_dword(dev, SUBSYS, dword);
		if(i==0){
		    dword = 0xfc100000;
		    pci_write_config_dword(dev, MBAR, dword);
		    word = 0x8804;
		    pci_write_config_word(dev, ABAR, word);
		}
		if(i==1){
		    dword = 0xfc101000;
		    pci_write_config_dword(dev, MBAR, dword);
		    word = 0x8800;
		    pci_write_config_word(dev, ABAR, word);
		}
	}

        *ioapic_1a=0;
        *ioapic_1d=(3<<24);
        *ioapic_1a=3;
        *ioapic_1d=1;
        *ioapic_1a=0;
        dword=*ioapic_1d;
        printk_debug("PCI 1 apic id = %x\n",dword);
        if(dword!=(3<<24))
                for(;;);
        *ioapic_1a=3;
        dword=*ioapic_1d;
        printk_debug("PCI 1 apic DT = %x\n",dword);
        if(dword!=1)
                for(;;);

        *ioapic_2a=0;
        *ioapic_2d=(4<<24);
        *ioapic_2a=3;
        *ioapic_2d=1;
        *ioapic_2a=0;
        dword=*ioapic_2d;
        printk_debug("PCI 2 apic id = %x\n",dword);
        if(dword!=(4<<24))
                for(;;);
        *ioapic_2a=3;
        dword=*ioapic_2d;
        printk_debug("PCI 2 apic DT = %x\n",dword);
        if(dword!=1)
                for(;;);


}
