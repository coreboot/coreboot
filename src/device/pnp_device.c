/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <stdint.h>

/* PNP config mode wrappers */

void pnp_enter_conf_mode(struct device *dev)
{
	if (dev->ops->ops_pnp_mode)
		dev->ops->ops_pnp_mode->enter_conf_mode(dev);
}

void pnp_exit_conf_mode(struct device *dev)
{
	if (dev->ops->ops_pnp_mode)
		dev->ops->ops_pnp_mode->exit_conf_mode(dev);
}

#if CONFIG(HAVE_ACPI_TABLES)
void pnp_ssdt_enter_conf_mode(struct device *dev, const char *idx, const char *data)
{
	if (dev->ops->ops_pnp_mode && dev->ops->ops_pnp_mode->ssdt_enter_conf_mode)
		dev->ops->ops_pnp_mode->ssdt_enter_conf_mode(dev, idx, data);
}
void pnp_ssdt_exit_conf_mode(struct device *dev, const char *idx, const char *data)
{
	if (dev->ops->ops_pnp_mode && dev->ops->ops_pnp_mode->ssdt_exit_conf_mode)
		dev->ops->ops_pnp_mode->ssdt_exit_conf_mode(dev, idx, data);
}
#endif

/* PNP fundamental operations */

void pnp_write_config(struct device *dev, u8 reg, u8 value)
{
	outb(reg, dev->path.pnp.port);
	outb(value, dev->path.pnp.port + 1);
}

u8 pnp_read_config(struct device *dev, u8 reg)
{
	outb(reg, dev->path.pnp.port);
	return inb(dev->path.pnp.port + 1);
}

void pnp_unset_and_set_config(struct device *dev, u8 reg, u8 unset, u8 set)
{
	outb(reg, dev->path.pnp.port);
	u8 value = inb(dev->path.pnp.port + 1);
	value &= ~unset;
	value |= set;
	outb(value, dev->path.pnp.port + 1);
}

void pnp_set_logical_device(struct device *dev)
{
	pnp_write_config(dev, 0x07, dev->path.pnp.device & 0xff);
}

void pnp_set_enable(struct device *dev, int enable)
{
	u8 tmp, bitpos;

	tmp = pnp_read_config(dev, PNP_IDX_EN);

	/* Handle virtual devices, which share the same LDN register. */
	bitpos = (dev->path.pnp.device >> 8) & 0x7;

	if (enable)
		tmp |= (1 << bitpos);
	else
		tmp &= ~(1 << bitpos);

	pnp_write_config(dev, PNP_IDX_EN, tmp);
}

int pnp_read_enable(struct device *dev)
{
	u8 tmp, bitpos;

	tmp = pnp_read_config(dev, PNP_IDX_EN);

	/* Handle virtual devices, which share the same LDN register. */
	bitpos = (dev->path.pnp.device >> 8) & 0x7;

	return !!(tmp & (1 << bitpos));
}

void pnp_set_iobase(struct device *dev, u8 index, u16 iobase)
{
	/* Index == 0x60 or 0x62. */
	pnp_write_config(dev, index + 0, (iobase >> 8) & 0xff);
	pnp_write_config(dev, index + 1, iobase & 0xff);
}

void pnp_set_irq(struct device *dev, u8 index, u8 irq)
{
	/* Index == 0x70 or 0x72. */
	pnp_write_config(dev, index, irq);
}

void pnp_set_drq(struct device *dev, u8 index, u8 drq)
{
	/* Index == 0x74. */
	pnp_write_config(dev, index, drq & 0xff);
}

/* PNP device operations */

void pnp_read_resources(struct device *dev)
{
	return;
}

static void pnp_set_resource(struct device *dev, struct resource *resource)
{
	if (!(resource->flags & IORESOURCE_ASSIGNED)) {
		/* The PNP_MSC Super IO registers have the IRQ flag set. If no
		   value is assigned in the devicetree, the corresponding
		   PNP_MSC register doesn't get written, which should be printed
		   as warning and not as error. */
		if (resource->flags & IORESOURCE_IRQ &&
		    (resource->index != PNP_IDX_IRQ0) &&
		    (resource->index != PNP_IDX_IRQ1))
			printk(BIOS_WARNING, "%s %02lx %s size: "
			       "0x%010llx not assigned in devicetree\n", dev_path(dev),
			       resource->index, resource_type(resource),
			       resource->size);
		else
			printk(BIOS_ERR, "%s %02lx %s size: 0x%010llx "
			       "not assigned in devicetree\n", dev_path(dev), resource->index,
			       resource_type(resource), resource->size);
		return;
	}

	/* Now store the resource. */
	if (resource->flags & IORESOURCE_IO) {
		pnp_set_iobase(dev, resource->index, resource->base);
	} else if (resource->flags & IORESOURCE_DRQ) {
		pnp_set_drq(dev, resource->index, resource->base);
	} else if (resource->flags & IORESOURCE_IRQ) {
		pnp_set_irq(dev, resource->index, resource->base);
	} else {
		printk(BIOS_ERR, "%s %02lx unknown resource type\n",
		       dev_path(dev), resource->index);
		return;
	}
	resource->flags |= IORESOURCE_STORED;

	report_resource_stored(dev, resource, "");
}

void pnp_set_resources(struct device *dev)
{
	struct resource *res;

	pnp_enter_conf_mode(dev);

	/* Select the logical device (LDN). */
	pnp_set_logical_device(dev);

	/* Paranoia says I should disable the device here... */
	for (res = dev->resource_list; res; res = res->next)
		pnp_set_resource(dev, res);

	pnp_exit_conf_mode(dev);
}

void pnp_enable_resources(struct device *dev)
{
	pnp_enter_conf_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 1);
	pnp_exit_conf_mode(dev);
}

