/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <stdint.h>
#include <edid.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <pc80/vga.h>
#include <pc80/vga_io.h>

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

static void bochs_write(int index, int val)
{
	outw(index, VBE_DISPI_IOPORT_INDEX);
	outw(val, VBE_DISPI_IOPORT_DATA);
}

static int bochs_read(int index)
{
	outw(index, VBE_DISPI_IOPORT_INDEX);
	return inw(VBE_DISPI_IOPORT_DATA);
}

static void bochs_init_linear_fb(struct device *dev)
{
	struct edid edid;
	int id, mem, bar;
	u32 addr;

	/* bochs dispi detection */
	id = bochs_read(VBE_DISPI_INDEX_ID);
	if ((id & 0xfff0) != VBE_DISPI_ID0) {
		printk(BIOS_DEBUG, "QEMU VGA: bochs dispi: ID mismatch.\n");
		return;
	}
	mem = bochs_read(VBE_DISPI_INDEX_VIDEO_MEMORY_64K) * 64 * 1024;

	/* find lfb pci bar */
	addr = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	if ((addr & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_MEMORY) {
		/* qemu -vga {std,qxl} */
		bar = 0;
	} else {
		/* qemu -vga vmware */
		addr = pci_read_config32(dev, PCI_BASE_ADDRESS_1);
		bar = 1;
	}
	addr &= ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;

	if (!addr)
		return;

	printk(BIOS_DEBUG, "QEMU VGA: bochs dispi interface found, "
	       "%d MiB video memory\n", mem / (1024 * 1024));
	printk(BIOS_DEBUG, "QEMU VGA: framebuffer @ %x (pci bar %d)\n",
	       addr, bar);

	/* setup video mode */
	bochs_write(VBE_DISPI_INDEX_ENABLE,	 0);
	bochs_write(VBE_DISPI_INDEX_BANK,	 0);
	bochs_write(VBE_DISPI_INDEX_BPP,	 32);
	bochs_write(VBE_DISPI_INDEX_XRES,	 width);
	bochs_write(VBE_DISPI_INDEX_YRES,	 height);
	bochs_write(VBE_DISPI_INDEX_VIRT_WIDTH,	 width);
	bochs_write(VBE_DISPI_INDEX_VIRT_HEIGHT, height);
	bochs_write(VBE_DISPI_INDEX_X_OFFSET,	 0);
	bochs_write(VBE_DISPI_INDEX_Y_OFFSET,	 0);
	bochs_write(VBE_DISPI_INDEX_ENABLE,
		    VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);

	outb(0x20, 0x3c0); /* disable blanking */

	/* setup coreboot framebuffer */
	edid.mode.ha = width;
	edid.mode.va = height;
	edid.panel_bits_per_color = 8;
	edid.panel_bits_per_pixel = 24;
	edid_set_framebuffer_bits_per_pixel(&edid, 32, 0);
	set_vbe_mode_info_valid(&edid, addr);
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
