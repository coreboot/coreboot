#include <console/console.h>
#include <device/device.h>
#include <device/chip.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "chip.h"
//#include <part/mainboard.h>
//#include "lsi_scsi.c"
unsigned long initial_apicid[MAX_CPUS] =
{
	0,1
};
/*
static void fixup_lsi_53c1030(struct device *pdev)
{
//	uint8_t byte;
	uint16_t word;

	byte = 1;
        pci_write_config8(pdev, 0xff, byte);
           // Set the device id 
//      pci_write_config_word(pdev, PCI_DEVICE_ID, PCI_DEVICE_ID_LSILOGIC_53C1030);
           // Set the subsytem vendor id 
//      pci_write_config16(pdev, PCI_SUBSYSTEM_VENDOR_ID, PCI_VENDOR_ID_TYAN);  
        word = 0x10f1;
	pci_write_config16(pdev, PCI_SUBSYSTEM_VENDOR_ID, word);
            // Set the subsytem id 
	word = 0x2880;
        pci_write_config16(pdev, PCI_SUBSYSTEM_ID, word);
            // Disable writes to the device id 
	byte = 0;
        pci_write_config8(pdev, 0xff, byte);

//	lsi_scsi_init(pdev);
	
}
//extern static void lsi_scsi_init(struct device *dev);
static void print_pci_regs(struct device *dev)
{
      uint8_t byte;
      int i;

      for(i=0;i<256;i++) {
	     byte = pci_read_config8(dev, i);
   
             if((i%16)==0) printk_info("\n %02x:",i);
             printk_debug(" %02x ",byte);
      }
      printk_debug("\r\n");
	
//        pci_write_config8(dev, 0x4, byte);

}
*/
static void onboard_scsi_fixup(void)
{
       struct device *dev;
/*
       // Set the scsi device id's 
       printk_debug("%2d:%2d:%2d\n",0,1,0);
	dev = dev_find_slot(0, PCI_DEVFN(0x1, 0));
       if (dev) {
        }
        // Set the scsi device id's 
       printk_debug("%2d:%2d:%2d\n",0,2,0);
        dev = dev_find_slot(0, PCI_DEVFN(0x2, 0));
        if (dev) {
		print_pci_regs(dev);
        }
 
      // Set the scsi device id's
       printk_debug("%2d:%2d:%2d\n",1,0xa,0);
       dev = dev_find_slot(1, PCI_DEVFN(0xa, 0));
       if (dev) {
                print_pci_regs(dev);
        }
        // Set the scsi device id's
       printk_debug("%2d:%2d:%2d\n",1,0xa,1);
        dev = dev_find_slot(1, PCI_DEVFN(0xa, 1));
        if (dev) {
                print_pci_regs(dev);
        }
       printk_debug("%2d:%2d:%2d\n",1,9,0);
       dev = dev_find_slot(1, PCI_DEVFN(0x9, 0));
       if (dev) {
                print_pci_regs(dev);
        }
        // Set the scsi device id's
       printk_debug("%2d:%2d:%2d\n",1,9,1);
        dev = dev_find_slot(1, PCI_DEVFN(0x9, 1));
        if (dev) {
                print_pci_regs(dev);
        }
*/

/*
        dev = dev_find_device(PCI_VENDOR_ID_LSI_LOGIC, PCI_DEVICE_ID_LSI_53C1030,0);
        if(!dev) {
                printk_info("LSI_SCSI_FW_FIXUP: No Device Found!");
                return;
        }

	lsi_scsi_init(dev); 
*/
}
 

static void
enable(struct chip *chip, enum chip_pass pass)
{

        struct mainboard_tyan_s2880_config *conf = 
		(struct mainboard_tyan_s2880_config *)chip->chip_info;

        switch (pass) {
		default: break;
		case CONF_PASS_PRE_BOOT:
			 if (conf->fixup_scsi)
        			onboard_scsi_fixup();
			printk_debug("mainboard fixup pass %d done\r\n",
					pass);
			break;
	}

}
void final_mainboard_fixup(void)
{
#if 0
        enable_ide_devices();
#endif
}

struct chip_control mainboard_tyan_s2880_control = {
	        enable: enable,
	        name:   "Tyan s2880 mainboard "
};

