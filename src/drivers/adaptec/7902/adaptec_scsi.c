/* Copyright 2003 Tyan */

/* Author: Yinghai Lu 
 *
 */


#include <delay.h>
#include <stdlib.h>
#include <string.h>
#include <arch/io.h>

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>


static void adaptec_scsi_init(struct device *dev)
{
	uint16_t word;

        word = pci_read_config16(dev, 0x4);
        word |= ((1 << 2) |(1<<4)); // Command: 3--> 17
        pci_write_config16(dev, 0x4, word);

	printk_debug("ADAPTEC_SCSI_FIXUP:  done  \n");
	
	
}

static struct device_operations adaptec_scsi_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = adaptec_scsi_init,
	.scan_bus         = 0,
};

static struct pci_driver adaptec2_scsi_driver __pci_driver = {
	.ops    = &adaptec_scsi_ops,
	.vendor = PCI_VENDOR_ID_ADAPTEC2,
	.device = PCI_DEVICE_ID_ADAPTEC2_7902,//0x8012
};

static struct pci_driver adaptec3_scsi_driver __pci_driver = {
        .ops    = &adaptec_scsi_ops,
        .vendor = PCI_VENDOR_ID_ADAPTEC2,
        .device = 0x801d
};

static struct pci_driver adaptec_scsi_driver __pci_driver = {
        .ops    = &adaptec_scsi_ops,
        .vendor = PCI_VENDOR_ID_ADAPTEC,
        .device = PCI_DEVICE_ID_ADAPTEC_7895,
};
 
