/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#include <arch/io.h>
#include <device/device.h>
#include <device/chip.h>
#include <console/console.h>
#include <string.h>
#include <bitops.h>
#include "chip.h"

void pnp_output(char address, char data)
{
	outb(address, PNP_INDEX_REG);
	outb(data, PNP_DATA_REG);
}

static void sio_enable(struct chip *chip, enum chip_pass pass)
{

	struct superio_NSC_pc87360_config *conf = (struct superio_NSC_pc87360_config *)chip->chip_info;

	switch (pass) {
	case CONF_PASS_PRE_CONSOLE:
		/* Enable Super IO Chip */
		pnp_output(0x07, 6); /* LD 6 = UART1 */
		pnp_output(0x30, 0); /* Dectivate */
		pnp_output(0x60, conf->port >> 8); /* IO Base */
		pnp_output(0x61, conf->port & 0xFF); /* IO Base */
		pnp_output(0x30, 1); /* Activate */
		break;
	default:
		/* nothing yet */
		break;
	}
}

static void pnp_write_config(device_t dev, unsigned char value, unsigned char reg)
{
	outb(reg, dev->path.u.pnp.port);
	outb(value, dev->path.u.pnp.port + 1);
}

static unsigned char pnp_read_config(device_t dev, unsigned char reg)
{
	outb(reg, dev->path.u.pnp.port);
	return inb(dev->path.u.pnp.port + 1);
}

static void pnp_set_logical_device(device_t dev)
{
	pnp_write_config(dev, dev->path.u.pnp.device, 0x07);
}

static void pnp_set_enable(device_t dev, int enable)
{
	pnp_write_config(dev, enable?0x1:0x0, 0x30);
}

static int pnp_read_enable(device_t dev)
{
	return !!pnp_read_config(dev, 0x30);
}

#define FLOPPY_DEVICE   0
#define PARALLEL_DEVICE 1
#define COM2_DEVICE     2
#define COM1_DEVICE     3
#define SWC_DEVICE      4
#define MOUSE_DEVICE    5
#define KBC_DEVICE      6
#define GPIO_DEVICE     7
#define ACB_DEVICE      8
#define FSCM_DEVICE     9
#define WDT_DEVICE     10

struct io_info {
	unsigned mask, set;
};
struct pnp_info {
	unsigned flags;
#define PNP_IO0  0x01
#define PNP_IO1  0x02
#define PNP_IRQ0 0x04
#define PNP_IRQ1 0x08
#define PNP_DRQ0 0x10
#define PNP_DRQ1 0x20
	struct io_info io0, io1;
};

static struct pnp_info pnp_dev_info[] = {
	[ 0] = { PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x07fa, 0}, },
	[ 1] = { PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x04f8, 0}, },
	[ 2] = { PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_DRQ1, { 0x7f8, 0 }, },
	[ 3] = { PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
	[ 4] = { PNP_IO0 | PNP_IRQ0, { 0xfff0, 0 }, },
	[ 5] = { PNP_IRQ0 },
	[ 6] = { PNP_IO0 | PNP_IO1 | PNP_IRQ0, { 0x7f8, 0 }, { 0x7f8, 0x4}, },
	[ 7] = { PNP_IO0 | PNP_IRQ0, { 0xfff8, 0 } },
	[ 8] = { PNP_IO0 | PNP_IRQ0, { 0xfff8, 0 } },
	[ 9] = { PNP_IO0 | PNP_IRQ0, { 0xfff8, 0 } },
	[10] = { PNP_IO0 | PNP_IRQ0, { 0xfffc, 0 } },
};

static struct resource *get_resource(device_t dev, unsigned index)
{
	struct resource *resource;
	int i;
	resource = 0;
	for(i = 0; i < dev->resources; i++) {
		resource = &dev->resource[i];
		if (resource->index == index) {
			break;
		}
	}
	if (!resource || (resource->index != index)) {
		resource = &dev->resource[dev->resources];
		memset(resource, 0, sizeof(*resource));
		dev->resources++;
	}
	/* Initialize the resource values */
	if (!(resource->flags & IORESOURCE_FIXED)) {
		resource->flags = 0;
		resource->base = 0;
	}
	resource->size  = 0;
	resource->limit = 0;
	resource->flags = 0;
	resource->index = index;
	resource->align = 0;
	resource->gran  = 0;

	return resource;
}

static void pnp_read_ioresource(device_t dev, unsigned index, struct io_info *info)
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


