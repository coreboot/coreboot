/*
 *  Copyright 2003 Tyan
 *
 *	Author: Yinghai Lu
 */
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

static void agp3bridge_init(device_t dev)
{
	uint8_t byte;

	/* Enable BM and IO */
	byte = pci_read_config32(dev, 0x04);
        byte |= 0x07;
        pci_write_config8(dev, 0x04, byte);

	/* Eable VGA/ISA decoding */
        byte = pci_read_config32(dev, 0x3e);
        byte |= 3<<2;
        pci_write_config8(dev, 0x3e, byte);

	return;
}

static struct device_operations agp3bridge_ops  = {
        .read_resources   = pci_bus_read_resources,
        .set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
        .init             = agp3bridge_init,
        .scan_bus         = pci_scan_bridge,
};

static struct pci_driver agp3bridge_driver __pci_driver = {
        .ops    = &agp3bridge_ops,
        .vendor = PCI_VENDOR_ID_AMD,
        .device = 0x7455, // AGP Bridge
};

static void agp3dev_enable(device_t dev)
{
	uint32_t value;
	
	//  AGP enable
        value = pci_read_config32(dev, 0xa8);
        value |= (3<<8)|2; //AGP 8x
        pci_write_config32(dev, 0xa8, value);
/*
	//  linkA 8bit-->16bit
        value = pci_read_config32(dev, 0xc4);
        value |= (0x11<<24);
        pci_write_config32(dev, 0xc4, value);

        //  linkA 200-->600
        value = pci_read_config32(dev, 0xcc);
        value |= (4<<8);
        pci_write_config32(dev, 0xcc, value);
*/  

        value = pci_read_config32(dev, 0x4);
        value |= 6;
        pci_write_config32(dev, 0x4, value);
}

static struct device_operations agp3dev_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init     = 0,
	.scan_bus = 0,
	.enable   = agp3dev_enable,
};

static struct pci_driver agp3dev_driver __pci_driver = {
	.ops    = &agp3dev_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x7454, //AGP Device
	
};
