/* Copyright 2004 Linux Networx  */
/* This code is distrubted wihtout warrant under the GPL v2 (see COPYING) */

#include <console/console.h>
#include <stdlib.h>
#include <stdint.h>
#include <bitops.h>
#include <string.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>


/* PNP fundamental operations */

void pnp_write_config(device_t dev, uint8_t reg, uint8_t value)
{
	outb(reg, dev->path.u.pnp.port);
	outb(value, dev->path.u.pnp.port + 1);
}

uint8_t pnp_read_config(device_t dev, uint8_t reg)
{
	outb(reg, dev->path.u.pnp.port);
	return inb(dev->path.u.pnp.port + 1);
}

void pnp_set_logical_device(device_t dev)
{
	pnp_write_config(dev, 0x07, dev->path.u.pnp.device);
}

void pnp_set_enable(device_t dev, int enable)
{
	pnp_write_config(dev, 0x30, enable?0x1:0x0);
}

int pnp_read_enable(device_t dev)
{
	return !!pnp_read_config(dev, 0x30);
}

void pnp_set_iobase(device_t dev, unsigned index, unsigned iobase)
{
	/* Index == 0x60 or 0x62 */
	pnp_write_config(dev, index + 0, (iobase >> 8) & 0xff);
	pnp_write_config(dev, index + 1, iobase & 0xff);
}

void pnp_set_irq(device_t dev, unsigned index, unsigned irq)
{
	/* Index == 0x70 or 0x72 */
	pnp_write_config(dev, index, irq);
}


void pnp_set_drq(device_t dev, unsigned drq, unsigned index)
{
	/* Index == 0x74 */
	pnp_write_config(dev, index, drq & 0xff);
}

/* PNP device operations */

void pnp_read_resources(device_t dev)
{
	return;
}

static void pnp_set_resource(device_t dev, struct resource *resource)
{
	if (!(resource->flags & IORESOURCE_ASSIGNED)) {
#if 1
		printk_err("ERROR: %s %02x not allocated\n",
			dev_path(dev), resource->index);
#endif
		return;
	}
	/* Now store the resource */
	resource->flags |= IORESOURCE_STORED;
	if (resource->flags & IORESOURCE_IO) {
		pnp_set_iobase(dev, resource->index, resource->base);
	}
	else if (resource->flags & IORESOURCE_DRQ) {
		pnp_set_drq(dev, resource->index, resource->base);
	}
	else if (resource->flags  & IORESOURCE_IRQ) {
		pnp_set_irq(dev, resource->index, resource->base);
	}
	else {
		/* Don't let me think I stored the resource */
		resource->flags &= IORESOURCE_STORED;
		printk_err("ERROR: %s %02x unknown resource type\n",
			dev_path(dev), resource->index);
		return;
	}

	printk_debug(
		"%s %02x <- [0x%08lx - 0x%08lx] %s\n",
		dev_path(dev),
		resource->index,
		resource->base,  resource->base + resource->size - 1,
		(resource->flags & IORESOURCE_IO)? "io":
		(resource->flags & IORESOURCE_DRQ)? "drq":
		(resource->flags & IORESOURCE_IRQ)? "irq":
		(resource->flags & IORESOURCE_MEM)? "mem":
		"???");
}

void pnp_set_resources(device_t dev)
{
	int i;

	/* Select the device */
	pnp_set_logical_device(dev);

	/* Paranoia says I should disable the device here... */
	for(i = 0; i < dev->resources; i++) {
		pnp_set_resource(dev, &dev->resource[i]);
	}

}

void pnp_enable_resources(device_t dev)
{
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 1);

}

void pnp_enable(device_t dev)
{

        if (!dev->enabled) {
		pnp_set_logical_device(dev);
		pnp_set_enable(dev, 0);
	}
}

struct device_operations pnp_ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
};

/* PNP chip opertations */

static void pnp_get_ioresource(device_t dev, unsigned index, struct io_info *info)
{
	struct resource *resource;
	uint32_t size;
	resource = get_resource(dev, index);
	
	/* Initilize the resource */
	resource->limit = 0xffff;
	resource->flags |= IORESOURCE_IO;
	
	/* Set the resource size and alignment */
	size = (0xffff & info->mask);
	resource->size  = (~(size | 0xfffff800) + 1);
	resource->align = log2(resource->size);
	resource->gran  = resource->align;
}

static void get_resources(device_t dev, struct pnp_info *info)
{
	struct resource *resource;

//	pnp_set_logical_device(dev);   // coment out by LYH

	if (info->flags & PNP_IO0) {
		pnp_get_ioresource(dev, PNP_IDX_IO0, &info->io0);
	}
	if (info->flags & PNP_IO1) {
		pnp_get_ioresource(dev, PNP_IDX_IO1, &info->io1);
	}
	if (info->flags & PNP_IRQ0) {
		resource = get_resource(dev, PNP_IDX_IRQ0);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_IRQ1) {
		resource = get_resource(dev, PNP_IDX_IRQ1);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_DRQ0) {
		resource = get_resource(dev, PNP_IDX_DRQ0);
		resource->size = 1;
		resource->flags |= IORESOURCE_DRQ;
	}
	if (info->flags & PNP_DRQ1) {
		resource = get_resource(dev, PNP_IDX_DRQ1);
		resource->size = 1;
		resource->flags |= IORESOURCE_DRQ;
	}
	
} 

void pnp_enumerate(struct chip *chip, unsigned functions, 
	struct device_operations *ops, struct pnp_info *info)
{
	struct device_path path;
	device_t dev;
	int i;

	chip_enumerate(chip);
	path.type       = DEVICE_PATH_PNP;
	path.u.pnp.port = chip->dev->path.u.pnp.port;

	
	/* Setup the ops and resources on the newly allocated devices */
	for(i = 0; i < functions; i++) {
		path.u.pnp.device = info[i].function;

		dev = alloc_find_dev(chip->bus, &path);

		if(info[i].ops == 0) {  // BY LYH
		  dev->ops = ops;
		} 
		else { 
		  dev->ops = info[i].ops;  // BY LYH
		}
		get_resources(dev, &info[i]);

	}
}