static void pnp_read_resources(device_t dev)
{
	struct pnp_info *info;
	struct resource *resource;
	pnp_set_logical_device(dev);

	info = &pnp_dev_info[dev->path.u.pnp.device];

	if (info->flags & PNP_IO0) {
		pnp_read_ioresource(dev, 0x60, &info->io0);
	}
	if (info->flags & PNP_IO1) {
		pnp_read_ioresource(dev, 0x62, &info->io1);
	}
	if (info->flags & PNP_IRQ0) {
		resource = get_resource(dev, 0x70);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_IRQ1) {
		resource = get_resource(dev, 0x72);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_DRQ0) {
		resource = get_resource(dev, 0x74);
		resource->size = 1;
		resource->flags |= IORESOURCE_DRQ;
	}
	if (info->flags & PNP_DRQ1) {
		resource = get_resource(dev, 0x75);
		resource->size = 1;
		resource->flags |= IORESOURCE_DRQ;
	}
}

static void pnp_set_iobase(device_t dev, unsigned iobase, unsigned index)
{
	/* Index == 0x60 or 0x62 */
	pnp_write_config(dev, (iobase >> 8) & 0xff, index);
	pnp_write_config(dev, iobase & 0xff, index + 1);
}

static void pnp_set_irq(device_t dev, unsigned irq, unsigned index)
{
	/* Index == 0x70 or 0x72 */
	pnp_write_config(dev, irq, index);
}

static void pnp_set_drq(device_t dev, unsigned drq, unsigned index)
{
	/* Index == 0x74 */
	pnp_write_config(dev, drq & 0xff, index);
}


static void pnp_set_resource(device_t dev, struct resource *resource)
{
	if (!(resource->flags & IORESOURCE_SET)) {
#if 1
		printk_err("ERROR: %s %02x not allocated\n",
			dev_path(dev), resource->index);
#endif
		return;
	}
	if (resource->flags & IORESOURCE_IO) {
		pnp_set_iobase(dev, resource->base, resource->index);
	}
	else if (resource->flags & IORESOURCE_DRQ) {
		pnp_set_drq(dev, resource->base, resource->index);
	}
	else if (resource->flags  & IORESOURCE_IRQ) {
		pnp_set_irq(dev, resource->base, resource->index);
	}
	else {
		printk_err("ERROR: %s %02x unknown resource type\n",
			dev_path(dev), resource->index);
		return;
	}
	printk_debug(
		"%s %02x <- [0x%08lx - 0x%08lx %s\n",
		dev_path(dev),
		resource->index,
		resource->base,  resource->base + resource->size - 1,
		(resource->flags & IORESOURCE_IO)? "io":
		(resource->flags & IORESOURCE_DRQ)? "drq":
		(resource->flags & IORESOURCE_IRQ)? "irq":
		(resource->flags & IORESOURCE_MEM)? "mem":
		"???");
}

static void pnp_set_resources(device_t dev)
{
	int i;
	pnp_set_logical_device(dev);
	for(i = 0; i < dev->resources; i++) {
		pnp_set_resource(dev, &dev->resource[i]);
	}

}
static void pnp_enable_resources(device_t dev)
{
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 1);

}
static void pnp_enable(device_t dev)
{
	pnp_set_logical_device(dev);
	if (!dev->enable) {
		pnp_set_enable(dev, 0);
	}
}

static struct device_operations pnp_ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
};

#define MAX_FUNCTION 10
static void enumerate(struct chip *chip)
{
	struct superio_NSC_pc87360_config *conf = (struct superio_NSC_pc87360_config *)chip->chip_info;
	struct resource *resource;
	struct device_path path;
	device_t dev;
	int i;

	chip_enumerate(chip);
	path.type       = DEVICE_PATH_PNP;
	path.u.pnp.port = chip->dev->path.u.pnp.port;

	/* Set the ops on the newly allocated devices */
	for(i = 0; i <= WDT_DEVICE; i++) {
		path.u.pnp.device = i;
		dev = alloc_find_dev(chip->bus, &path);
		dev->ops = &pnp_ops;
	}

	/* Processes the hard codes for com1 */
	path.u.pnp.device = COM1_DEVICE;
	dev = alloc_find_dev(chip->bus, &path);
	resource = get_resource(dev, 0x60);
	if (conf->com1.base) {
		resource->base = conf->com1.base;
		resource->flags = IORESOURCE_IO | IORESOURCE_FIXED | IORESOURCE_SET;
	}
	resource = get_resource(dev, 0x70);
	if (conf->com1.irq) {
		resource->base = conf->com1.irq;
		resource->flags = IORESOURCE_IRQ | IORESOURCE_FIXED | IORESOURCE_SET;
	}

	/* Process the hard codes for the keyboard controller */
	path.u.pnp.device = KBC_DEVICE;
	dev = alloc_find_dev(dev, &path);
	resource = get_resource(dev, 0x60);
	resource->base = 0x60;
	resource->flags = IORESOURCE_IO | IORESOURCE_FIXED | IORESOURCE_SET;
	resource = get_resource(dev, 0x62);
	resource->base = 0x64;
	resource->flags = IORESOURCE_IO | IORESOURCE_FIXED | IORESOURCE_SET;
}

struct chip_control superio_NSC_pc87360_control = {
	.enable    = sio_enable,
	.enumerate = enumerate,
	.name      = "winbond w83627thf";
};
