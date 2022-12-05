/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <framebuffer_info.h>
#include <pc80/vga.h>
#include <pc80/vga_io.h>
#include <stdint.h>

/* VGA init. We use the Bochs VESA VBE extensions  */
#define VBE_DISPI_IOPORT_INDEX          0x01CE
#define VBE_DISPI_IOPORT_DATA           0x01CF

#define VBE_DISPI_INDEX_ID              0x0
#define VBE_DISPI_INDEX_XRES            0x1
#define VBE_DISPI_INDEX_YRES            0x2
#define VBE_DISPI_INDEX_BPP             0x3
#define VBE_DISPI_INDEX_ENABLE          0x4
#define VBE_DISPI_INDEX_BANK            0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH      0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT     0x7
#define VBE_DISPI_INDEX_X_OFFSET        0x8
#define VBE_DISPI_INDEX_Y_OFFSET        0x9
#define VBE_DISPI_INDEX_VIDEO_MEMORY_64K 0xa

#define VBE_DISPI_ID0                   0xB0C0
#define VBE_DISPI_ID1                   0xB0C1
#define VBE_DISPI_ID2                   0xB0C2
#define VBE_DISPI_ID4                   0xB0C4
#define VBE_DISPI_ID5                   0xB0C5

#define VBE_DISPI_DISABLED              0x00
#define VBE_DISPI_ENABLED               0x01
#define VBE_DISPI_LFB_ENABLED           0x40
#define VBE_DISPI_NOCLEARMEM            0x80

static int width  = CONFIG_DRIVERS_EMULATION_QEMU_BOCHS_XRES;
static int height = CONFIG_DRIVERS_EMULATION_QEMU_BOCHS_YRES;

static void bochs_write(struct resource *res, int index, int val)
{
	if (res->flags & IORESOURCE_IO) {
		outw(index, res->base);
		outw(val, res->base + 1);
	} else {
		write16(res2mmio(res, 0x500 + index * 2, 0), val);
	}
}

static int bochs_read(struct resource *res, int index)
{
	if (res->flags & IORESOURCE_IO) {
		outw(index, res->base);
		return inw(res->base + 1);
	} else {
		return read16(res2mmio(res, 0x500 + index * 2, 0));
	}
}

static void bochs_vga_write(struct resource *res, int index, uint8_t val)
{
	if (res->flags & IORESOURCE_IO)
		outb(val, index + 0x3c0);
	else
		write8(res2mmio(res, (0x400 - 0x3c0) + index, 0), val);
}

static struct resource res_legacy = {
	VBE_DISPI_IOPORT_INDEX,
	VBE_DISPI_IOPORT_DATA - VBE_DISPI_IOPORT_INDEX,
	VBE_DISPI_IOPORT_DATA,
	NULL,
	IORESOURCE_IO,
	0,
	1,
	1
};

static void bochs_init_linear_fb(struct device *dev)
{
	struct resource *res_fb, *res_io;
	int id, mem, bar;

	res_fb = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (res_fb && res_fb->flags & IORESOURCE_MEM) {
		/* qemu -vga {std,qxl} */
		bar = 0;
	} else {
		res_fb = probe_resource(dev, PCI_BASE_ADDRESS_1);
		if (res_fb && res_fb->flags & IORESOURCE_MEM) {
			/* qemu -vga vmware */
			bar = 1;
		} else {
			printk(BIOS_ERR, "%s: Not bochs compatible\n", dev_name(dev));
			return;
		}
	}

	/* MMIO bar supported since qemu 3.0+ */
	res_io = probe_resource(dev, PCI_BASE_ADDRESS_2);
	if (((dev->class >> 8) == PCI_CLASS_DISPLAY_VGA) ||
	    !res_io || !(res_io->flags & IORESOURCE_MEM)) {
		printk(BIOS_DEBUG, "QEMU VGA: Using legacy VGA\n");
		res_io = &res_legacy;
	} else {
		printk(BIOS_DEBUG, "QEMU VGA: Using I/O bar at %llx\n", res_io->base);
	}

	/* bochs dispi detection */
	id = bochs_read(res_io, VBE_DISPI_INDEX_ID);
	if ((id & 0xfff0) != VBE_DISPI_ID0) {
		printk(BIOS_DEBUG, "QEMU VGA: bochs dispi: ID mismatch.\n");
		return;
	}
	mem = bochs_read(res_io, VBE_DISPI_INDEX_VIDEO_MEMORY_64K) * 64 * 1024;

	printk(BIOS_DEBUG, "QEMU VGA: bochs dispi interface found, "
	       "%d MiB video memory\n", mem / (1024 * 1024));
	printk(BIOS_DEBUG, "QEMU VGA: framebuffer @ %llx (pci bar %d)\n",
	       res_fb->base, bar);

	/* setup video mode */
	bochs_write(res_io, VBE_DISPI_INDEX_ENABLE,	 0);
	bochs_write(res_io, VBE_DISPI_INDEX_BANK,	 0);
	bochs_write(res_io, VBE_DISPI_INDEX_BPP,	 32);
	bochs_write(res_io, VBE_DISPI_INDEX_XRES,	 width);
	bochs_write(res_io, VBE_DISPI_INDEX_YRES,	 height);
	bochs_write(res_io, VBE_DISPI_INDEX_VIRT_WIDTH,	 width);
	bochs_write(res_io, VBE_DISPI_INDEX_VIRT_HEIGHT, height);
	bochs_write(res_io, VBE_DISPI_INDEX_X_OFFSET,	 0);
	bochs_write(res_io, VBE_DISPI_INDEX_Y_OFFSET,	 0);
	bochs_write(res_io, VBE_DISPI_INDEX_ENABLE,
		    VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);

	bochs_vga_write(res_io, 0, 0x20);	/* disable blanking */

	/* Advertise new mode */
	fb_add_framebuffer_info(res_fb->base, width, height, 4 * width, 32);
}

static void bochs_init_text_mode(struct device *dev)
{
	vga_misc_write(0x1);
	vga_textmode_init();
}

static void bochs_init(struct device *dev)
{
	if (CONFIG(LINEAR_FRAMEBUFFER))
		bochs_init_linear_fb(dev);
	else if (CONFIG(VGA_TEXT_FRAMEBUFFER))
		bochs_init_text_mode(dev);
}

static struct device_operations qemu_graph_ops = {
	.read_resources	  = pci_dev_read_resources,
	.set_resources	  = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init		  = bochs_init,
};

static const struct pci_driver qemu_stdvga_driver __pci_driver = {
	.ops	= &qemu_graph_ops,
	.vendor = 0x1234,
	.device = 0x1111,
};

static const struct pci_driver qemu_vmware_driver __pci_driver = {
	.ops	= &qemu_graph_ops,
	.vendor = 0x15ad,
	.device = 0x0405,
};
static const struct pci_driver qemu_qxl_driver __pci_driver = {
	.ops	= &qemu_graph_ops,
	.vendor = 0x1b36,
	.device = 0x0100,
};