void pnp_enable(struct device *dev)
{
	if (!dev->enabled) {
		pnp_enter_conf_mode(dev);
		pnp_set_logical_device(dev);
		pnp_set_enable(dev, 0);
		pnp_exit_conf_mode(dev);
	}
}

void pnp_alt_enable(struct device *dev)
{
	pnp_enter_conf_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, !!dev->enabled);
	pnp_exit_conf_mode(dev);
}

struct device_operations pnp_ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
};

/* PNP chip operations */

static void pnp_get_ioresource(struct device *dev, u8 index, u16 mask)
{
	struct resource *resource;
	unsigned int bit;

	/* If none of the mask bits is set, the resource would occupy the whole
	   IO space leading to IO resource conflicts with the other devices */
	if (!mask) {
		printk(BIOS_ERR, "device %s index %d has no mask.\n",
				dev_path(dev), index);
		return;
	}

	resource = new_resource(dev, index);
	resource->flags |= IORESOURCE_IO;

	/* Calculate IO region size which is determined by the first one from
	   the LSB of the mask. */
	for (bit = 0; bit <= 15 && (mask & (1 << bit)) == 0; ++bit)
		;

	resource->gran  = bit;
	resource->align = bit;
	resource->size  = 1 << bit;

	/* Calculate IO region address limit which is determined by the first
	   one from the MSB of the mask. */
	for (bit = 15; bit != 0 && (mask & (1 << bit)) == 0; --bit)
		;

	resource->limit = (1 << (bit + 1)) - 1;

	/* The block of ones in the mask is expected to be continuous.
	   If there is any zero in between the block of ones, it is ignored
	   in the calculation of the resource size and limit. */
	if (mask != (resource->limit ^ (resource->size - 1)))
		printk(BIOS_WARNING, "mask of device %s index %d is wrong.\n",
			dev_path(dev), index);
}

static void get_resources(struct device *dev, struct pnp_info *info)
{
	struct resource *resource;

	if (info->flags & PNP_IO0)
		pnp_get_ioresource(dev, PNP_IDX_IO0, info->io0);
	if (info->flags & PNP_IO1)
		pnp_get_ioresource(dev, PNP_IDX_IO1, info->io1);
	if (info->flags & PNP_IO2)
		pnp_get_ioresource(dev, PNP_IDX_IO2, info->io2);
	if (info->flags & PNP_IO3)
		pnp_get_ioresource(dev, PNP_IDX_IO3, info->io3);
	if (info->flags & PNP_IO4)
		pnp_get_ioresource(dev, PNP_IDX_IO4, info->io4);

	if (info->flags & PNP_IRQ0) {
		resource = new_resource(dev, PNP_IDX_IRQ0);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_IRQ1) {
		resource = new_resource(dev, PNP_IDX_IRQ1);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}

	if (info->flags & PNP_DRQ0) {
		resource = new_resource(dev, PNP_IDX_DRQ0);
		resource->size = 1;
		resource->flags |= IORESOURCE_DRQ;
	}
	if (info->flags & PNP_DRQ1) {
		resource = new_resource(dev, PNP_IDX_DRQ1);
		resource->size = 1;
		resource->flags |= IORESOURCE_DRQ;
	}

	/*
	 * These are not IRQs, but set the flag to have the
	 * resource allocator do the right thing.
	 */
	if (info->flags & PNP_EN) {
		resource = new_resource(dev, PNP_IDX_EN);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_MSC0) {
		resource = new_resource(dev, PNP_IDX_MSC0);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_MSC1) {
		resource = new_resource(dev, PNP_IDX_MSC1);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_MSC2) {
		resource = new_resource(dev, PNP_IDX_MSC2);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_MSC3) {
		resource = new_resource(dev, PNP_IDX_MSC3);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_MSC4) {
		resource = new_resource(dev, PNP_IDX_MSC4);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_MSC5) {
		resource = new_resource(dev, PNP_IDX_MSC5);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_MSC6) {
		resource = new_resource(dev, PNP_IDX_MSC6);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_MSC7) {
		resource = new_resource(dev, PNP_IDX_MSC7);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_MSC8) {
		resource = new_resource(dev, PNP_IDX_MSC8);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_MSC9) {
		resource = new_resource(dev, PNP_IDX_MSC9);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_MSCA) {
		resource = new_resource(dev, PNP_IDX_MSCA);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_MSCB) {
		resource = new_resource(dev, PNP_IDX_MSCB);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_MSCC) {
		resource = new_resource(dev, PNP_IDX_MSCC);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_MSCD) {
		resource = new_resource(dev, PNP_IDX_MSCD);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_MSCE) {
		resource = new_resource(dev, PNP_IDX_MSCE);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
}

void pnp_enable_devices(struct device *base_dev, struct device_operations *ops,
			unsigned int functions, struct pnp_info *info)
{
	struct device_path path;
	struct device *dev;
	int i;

	path.type = DEVICE_PATH_PNP;
	path.pnp.port = base_dev->path.pnp.port;

	/* Setup the ops and resources on the newly allocated devices. */
	for (i = 0; i < functions; i++) {
		/* Skip logical devices this Super I/O doesn't have. */
		if (info[i].function == PNP_SKIP_FUNCTION)
			continue;

		path.pnp.device = info[i].function;
		dev = alloc_find_dev(base_dev->bus, &path);

		/* Don't initialize a device multiple times. */
		if (dev->ops)
			continue;

		/* use LDN-specific ops override from corresponding pnp_info
		   entry if not NULL */
		if (info[i].ops)
			dev->ops = info[i].ops;
		/* else use device ops  */
		else
			dev->ops = ops;

		get_resources(dev, &info[i]);
	}
}
