#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "../../../northbridge/amd/amdk8/northbridge.h"
#include "chip.h"

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
        unsigned int i;
	unsigned int low_1MB = 0xf4107000;
	for(i=low_1MB;i<low_1MB+1024;i++) {
             if((i%16)==0) printk_debug("\n %08x:",i);
             printk_debug(" %02x ",(unsigned char)*((unsigned char *)i));
             }
#if 0
        for(i=low_1MB;i<low_1MB+1024*4;i++) {
             if((i%16)==0) printk_debug("\n %08x:",i);
             printk_debug(" %c ",(unsigned char)*((unsigned char *)i));
             }
#endif
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
#if 0
static void onboard_scsi_fixup(void)
{
        struct device *dev;
#if 1
	unsigned char i,j,k;

	for(i=0;i<=15;i++) {
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
#if 0
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

#endif
 
#if 0
static void
enable(struct chip *chip, enum chip_pass pass)
{

        struct mainboard_tyan_s2895_config *conf = 
		(struct mainboard_tyan_s2895_config *)chip->chip_info;

        switch (pass) {
		default: break;
//		case CONF_PASS_PRE_CONSOLE:
//		case CONF_PASS_PRE_PCI:
//		case CONF_PASS_POST_PCI:		
                case CONF_PASS_PRE_BOOT:
//			if (conf->fixup_scsi)
//        			onboard_scsi_fixup();
//			if (conf->fixup_vga)
//				vga_fixup();
			printk_debug("mainboard fixup pass %d done\r\n",
					pass);
			break;
	}

}
#endif

#undef DEBUG
#define DEBUG 0
#if DEBUG 
static void debug_init(device_t dev)
{
        unsigned bus;
        unsigned devfn;
#if 0
        for(bus = 0; bus < 256; bus++) {
                for(devfn = 0; devfn < 256; devfn++) {
                        int i;
                        dev = dev_find_slot(bus, devfn);
                        if (!dev) {
                                continue;
                        }
                        if (!dev->enabled) {
                                continue;
                        }
                        printk_info("%02x:%02x.%0x aka %s\n", 
                                bus, devfn >> 3, devfn & 7, dev_path(dev));
                        for(i = 0; i < 256; i++) {
                                if ((i & 0x0f) == 0) {
                                        printk_info("%02x:", i);
                                }
                                printk_info(" %02x", pci_read_config8(dev, i));
                                if ((i & 0x0f) == 0xf) {
                                        printk_info("\n");
                                }
                        }
                        printk_info("\n");
                }
        }
#endif
#if 0
        msr_t msr;
        unsigned index;
        unsigned eax, ebx, ecx, edx;
        index = 0x80000007;
        printk_debug("calling cpuid 0x%08x\n", index);
        asm volatile(
                "cpuid"
                : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
                : "a" (index)
                );
        printk_debug("cpuid[%08x]: %08x %08x %08x %08x\n",
                index, eax, ebx, ecx, edx);
        if (edx & (3 << 1)) {
                index = 0xC0010042;
                printk_debug("Reading msr: 0x%08x\n", index);
                msr = rdmsr(index);
                printk_debug("msr[0x%08x]: 0x%08x%08x\n",
                        index, msr.hi, msr.hi);
        }
#endif
}

static void debug_noop(device_t dummy)
{
}

static struct device_operations debug_operations = {
        .read_resources   = debug_noop,
        .set_resources    = debug_noop,
        .enable_resources = debug_noop,
        .init             = debug_init,
};

static unsigned int scan_root_bus(device_t root, unsigned int max)
{
        struct device_path path;
        device_t debug;
        max = root_dev_scan_bus(root, max);
        path.type = DEVICE_PATH_PNP;
        path.u.pnp.port   = 0;
        path.u.pnp.device = 0;
        debug = alloc_dev(&root->link[1], &path);
        debug->ops = &debug_operations;
        return max;
}
#endif

static void mainboard_init(device_t dev)
{       
        root_dev_init(dev);
        
//        do_verify_cpu_voltages();
}

static struct device_operations mainboard_operations = {
	.read_resources   = root_dev_read_resources,
	.set_resources    = root_dev_set_resources,
	.enable_resources = root_dev_enable_resources,
	.init             = mainboard_init,
#if !DEBUG
	.scan_bus         = root_dev_scan_bus,
#else
	.scan_bus         = scan_root_bus,
#endif
	.enable           = 0,
};

static void enable_dev(struct device *dev)
{
	dev_root.ops = &mainboard_operations;
}
struct chip_operations mainboard_tyan_s2885_ops = {
	.name      = "Tyan s2885 mainboard ",
	.enable_dev = enable_dev, 
};
