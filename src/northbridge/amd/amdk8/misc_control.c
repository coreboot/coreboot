/* Turn off machine check triggers when reading
 * pci space where there are no devices.
 * This is necessary when scaning the bus for
 * devices which is done by the kernel
 *
 * written in 2003 by Eric Biederman
 * 
 *  - Athlon64 workarounds by Stefan Reinauer
 *  - "reset once" logic by Yinghai Lu
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <part/hard_reset.h>
#include "./cpu_rev.c"
#include "amdk8.h"

static void mcf3_read_resources(device_t dev)
{
	struct resource *resource;

	/* Read the generic PCI resources */
	pci_dev_read_resources(dev);

	/* If we are not the first processor don't allocate the gart apeture */
	if (dev->path.u.pci.devfn != PCI_DEVFN(0x18, 0x3)) {
		return;
	}
		
	/* Add a 64M Gart apeture resource */
	if (dev->resources < MAX_RESOURCES) {
		resource = &dev->resource[dev->resources];
		dev->resources++;
		resource->base  = 0;
		resource->size  = AGP_APERTURE_SIZE;
		resource->align = log2(resource->size);
		resource->gran  = log2(resource->size);
		resource->limit = 0xffffffff; /* 4G */
		resource->flags = IORESOURCE_MEM;
		resource->index = 0x94;
	} else {
		printk_err("%s Unexpeted resource shortage\n", dev_path(dev));
	}
}

static void mcf3_set_resources(device_t dev)
{
	struct resource *resource, *last;
	last = &dev->resource[dev->resources];
	for (resource = &dev->resource[0]; resource < last; resource++) {
		if (resource->index == 0x94) {
			device_t pdev;
			uint32_t base;
			uint32_t size;
			
			size = (0<<6)|(0<<5)|(0<<4)|
			    ((log2(resource->size) - 25) << 1)|(0<<0);
			base = ((resource->base) >> 25) & 0x00007fff;
			pdev = 0;
			while (pdev = dev_find_device(PCI_VENDOR_ID_AMD, 0x1103, pdev)) {
				/* I want a 64M GART apeture */
				pci_write_config32(pdev, 0x90, (0<<6)|(0<<5)|(0<<4)|(1<<1)|(0<<0));
				/* Store the GART base address */
				pci_write_config32(pdev, 0x94, base);
				/* Don't set the GART Table base address */
				pci_write_config32(pdev, 0x98, 0);

				printk_debug("%s %02x <- [0x%08lx - 0x%08lx] mem <gart>\n",
					     dev_path(pdev), resource->index, resource->base,
					     resource->base + resource->size - 1);
			}
			/* Remember this resource has been stored */
			resource->flags |= IORESOURCE_STORED;
		}
	}
	/* Set the generic PCI resources */
	pci_dev_set_resources(dev);
}

static void misc_control_init(struct device *dev)
{
	uint32_t cmd, cmd_ref;
	int needs_reset;
	struct device *f0_dev, *f2_dev;
	
	printk_debug("NB: Function 3 Misc Control.. ");
	needs_reset = 0;

	/* Disable Machine checks from Invalid Locations.
	 * This is needed for PC backwards compatibility.
	 */
	cmd = pci_read_config32(dev, 0x44);
	cmd |= (1<<6) | (1<<25);
	pci_write_config32(dev, 0x44, cmd );
	if (is_cpu_pre_c0()) {

		/* Errata 58
		 * Disable CPU low power states C2, C1 and throttling 
		 */
		cmd = pci_read_config32(dev, 0x80);
		cmd &= ~(1<<0);
		pci_write_config32(dev, 0x80, cmd );
		cmd = pci_read_config32(dev, 0x84);
		cmd &= ~(1<<24);
		cmd &= ~(1<<8);
		pci_write_config32(dev, 0x84, cmd );

		/* Errata 66
		 * Limit the number of downstream posted requests to 1 
		 */
		cmd = pci_read_config32(dev, 0x70);
		if ((cmd & (3 << 0)) != 2) {
			cmd &= ~(3<<0);
			cmd |= (2<<0);
			pci_write_config32(dev, 0x70, cmd );
			needs_reset = 1;
		}
		cmd = pci_read_config32(dev, 0x7c);
		if ((cmd & (3 << 4)) != 0) {
			cmd &= ~(3<<4);
			cmd |= (0<<4);
			pci_write_config32(dev, 0x7c, cmd );
			needs_reset = 1;
		}
		/* Clock Power/Timing Low */
		cmd = pci_read_config32(dev, 0xd4);
		if (cmd != 0x000D0001) {
			cmd = 0x000D0001;
			pci_write_config32(dev, 0xd4, cmd);
			needs_reset = 1; /* Needed? */
		}
	}
	else {
		uint32_t dcl;
		f2_dev = dev_find_slot(0, dev->path.u.pci.devfn - 3 + 2);
		/* Errata 98 
		 * Set Clk Ramp Hystersis to 7
		 * Clock Power/Timing Low
		 */
		cmd_ref = 0x04e20707; /* Registered */
		dcl = pci_read_config32(f2_dev, DRAM_CONFIG_LOW);
		if (dcl & DCL_UnBufDimm) {
			cmd_ref = 0x000D0701; /* Unbuffered */
		}
		cmd = pci_read_config32(dev, 0xd4);
		if(cmd != cmd_ref) {
			pci_write_config32(dev, 0xd4, cmd_ref );
			needs_reset = 1; /* Needed? */
		}
	}
#if CONFIG_MAX_CPUS > 1 
/* Single CPU systems don't seem to need this. It might cause resets? (YhLu) */
	/* Optimize the Link read pointers */
	f0_dev = dev_find_slot(0, dev->path.u.pci.devfn - 3);
	if (f0_dev) {
		int link;
		cmd_ref = cmd = pci_read_config32(dev, 0xdc);
		for(link = 0; link < 3; link++) {
			uint32_t link_type;
			unsigned reg;
			/* This works on an Athlon64 because unimplemented links return 0 */
			reg = 0x98 + (link * 0x20);
			link_type = pci_read_config32(f0_dev, reg);
			if (link_type & LinkConnected) {
				cmd &= 0xff << (link *8);
				/* FIXME this assumes the device on the other side is an AMD device */
				cmd |= 0x25 << (link *8);
			}
		}
		if (cmd != cmd_ref) {
			pci_write_config32(dev, 0xdc, cmd);
			needs_reset = 1;
		}
	}
	else {
		printk_err("Missing f0 device!\n");
	}
#endif 
	if (needs_reset) {
		printk_debug("resetting cpu\n");
		hard_reset();
	}
	printk_debug("done.\n");
}


static struct device_operations mcf3_ops  = {
	.read_resources   = mcf3_read_resources,
	.set_resources    = mcf3_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = misc_control_init,
	.scan_bus         = 0,
};

static struct pci_driver mcf3_driver __pci_driver = {
	.ops    = &mcf3_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1103,
};
