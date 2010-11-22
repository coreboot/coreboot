#include <delay.h>
#include <stdlib.h>
#include <string.h>
#include <arch/io.h>

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

/* VGA init. We use the Bochs VESA VBE extensions  */
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
#define VBE_DISPI_INDEX_NB              0xa

#define VBE_DISPI_ID0                   0xB0C0
#define VBE_DISPI_ID1                   0xB0C1
#define VBE_DISPI_ID2                   0xB0C2

#define VBE_DISPI_DISABLED              0x00
#define VBE_DISPI_ENABLED               0x01
#define VBE_DISPI_LFB_ENABLED           0x40
#define VBE_DISPI_NOCLEARMEM            0x80

#define VBE_DISPI_LFB_PHYSICAL_ADDRESS  0xE0000000

static void vbe_outw(int index, int val)
{
    outw(0x1ce, index);
    outw(0x1cf, val);
}

static void qemu_init(void)
{
    int width=640, height=480, depth=8;

    printk(BIOS_DEBUG, "Initializing VGA!\n");

    vbe_outw(VBE_DISPI_INDEX_XRES, width);
    vbe_outw(VBE_DISPI_INDEX_YRES, height);
    vbe_outw(VBE_DISPI_INDEX_BPP, depth);
    vbe_outw(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED);
    outb(0x3c0, 0x20); /* disable blanking */
/*
    vga_fb_phys_addr = VBE_DISPI_LFB_PHYSICAL_ADDRESS;
    vga_fb_width = width;
    vga_fb_height = height;
    vga_fb_depth = depth;
    vga_fb_bpp = (depth + 7) >> 3;
    vga_fb_linesize = width * vga_fb_bpp;
    */
}

static struct device_operations qemu_graph_ops  = {
        .read_resources   = pci_dev_read_resources,
        .set_resources    = pci_dev_set_resources,
        .enable_resources = pci_dev_enable_resources,
        .init             = qemu_init,
        .scan_bus         = 0,
};

static const struct pci_driver qemu_graph_driver __pci_driver = {
        .ops    = &qemu_graph_ops,
        .vendor = 0x1234,
        .device = 0x1111,
};


