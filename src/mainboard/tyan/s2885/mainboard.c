#include <console/console.h>
#include <device/device.h>
#include <device/chip.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "../../../northbridge/amd/amdk8/northbridge.h"
#include "chip.h"
//#include <part/mainboard.h>
unsigned long initial_apicid[CONFIG_MAX_CPUS] =
{
	0,1
};
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
   
             if((i%16)==0) printk_debug("\n%02x:",i);
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
static void amd8111_enable_rom(void)
{
        uint8_t byte;
        struct device *dev;

        /* Enable 4MB rom access at 0xFFC00000 - 0xFFFFFFFF */
        /* Locate the amd8111 */
        dev = dev_find_device(0x1022, 0x7468, 0);

        /* Set the 4MB enable bit bit */
        byte = pci_read_config8(dev, 0x43);
        byte |= 0x80;
        pci_write_config8(dev, 0x43, byte);
}
#endif
static void onboard_scsi_fixup(void)
{
        struct device *dev;
#if 0 
	unsigned char i,j,k;

	for(i=0;i<=6;i++) {
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
        do_vgabios(0x0600);
        post_code(0x93);
#endif

}
 

static void
enable(struct chip *chip, enum chip_pass pass)
{

        struct mainboard_tyan_s2885_config *conf = 
		(struct mainboard_tyan_s2885_config *)chip->chip_info;

        switch (pass) {
		default: break;
//		case CONF_PASS_PRE_CONSOLE:
//		case CONF_PASS_PRE_PCI:
//		case CONF_PASS_POST_PCI:		
                case CONF_PASS_PRE_BOOT:
			if (conf->fixup_scsi)
        			onboard_scsi_fixup();
			if (conf->fixup_vga)
				vga_fixup();
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
static struct device_operations mainboard_operations = {
        .read_resources   = root_dev_read_resources,
        .set_resources    = root_dev_set_resources,
        .enable_resources = enable_childrens_resources,
        .init             = 0,
        .scan_bus         = amdk8_scan_root_bus,
        .enable           = 0,
};

static void enumerate(struct chip *chip)
{
        struct chip *child;
        dev_root.ops = &mainboard_operations;
        chip->dev = &dev_root;
        chip->bus = 0;
        for(child = chip->children; child; child = child->next) {
                child->bus = &dev_root.link[0];
        }
}
struct chip_control mainboard_tyan_s2885_control = {
	.enable = enable,
        .enumerate = enumerate,
        .name      = "Tyan s2885 mainboard ",
};
