
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#include <arch/io.h>
#include <device/chip.h>
#include "chip.h"

void vga_enable_console();


static int
mainboard_scan_bus(device_t root, int maxbus) 
{
	int retval;
	printk_spew("%s: root %p maxbus %d\n", __FUNCTION__, root, maxbus);
	retval = pci_scan_bus(root->bus, 0, 0xff, maxbus);
	printk_spew("DONE %s: return %d\n", __FUNCTION__, maxbus);
	return maxbus;
}

void vga_fixup(void) {
        // we do this right here because:
        // - all the hardware is working, and some VGA bioses seem to need
        //   that
        // - we need page 0 below for linuxbios tables.

        printk_debug("INSTALL REAL-MODE IDT\n");
        setup_realmode_idt();
        printk_debug("DO THE VGA BIOS\n");
        do_vgabios();
        post_code(0x93);
	vga_enable_console();


}
 
void write_protect_vgabios(void)
{
 	device_t dev;
 
 	printk_info("write_protect_vgabios\n");
 	dev = dev_find_device(PCI_VENDOR_ID_VIA, 0x3123, 0);
 	if(dev)
 		pci_write_config8(dev, 0x61, 0xaa);
 
}

static void
enable(struct chip *chip, enum chip_pass pass)
{

        struct mainboard_tyan_s4882_config *conf = 
		(struct mainboard_tyan_s4882_config *)chip->chip_info;

        switch (pass) {
		default: break;
//		case CONF_PASS_PRE_CONSOLE:
//		case CONF_PASS_PRE_PCI:
		case CONF_PASS_POST_PCI:		
//                case CONF_PASS_PRE_BOOT:
//			if (conf->fixup_scsi)
//        			onboard_scsi_fixup();
//			if (conf->fixup_vga)
//				vga_fixup();
			printk_debug("mainboard fixup pass %d done\r\n",
					pass);
			break;
	}

}
static struct device_operations mainboard_operations = {
	.read_resources   = root_dev_read_resources,
	.set_resources    = root_dev_set_resources,
	.enable_resources = enable_childrens_resources,
	.init             = 0,
	.scan_bus         = mainboard_scan_bus,
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
struct chip_control mainboard_via_epia_m_control = {
	.enumerate = enumerate, 
	.name      = "VIA EPIA-M mainboard ",
	.enable = enable
};

