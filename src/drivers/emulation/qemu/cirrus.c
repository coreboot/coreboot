/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdint.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <pc80/vga.h>
#include <pc80/vga_io.h>
#include <framebuffer_info.h>

static int width  = CONFIG_DRIVERS_EMULATION_QEMU_BOCHS_XRES;
static int height = CONFIG_DRIVERS_EMULATION_QEMU_BOCHS_YRES;
static u32 addr   = 0;

enum
  {
    VGA_CR_HTOTAL = 0x00,
    VGA_CR_HORIZ_END = 0x01,
    VGA_CR_HBLANK_START = 0x02,
    VGA_CR_HBLANK_END = 0x03,
    VGA_CR_HORIZ_SYNC_PULSE_START = 0x04,
    VGA_CR_HORIZ_SYNC_PULSE_END = 0x05,
    VGA_CR_VERT_TOTAL = 0x06,
    VGA_CR_OVERFLOW = 0x07,
    VGA_CR_BYTE_PANNING = 0x08,
    VGA_CR_CELL_HEIGHT = 0x09,
    VGA_CR_CURSOR_START = 0x0a,
    VGA_CR_CURSOR_END = 0x0b,
    VGA_CR_START_ADDR_HIGH_REGISTER = 0x0c,
    VGA_CR_START_ADDR_LOW_REGISTER = 0x0d,
    VGA_CR_CURSOR_ADDR_HIGH = 0x0e,
    VGA_CR_CURSOR_ADDR_LOW = 0x0f,
    VGA_CR_VSYNC_START = 0x10,
    VGA_CR_VSYNC_END = 0x11,
    VGA_CR_VDISPLAY_END = 0x12,
    VGA_CR_PITCH = 0x13,
    VGA_CR_UNDERLINE_LOCATION = 0x14,
    VGA_CR_VERTICAL_BLANK_START = 0x15,
    VGA_CR_VERTICAL_BLANK_END = 0x16,
    VGA_CR_MODE = 0x17,
    VGA_CR_LINE_COMPARE = 0x18,
  };

#define VGA_IO_MISC_COLOR 0x01

#define VGA_CR_WIDTH_DIVISOR 8

#define VGA_CR_OVERFLOW_VERT_DISPLAY_ENABLE_END1_SHIFT 7
#define VGA_CR_OVERFLOW_VERT_DISPLAY_ENABLE_END1_MASK 0x02
#define VGA_CR_OVERFLOW_VERT_DISPLAY_ENABLE_END2_SHIFT 3
#define VGA_CR_OVERFLOW_VERT_DISPLAY_ENABLE_END2_MASK 0x40

#define VGA_CR_OVERFLOW_VERT_TOTAL1_SHIFT 8
#define VGA_CR_OVERFLOW_VERT_TOTAL1_MASK 0x01
#define VGA_CR_OVERFLOW_VERT_TOTAL2_SHIFT 4
#define VGA_CR_OVERFLOW_VERT_TOTAL2_MASK 0x20

#define VGA_CR_OVERFLOW_VSYNC_START1_SHIFT 6
#define VGA_CR_OVERFLOW_VSYNC_START1_MASK 0x04
#define VGA_CR_OVERFLOW_VSYNC_START2_SHIFT 2
#define VGA_CR_OVERFLOW_VSYNC_START2_MASK 0x80

#define VGA_CR_OVERFLOW_HEIGHT1_SHIFT 7
#define VGA_CR_OVERFLOW_HEIGHT1_MASK 0x02
#define VGA_CR_OVERFLOW_HEIGHT2_SHIFT 3
#define VGA_CR_OVERFLOW_HEIGHT2_MASK 0xc0
#define VGA_CR_OVERFLOW_LINE_COMPARE_SHIFT 4
#define VGA_CR_OVERFLOW_LINE_COMPARE_MASK 0x10

#define VGA_CR_CELL_HEIGHT_LINE_COMPARE_MASK 0x40
#define VGA_CR_CELL_HEIGHT_LINE_COMPARE_SHIFT 3
#define VGA_CR_CELL_HEIGHT_VERTICAL_BLANK_MASK 0x20
#define VGA_CR_CELL_HEIGHT_VERTICAL_BLANK_SHIFT 4
#define VGA_CR_CELL_HEIGHT_DOUBLE_SCAN 0x80
enum
  {
    VGA_CR_CURSOR_START_DISABLE = (1 << 5)
  };

