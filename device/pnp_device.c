/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2004 Li-Ta Lo <ollie@lanl.gov>
 * Copyright (C) 2005 Tyan
 * (Written by Yinghai Lu <yhlu@tyan.com> for Tyan)
 * Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <console.h>
#include <string.h>
#include <io.h>
#include <device/device.h>
#include <device/pnp.h>

#define ISA_PNP_ADDR		0x279

/* PNP fundamental operations: */

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

void pnp_set_logical_device(struct device *dev)
{
	pnp_write_config(dev, 0x07, dev->path.pnp.device);
}

void pnp_set_enable(struct device *dev, int enable)
{
	pnp_write_config(dev, 0x30, enable ? 0x1 : 0x0);
}

int pnp_read_enable(struct device *dev)
{
	return !!pnp_read_config(dev, 0x30);
}

void pnp_set_iobase(struct device *dev, unsigned int index, unsigned int iobase)
{
	/* Index == 0x60 or 0x62 */
	pnp_write_config(dev, index + 0, (iobase >> 8) & 0xff);
	pnp_write_config(dev, index + 1, iobase & 0xff);
}

void pnp_set_irq(struct device *dev, unsigned int index, unsigned int irq)
{
	/* Index == 0x70 or 0x72 */
	pnp_write_config(dev, index, irq);
}

void pnp_set_drq(struct device *dev, unsigned int index, unsigned int drq)
{
	/* Index == 0x74 */
	pnp_write_config(dev, index, drq & 0xff);
}

/* PNP device operations: */

void pnp_read_resources(struct device *dev)
{
	return;
}

static void pnp_set_resource(struct device *dev, struct resource *resource)
{
	if (!(resource->flags & IORESOURCE_ASSIGNED)) {
		printk(BIOS_ERR,
		       "ERROR: %s %02lx %s size: 0x%010llx not assigned\n",
		       dev_path(dev), resource->index, resource_type(resource),
		       resource->size);
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
		printk(BIOS_ERR, "ERROR: %s %02lx unknown resource type\n",
		       dev_path(dev), resource->index);
		return;
	}
	resource->flags |= IORESOURCE_STORED;

	report_resource_stored(dev, resource, "");
}

void pnp_set_resources(struct device *dev)
{
	int i;

	/* Select the device. */
	pnp_set_logical_device(dev);

	/* Paranoia says I should disable the device here... */
	for (i = 0; i < dev->resources; i++) {
		pnp_set_resource(dev, &dev->resource[i]);
	}
}

void pnp_enable_resources(struct device *dev)
{
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 1);
}

void pnp_enable(struct device *dev)
{
	if (!dev->enabled) {
		pnp_set_logical_device(dev);
		pnp_set_enable(dev, 0);
	}
}

struct device_operations pnp_ops = {
	.phase4_read_resources   = pnp_read_resources,
	.phase4_set_resources    = pnp_set_resources,
	.phase5_enable_resources = pnp_enable_resources,
	// .enable               = pnp_enable,
	// FIXME
};

/* PNP chip operations: */

static void pnp_get_ioresource(struct device *dev, unsigned int index,
			       struct io_info *info)
{
	struct resource *resource;
	unsigned int moving, gran, step;

	resource = new_resource(dev, index);

	/* Initialize the resource. */
	resource->flags |= IORESOURCE_IO;

	/* Get the resource size. */
	moving = info->mask;
	gran = 15;
	step = 1 << gran;

	/* Find the first bit that moves. */
	while ((moving & step) == 0) {
		gran--;
		step >>= 1;
	}

	/* Now find the first bit that does not move. */
	while ((moving & step) != 0) {
		gran--;
		step >>= 1;
	}

	/* Of the moving bits the last bit in the first group,
	 * tells us the size of this resource.
	 */
	if ((moving & step) == 0) {
		gran++;
		step <<= 1;
	}

	/* Set the resource size and alignment. */
	resource->gran = gran;
	resource->align = gran;
	resource->limit = info->mask | (step - 1);
	resource->size = 1 << gran;
	resource->base = info->val;
	resource->flags |= IORESOURCE_FIXED || IORESOURCE_ASSIGNED;
}

