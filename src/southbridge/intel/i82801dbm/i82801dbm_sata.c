#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "i82801dbm.h"

static void sata_init(struct device *dev)
{

        uint16_t word;
        uint8_t byte;
	int enable_c=1, enable_d=1;
	//	int i;
                
        //Enable Serial ATA port
        byte = pci_read_config8(dev,0x90);
        byte &= 0xf8;
        byte |= ICH5_SATA_ADDRESS_MAP & 7;
        pci_write_config8(dev,0x90,byte);

//        for(i=0;i<10;i++) {
		word = pci_read_config16(dev,0x92);
                word &= 0xfffc;
//                if( (word & 0x0003) == 0x0003) break;
                word |= 0x0003; // enable P0/P1 
                pci_write_config16(dev,0x92,word);
//        }

//        for(i=0;i<10;i++) {
              /* enable ide0 */
		word = pci_read_config16(dev, 0x40);
        	word &= ~(1 << 15);
                if(enable_c==0) {
//                     	if( (word & 0x8000) == 0x0000) break;
                       	word |= 0x0000;
                }
                else {
//                       	if( (word & 0x8000) == 0x8000) break;
                       	word |= 0x8000;
                }
                pci_write_config16(dev, 0x40, word);
//	}
                /* enable ide1 */
//        for(i=0;i<10;i++) {
              	word = pci_read_config16(dev, 0x42);
                word &= ~(1 << 15);
                if(enable_d==0) {
//                       	if( (word & 0x8000) == 0x0000) break;
                       	word |= 0x0000;
                }
                else {
//                       	if( (word & 0x8000) == 0x8000) break;
                       	word |= 0x8000;
                }
                pci_write_config16(dev, 0x42, word);
//        }

}

static struct device_operations sata_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = sata_init,
	.scan_bus         = 0,
	.enable           = i82801dbm_enable,
};

static const struct pci_driver stat_driver __pci_driver = {
	.ops    = &sata_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801DBM_SATA,
};