#define VGA_CR_PITCH_DIVISOR 8

enum
  {
    VGA_CR_MODE_NO_CGA = 0x01,
    VGA_CR_MODE_NO_HERCULES = 0x02,
    VGA_CR_MODE_ADDRESS_WRAP = 0x20,
    VGA_CR_MODE_BYTE_MODE = 0x40,
    VGA_CR_MODE_TIMING_ENABLE = 0x80
  };

enum
  {
    VGA_SR_RESET = 0,
    VGA_SR_CLOCKING_MODE = 1,
    VGA_SR_MAP_MASK_REGISTER = 2,
    VGA_SR_CHAR_MAP_SELECT = 3,
    VGA_SR_MEMORY_MODE = 4,
  };

enum
  {
    VGA_SR_RESET_ASYNC = 1,
    VGA_SR_RESET_SYNC = 2
  };

enum
  {
    VGA_SR_CLOCKING_MODE_8_DOT_CLOCK = 1
  };

enum
  {
    VGA_SR_MEMORY_MODE_NORMAL = 0,
    VGA_SR_MEMORY_MODE_EXTERNAL_VIDEO_MEMORY = 2,
    VGA_SR_MEMORY_MODE_SEQUENTIAL_ADDRESSING = 4,
    VGA_SR_MEMORY_MODE_CHAIN4 = 8,
  };

enum
  {
    VGA_GR_SET_RESET_PLANE = 0,
    VGA_GR_SET_RESET_PLANE_ENABLE = 1,
    VGA_GR_COLOR_COMPARE = 2,
    VGA_GR_READ_MAP_REGISTER = 4,
    VGA_GR_MODE = 5,
    VGA_GR_GR6 = 6,
    VGA_GR_COLOR_COMPARE_DISABLE = 7,
    VGA_GR_BITMASK = 8,
    VGA_GR_MAX
  };

enum
  {
    VGA_TEXT_TEXT_PLANE = 0,
    VGA_TEXT_ATTR_PLANE = 1,
    VGA_TEXT_FONT_PLANE = 2
  };

enum
  {
    VGA_GR_GR6_GRAPHICS_MODE = 1,
    VGA_GR_GR6_MMAP_A0 = (1 << 2),
    VGA_GR_GR6_MMAP_CGA = (3 << 2)
  };

enum
  {
    VGA_GR_MODE_READ_MODE1 = 0x08,
    VGA_GR_MODE_ODD_EVEN = 0x10,
    VGA_GR_MODE_ODD_EVEN_SHIFT = 0x20,
    VGA_GR_MODE_256_COLOR = 0x40
  };

#define CIRRUS_CR_EXTENDED_DISPLAY 0x1b
#define CIRRUS_CR_EXTENDED_OVERLAY 0x1d

#define CIRRUS_CR_EXTENDED_DISPLAY_PITCH_MASK 0x10
#define CIRRUS_CR_EXTENDED_DISPLAY_PITCH_SHIFT 4
#define CIRRUS_CR_EXTENDED_DISPLAY_START_MASK1 0x1
#define CIRRUS_CR_EXTENDED_DISPLAY_START_SHIFT1 16
#define CIRRUS_CR_EXTENDED_DISPLAY_START_MASK2 0xc
#define CIRRUS_CR_EXTENDED_DISPLAY_START_SHIFT2 15

#define CIRRUS_CR_EXTENDED_OVERLAY_DISPLAY_START_MASK 0x80
#define CIRRUS_CR_EXTENDED_OVERLAY_DISPLAY_START_SHIFT 12
#define CIRRUS_SR_EXTENDED_MODE 7
#define CIRRUS_SR_EXTENDED_MODE_LFB_ENABLE 0xf0
#define CIRRUS_SR_EXTENDED_MODE_ENABLE_EXT 0x01
#define CIRRUS_SR_EXTENDED_MODE_32BPP      0x08
#define CIRRUS_HIDDEN_DAC_888COLOR 0xc5

static void
write_hidden_dac (uint8_t data)
{
	inb (0x3c8);
	inb (0x3c6);
	inb (0x3c6);
	inb (0x3c6);
	inb (0x3c6);
	outb (data, 0x3c6);
}