static void get_resources(struct device *dev, struct pnp_info *info)
{
	struct resource *resource;

	if (info->flags & PNP_IO0) {
		pnp_get_ioresource(dev, PNP_IDX_IO0, &info->io0);
	}
	if (info->flags & PNP_IO1) {
		pnp_get_ioresource(dev, PNP_IDX_IO1, &info->io1);
	}
	if (info->flags & PNP_IO2) {
		pnp_get_ioresource(dev, PNP_IDX_IO2, &info->io2);
	}
	if (info->flags & PNP_IO3) {
		pnp_get_ioresource(dev, PNP_IDX_IO3, &info->io3);
	}
	if (info->flags & PNP_IRQ0) {
		resource = new_resource(dev, PNP_IDX_IRQ0);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
		resource->base = info->irq0.val;
		resource->flags |= IORESOURCE_FIXED || IORESOURCE_ASSIGNED;
	}
	if (info->flags & PNP_IRQ1) {
		resource = new_resource(dev, PNP_IDX_IRQ1);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
		resource->base = info->irq1.val;
		resource->flags |= IORESOURCE_FIXED || IORESOURCE_ASSIGNED;
	}
	if (info->flags & PNP_DRQ0) {
		resource = new_resource(dev, PNP_IDX_DRQ0);
		resource->size = 1;
		resource->flags |= IORESOURCE_DRQ;
		resource->base = info->drq0.val;
		resource->flags |= IORESOURCE_FIXED || IORESOURCE_ASSIGNED;
	}
	if (info->flags & PNP_DRQ1) {
		resource = new_resource(dev, PNP_IDX_DRQ1);
		resource->size = 1;
		resource->flags |= IORESOURCE_DRQ;
		resource->base = info->drq0.val;
		resource->flags |= IORESOURCE_FIXED || IORESOURCE_ASSIGNED;
	}
}

void pnp_enable_devices(struct device *base_dev, struct device_operations *ops,
			unsigned int functions, struct pnp_info *info)
{
	struct device_path path;
	struct device_id id = {.type = DEVICE_ID_PNP };
	struct device *dev;
	int i;

	path.type = DEVICE_PATH_PNP;
	path.pnp.port = base_dev->path.pnp.port;

	/* Setup the ops and resources on the newly allocated devices. */
	for (i = 0; i < functions; i++) {
		path.pnp.device = info[i].function;

		dev = find_dev_path(&base_dev->link[0], &path);

		if (dev) {
			printk(BIOS_DEBUG,"%s: %s is already at %s.\n",
			       __func__, dev->dtsname, dev_path(dev));
			continue;
		}
		
		dev = alloc_dev(&base_dev->link[0], &path, &id);

		if (!dev)
			printk(BIOS_DEBUG,"%s: Couldn't allocate child %d.\n",
			       __func__, i);

		dev->enabled = info[i].enable;

		if (info[i].ops == 0) {
			dev->ops = ops;
		} else {
			dev->ops = info[i].ops;
		}
		get_resources(dev, &info[i]);
	}
}

/*
 * PNP functions used to enter/exit PNP config mode:
 */

/* Works for: IT8661F/IT8770F */
static const u8 initkey_it8661f[][4] = {
	{0x86, 0x61, 0x55, 0x55},       /* 0x3f0 */
	{0x86, 0x61, 0x55, 0xaa},       /* 0x3bd */
	{0x86, 0x61, 0xaa, 0x55},       /* 0x370 */
};

/* Works for: IT8671F/IT8687R, IT8673F */
static const u8 initkey_it8671f[][4] = {
	{0x86, 0x80, 0x55, 0x55},       /* 0x3f0 */
	{0x86, 0x80, 0x55, 0xaa},       /* 0x3bd */
	{0x86, 0x80, 0xaa, 0x55},       /* 0x370 */
};

/* Works for: IT8661F/IT8770F, IT8671F/IT8687R, IT8673F. */
static const u8 initkey_mbpnp[] = {
	0x6a, 0xb5, 0xda, 0xed, 0xf6, 0xfb, 0x7d, 0xbe, 0xdf, 0x6f, 0x37,
	0x1b, 0x0d, 0x86, 0xc3, 0x61, 0xb0, 0x58, 0x2c, 0x16, 0x8b, 0x45,
	0xa2, 0xd1, 0xe8, 0x74, 0x3a, 0x9d, 0xce, 0xe7, 0x73, 0x39,
};

