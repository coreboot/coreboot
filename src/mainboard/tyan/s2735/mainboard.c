#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "chip.h"
//#include <part/mainboard.h>
//#include "lsi_scsi.c"

#if 0
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
#endif
//extern static void lsi_scsi_init(struct device *dev);
#if 0
static void print_pci_regs(struct device *dev)
{
      uint8_t byte;
      int i;

      for(i=0;i<256;i++) {
	     byte = pci_read_config8(dev, i);
   
             if((i%16)==0) printk_info("\n%02x:",i);
             printk_debug(" %02x",byte);
      }
      printk_debug("\n");
	
//        pci_write_config8(dev, 0x4, byte);

}
#endif
#if 0
static void print_mem(void)
{
        int i;
	int low_1MB = 0;
	for(i=low_1MB;i<low_1MB+1024*4;i++) {
             if((i%16)==0) printk_debug("\n %08x:",i);
             printk_debug(" %02x ",(unsigned char)*((unsigned char *)i));
             }

        for(i=low_1MB;i<low_1MB+1024*4;i++) {
             if((i%16)==0) printk_debug("\n %08x:",i);
             printk_debug(" %c ",(unsigned char)*((unsigned char *)i));
             }
 }
#endif
#if 0
static void onboard_scsi_fixup(void)
{
       struct device *dev;
	unsigned char i,j,k;
#if 1
	for(i=0;i<=5;i++) {
		for(j=0;j<=0x1f;j++) {
			for (k=0;k<=6;k++){
				dev = dev_find_slot(i, PCI_DEVFN(j, k));
				if (dev) {
			                printk_debug("%02x:%02x:%02x",i,j,k);
					print_pci_regs(dev);
        			}
			}
		}
	}
#endif


#if 0
        dev = dev_find_device(PCI_VENDOR_ID_LSI_LOGIC, PCI_DEVICE_ID_LSI_53C1030,0);
        if(!dev) {
                printk_info("LSI_SCSI_FW_FIXUP: No Device Found!");
                return;
        }

	lsi_scsi_init(dev); 
#endif
//	print_mem();
//	amd8111_enable_rom();
}
#endif
/*
static void vga_fixup(void) {
        // we do this right here because:
        // - all the hardware is working, and some VGA bioses seem to need
        //   that
        // - we need page 0 below for linuxbios tables.
#if CONFIG_REALMODE_IDT == 1
        printk_debug("INSTALL REAL-MODE IDT\n");
        setup_realmode_idt();
#endif
#if CONFIG_VGABIOS == 1
        printk_debug("DO THE VGA BIOS\n");
        do_vgabios();
        post_code(0x93);
#endif

}
 */

static int
mainboard_scan_bus(device_t root, int maxbus)
{
        int retval;
        printk_spew("%s: root %p maxbus %d\n", __FUNCTION__, root, maxbus);
        retval = pci_scan_bus(root->bus, 0, 0xff, maxbus);
        printk_spew("DONE %s: return %d\n", __FUNCTION__, maxbus);
        return maxbus;
}

static struct device_operations mainboard_operations = {
        .read_resources   = root_dev_read_resources,
        .set_resources    = root_dev_set_resources,
        .enable_resources = root_dev_enable_resources,
        .init             = root_dev_init,
        .scan_bus         = mainboard_scan_bus,
};

static void enable_dev(device_t dev)
{
	dev->ops = &mainboard_operations;
}

struct chip_operations mainboard_tyan_s2735_ops = {
        .enable_dev = enable_dev,
        .name      = "Tyan s2735 mainboard ",
};