static void cirrus_init_linear_fb(struct device *dev)
{
	uint8_t cr_ext, cr_overlay;
	unsigned int pitch = (width * 4) / VGA_CR_PITCH_DIVISOR;
	uint8_t sr_ext = 0, hidden_dac = 0;
	unsigned int vdisplay_end = height - 2;
	unsigned int line_compare = 0x3ff;
	uint8_t overflow, cell_height_reg;
	unsigned int horizontal_end = width / VGA_CR_WIDTH_DIVISOR;
	unsigned int horizontal_total = horizontal_end + 40;
	unsigned int horizontal_blank_start = horizontal_end;
	unsigned int horizontal_sync_pulse_start = horizontal_end + 3;
	unsigned int horizontal_sync_pulse_end = 0;

	unsigned int horizontal_blank_end = 0;
	unsigned int vertical_blank_start = height + 1;
	unsigned int vertical_blank_end = 0;
	unsigned int vertical_sync_start = height + 3;
	unsigned int vertical_sync_end = 0;
	unsigned int vertical_total = height + 40;

	/* find lfb pci bar */
	addr = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	addr &= ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
	printk(BIOS_DEBUG, "QEMU VGA: cirrus framebuffer @ %x (pci bar 0)\n",
	       addr);

	vga_misc_write (VGA_IO_MISC_COLOR);

	vga_sr_write (VGA_SR_MEMORY_MODE,
		      VGA_SR_MEMORY_MODE_NORMAL);

	vga_sr_write (VGA_SR_MAP_MASK_REGISTER,
		      (1 << VGA_TEXT_TEXT_PLANE)
		      | (1 << VGA_TEXT_ATTR_PLANE));

	vga_sr_write (VGA_SR_CLOCKING_MODE,
		      VGA_SR_CLOCKING_MODE_8_DOT_CLOCK);

	vga_palette_disable();

	/* Disable CR0-7 write protection.  */
	vga_cr_write (VGA_CR_VSYNC_END, 0);

	overflow = ((vertical_total >> VGA_CR_OVERFLOW_VERT_TOTAL1_SHIFT)
		    & VGA_CR_OVERFLOW_VERT_TOTAL1_MASK)
		| ((vertical_total >> VGA_CR_OVERFLOW_VERT_TOTAL2_SHIFT)
		   & VGA_CR_OVERFLOW_VERT_TOTAL2_MASK)
		| ((vertical_sync_start >> VGA_CR_OVERFLOW_VSYNC_START2_SHIFT)
		   & VGA_CR_OVERFLOW_VSYNC_START2_MASK)
		| ((vertical_sync_start >> VGA_CR_OVERFLOW_VSYNC_START1_SHIFT)
		   & VGA_CR_OVERFLOW_VSYNC_START1_MASK)
		| ((vdisplay_end >> VGA_CR_OVERFLOW_VERT_DISPLAY_ENABLE_END1_SHIFT)
		   & VGA_CR_OVERFLOW_VERT_DISPLAY_ENABLE_END1_MASK)
		| ((vdisplay_end >> VGA_CR_OVERFLOW_VERT_DISPLAY_ENABLE_END2_SHIFT)
		   & VGA_CR_OVERFLOW_VERT_DISPLAY_ENABLE_END2_MASK)
		| ((vertical_sync_start >> VGA_CR_OVERFLOW_VSYNC_START1_SHIFT)
		   & VGA_CR_OVERFLOW_VSYNC_START1_MASK)
		| ((line_compare >> VGA_CR_OVERFLOW_LINE_COMPARE_SHIFT)
		   & VGA_CR_OVERFLOW_LINE_COMPARE_MASK);

	cell_height_reg = ((vertical_blank_start
			    >> VGA_CR_CELL_HEIGHT_VERTICAL_BLANK_SHIFT)
			   & VGA_CR_CELL_HEIGHT_VERTICAL_BLANK_MASK)
		| ((line_compare >> VGA_CR_CELL_HEIGHT_LINE_COMPARE_SHIFT)
		   & VGA_CR_CELL_HEIGHT_LINE_COMPARE_MASK);

	vga_cr_write (VGA_CR_HTOTAL, horizontal_total - 1);
	vga_cr_write (VGA_CR_HORIZ_END, horizontal_end - 1);
	vga_cr_write (VGA_CR_HBLANK_START, horizontal_blank_start - 1);
	vga_cr_write (VGA_CR_HBLANK_END, horizontal_blank_end);
	vga_cr_write (VGA_CR_HORIZ_SYNC_PULSE_START,
		      horizontal_sync_pulse_start);
	vga_cr_write (VGA_CR_HORIZ_SYNC_PULSE_END,
		      horizontal_sync_pulse_end);
	vga_cr_write (VGA_CR_VERT_TOTAL, vertical_total & 0xff);
	vga_cr_write (VGA_CR_OVERFLOW, overflow);
	vga_cr_write (VGA_CR_CELL_HEIGHT, cell_height_reg);
	vga_cr_write (VGA_CR_VSYNC_START, vertical_sync_start & 0xff);
	vga_cr_write (VGA_CR_VSYNC_END, vertical_sync_end & 0x0f);
	vga_cr_write (VGA_CR_VDISPLAY_END, vdisplay_end & 0xff);
	vga_cr_write (VGA_CR_PITCH, pitch & 0xff);
	vga_cr_write (VGA_CR_VERTICAL_BLANK_START, vertical_blank_start & 0xff);
	vga_cr_write (VGA_CR_VERTICAL_BLANK_END, vertical_blank_end & 0xff);
	vga_cr_write (VGA_CR_LINE_COMPARE, line_compare & 0xff);

	vga_gr_write (VGA_GR_MODE, VGA_GR_MODE_256_COLOR | VGA_GR_MODE_READ_MODE1);
	vga_gr_write (VGA_GR_GR6, VGA_GR_GR6_GRAPHICS_MODE);

	vga_sr_write (VGA_SR_MEMORY_MODE, VGA_SR_MEMORY_MODE_NORMAL);

	vga_cr_write (CIRRUS_CR_EXTENDED_DISPLAY,
		      (pitch >> CIRRUS_CR_EXTENDED_DISPLAY_PITCH_SHIFT)
		      & CIRRUS_CR_EXTENDED_DISPLAY_PITCH_MASK);

	vga_cr_write (VGA_CR_MODE, VGA_CR_MODE_TIMING_ENABLE
		      | VGA_CR_MODE_BYTE_MODE
		      | VGA_CR_MODE_NO_HERCULES | VGA_CR_MODE_NO_CGA);

	vga_cr_write (VGA_CR_START_ADDR_LOW_REGISTER, 0);
	vga_cr_write (VGA_CR_START_ADDR_HIGH_REGISTER, 0);

	cr_ext = vga_cr_read (CIRRUS_CR_EXTENDED_DISPLAY);
	cr_ext &= ~(CIRRUS_CR_EXTENDED_DISPLAY_START_MASK1
		    | CIRRUS_CR_EXTENDED_DISPLAY_START_MASK2);
	vga_cr_write (CIRRUS_CR_EXTENDED_DISPLAY, cr_ext);

	cr_overlay = vga_cr_read (CIRRUS_CR_EXTENDED_OVERLAY);
	cr_overlay &= ~(CIRRUS_CR_EXTENDED_OVERLAY_DISPLAY_START_MASK);
	vga_cr_write (CIRRUS_CR_EXTENDED_OVERLAY, cr_overlay);

	sr_ext = CIRRUS_SR_EXTENDED_MODE_LFB_ENABLE
		| CIRRUS_SR_EXTENDED_MODE_ENABLE_EXT
		| CIRRUS_SR_EXTENDED_MODE_32BPP;
	hidden_dac = CIRRUS_HIDDEN_DAC_888COLOR;
	vga_sr_write (CIRRUS_SR_EXTENDED_MODE, sr_ext);
	write_hidden_dac (hidden_dac);

	fb_add_framebuffer_info(addr, width, height, 4 * width, 32);
}

static void cirrus_init_text_mode(struct device *dev)
{
	vga_misc_write(0x1);
	vga_textmode_init();
}

static void cirrus_init(struct device *dev)
{
	if (CONFIG(LINEAR_FRAMEBUFFER))
		cirrus_init_linear_fb(dev);
	else if (CONFIG(VGA_TEXT_FRAMEBUFFER))
		cirrus_init_text_mode(dev);
}

static struct device_operations qemu_cirrus_graph_ops = {
	.read_resources	  = pci_dev_read_resources,
	.set_resources	  = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init		  = cirrus_init,
};

static const struct pci_driver qemu_cirrus_driver __pci_driver = {
	.ops	= &qemu_cirrus_graph_ops,
	.vendor = 0x1013,
	.device = 0x00b8,
};