static void pnp_enter_ite_legacy(struct device *dev, const u8 init[][4])
{
	int i, idx;
	u16 port = dev->path.pnp.port;
	
	/* Determine Super I/O config port. */
	idx = (port == 0x3f0) ? 0 : ((port == 0x3bd) ? 1 : 2);
	for (i = 0; i < 4; i++)
		outb(init[idx][i], ISA_PNP_ADDR);
	
	/* Sequentially write the 32 MB PnP init values. */
	for (i = 0; i < 32; i++)
		outb(initkey_mbpnp[i], port);
}

/* Works for: IT8661F/IT8770F */
void pnp_enter_ite_it8661f(struct device *dev)
{
	pnp_enter_ite_legacy(dev, initkey_it8661f);
}

/* Works for: IT8671F/IT8687R, IT8673F */
void pnp_enter_ite_it8671f(struct device *dev)
{
	pnp_enter_ite_legacy(dev, initkey_it8671f);
}

/* Works for: Many modern ITE Super I/Os */
void pnp_enter_ite(struct device *dev)
{
	outb(0x87, dev->path.pnp.port);
	outb(0x01, dev->path.pnp.port);
	outb(0x55, dev->path.pnp.port);
	outb((dev->path.pnp.port == 0x2e) ? 0x55 : 0xaa, dev->path.pnp.port);
}

/* Works for: IT8761E */
void pnp_enter_ite_it8761e(struct device *dev)
{
	outb(0x87, dev->path.pnp.port);
	outb(0x61, dev->path.pnp.port);
	outb(0x55, dev->path.pnp.port);
	outb((dev->path.pnp.port == 0x2e) ? 0x55 : 0xaa, dev->path.pnp.port);
}

/* Works for: IT8228E */
void pnp_enter_ite_it8228e(struct device *dev)
{
	outb(0x82, dev->path.pnp.port);
	outb(0x28, dev->path.pnp.port);
	outb(0x55, dev->path.pnp.port);
	outb((dev->path.pnp.port == 0x2e) ? 0x55 : 0xaa, dev->path.pnp.port);
}

/* Works for: Various ITE, Fintek, Winbond Super I/Os */
void pnp_enter_8787(struct device *dev)
{
	outb(0x87, dev->path.pnp.port);
	outb(0x87, dev->path.pnp.port);
}

/* Works for: Most/all ALi Super I/Os */
void pnp_enter_ali(struct device *dev)
{
	outb(0x51, dev->path.pnp.port);
	outb(0x23, dev->path.pnp.port);
}

/* Works for: Most/all SMSC Super I/Os */
void pnp_enter_smsc(struct device *dev)
{
	/*
	 * Some of the SMSC Super I/Os have an 0x55,0x55 init, some only
	 * require one 0x55. We do 0x55,0x55 for all of them at the moment,
	 * in the assumption that the extra 0x55 won't hurt the other
	 * Super I/Os. This is verified to be true on (at least) the FDC37N769.
	 */
	outb(0x55, dev->path.pnp.port);
	outb(0x55, dev->path.pnp.port);
}

/* Works for: Some older Winbond Super I/Os */
void pnp_enter_88(struct device *dev)
{
	outb(0x88, dev->path.pnp.port);
}

/* Works for: Some older Winbond Super I/Os */
void pnp_enter_89(struct device *dev)
{
	outb(0x89, dev->path.pnp.port);
}

/* Works for: Some older Winbond Super I/Os */
void pnp_enter_8686(struct device *dev)
{
	outb(0x86, dev->path.pnp.port);
	outb(0x86, dev->path.pnp.port);
}

/* Works for: Various ITE, Fintek, Winbond, SMSC Super I/Os */
void pnp_exit_aa(struct device *dev)
{
	outb(0xaa, dev->path.pnp.port);
}

/* Works for: Most modern ITE Super I/Os */
void pnp_exit_ite(struct device *dev)
{
	pnp_write_config(dev, 0x02, 0x02);
}

/* Works for: Most/all ALi Super I/Os */
void pnp_exit_ali(struct device *dev)
{
	outb(0xbb, dev->path.pnp.port);
}
