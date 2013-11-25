#include <stdint.h>
#include <delay.h>
#include <edid.h>
#include <stdlib.h>
#include <string.h>
#include <arch/io.h>

#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#include "vgaregs.h"
#include "cirrusregs.h"

#define RREG8(reg) read8((mmio) + (reg))
#define WREG8(reg, v) write8((mmio) + (reg), v)

#define SEQ_INDEX 4
#define SEQ_DATA 5

#define WREG_SEQ(reg, v)					\
	do {							\
		WREG8(SEQ_INDEX, reg);				\
		WREG8(SEQ_DATA, v);				\
	} while (0)						\

#define CRT_INDEX 0x14
#define CRT_DATA 0x15

#define WREG_CRT(reg, v)					\
	do {							\
		WREG8(CRT_INDEX, reg);				\
		WREG8(CRT_DATA, v);				\
	} while (0)						\

#define GFX_INDEX 0xe
#define GFX_DATA 0xf

#define WREG_GFX(reg, v)					\
	do {							\
		WREG8(GFX_INDEX, reg);				\
		WREG8(GFX_DATA, v);				\
	} while (0)						\

/*
 * Cirrus has a "hidden" DAC register that can be accessed by writing to
 * the pixel mask register to reset the state, then reading from the register
 * four times. The next write will then pass to the DAC
 */
#define VGA_DAC_MASK 0x6

#define WREG_HDR(v)						\
	do {							\
		RREG8(VGA_DAC_MASK);					\
		RREG8(VGA_DAC_MASK);					\
		RREG8(VGA_DAC_MASK);					\
		RREG8(VGA_DAC_MASK);					\
		WREG8(VGA_DAC_MASK, v);					\
	} while (0)						\

struct drm_display_mode {
	char name[32];

	/* Proposed mode values */
	int clock;		/* in kHz */
	int hdisplay;
	int hsync_start;
	int hsync_end;
	int htotal;
	int hskew;
	int vdisplay;
	int vsync_start;
	int vsync_end;
	int vtotal;
	int vscan;
};

#define DRM_MODE(nm, c, hd, hss, hse, ht, hsk, vd, vss, vse, vt, vs) \
	.name = nm, .clock = (c), \
	.hdisplay = (hd), .hsync_start = (hss), .hsync_end = (hse), \
	.htotal = (ht), .hskew = (hsk), .vdisplay = (vd), \
	.vsync_start = (vss), .vsync_end = (vse), .vtotal = (vt), \
	.vscan = (vs), \

static const struct drm_display_mode mode_640_480 = {
	/* 640x480@60Hz */
	DRM_MODE("640x480", 25175, 640, 656,
		 752, 800, 0, 480, 489, 492, 525, 0)
};
static const struct drm_display_mode mode_800_600 = {
	/* 800x600@60Hz */
	DRM_MODE("800x600", 40000, 800, 840,
		 968, 1056, 0, 600, 601, 605, 628, 0)
};
static const struct drm_display_mode mode_1024_768 = {
	/* 1024x768@60Hz */
	DRM_MODE("1024x768", 65000, 1024, 1048,
		 1184, 1344, 0, 768, 771, 777, 806, 0)
};

static void cirrus_set_start_address(u32 mmio, unsigned offset)
{
	u32 addr;
	u8 tmp;

	addr = offset >> 2;
	WREG_CRT(0x0c, (u8)((addr >> 8) & 0xff));
	WREG_CRT(0x0d, (u8)(addr & 0xff));

	WREG8(CRT_INDEX, 0x1b);
	tmp = RREG8(CRT_DATA);
	tmp &= 0xf2;
	tmp |= (addr >> 16) & 0x01;
	tmp |= (addr >> 15) & 0x0c;
	WREG_CRT(0x1b, tmp);
	WREG8(CRT_INDEX, 0x1d);
	tmp = RREG8(CRT_DATA);
	tmp &= 0x7f;
	tmp |= (addr >> 12) & 0x80;
	WREG_CRT(0x1d, tmp);
}

/*
 * The meat of this driver. The core passes us a mode and we have to program
 * it. The modesetting here is the bare minimum required to satisfy the qemu
 * emulation of this hardware, and running this against a real device is
 * likely to result in an inadequately programmed mode. We've already had
 * the opportunity to modify the mode, so whatever we receive here should
 * be something that can be correctly programmed and displayed
 */
static int cirrus_crtc_mode_set(u32 mmio, int bits_per_pixel,
				const struct drm_display_mode *mode)
{
	int hsyncstart, hsyncend, htotal, hdispend;
	int vtotal, vdispend;
	int pitch, tmp;
	int sr07 = 0, hdr = 0;

	WREG8(VGA_MIS_W - 0x3c0, VGA_MIS_COLOR);

	htotal = mode->htotal / 8;
	hsyncend = mode->hsync_end / 8;
	hsyncstart = mode->hsync_start / 8;
	hdispend = mode->hdisplay / 8;

	vtotal = mode->vtotal;
	vdispend = mode->vdisplay;

	vdispend -= 1;
	vtotal -= 2;

	htotal -= 5;
	hdispend -= 1;
	hsyncstart += 1;
	hsyncend += 1;

	WREG_CRT(VGA_CRTC_V_SYNC_END, 0x20);
	WREG_CRT(VGA_CRTC_H_TOTAL, htotal);
	WREG_CRT(VGA_CRTC_H_DISP, hdispend);
	WREG_CRT(VGA_CRTC_H_SYNC_START, hsyncstart);
	WREG_CRT(VGA_CRTC_H_SYNC_END, hsyncend);
	WREG_CRT(VGA_CRTC_V_TOTAL, vtotal & 0xff);
	WREG_CRT(VGA_CRTC_V_DISP_END, vdispend & 0xff);

	tmp = 0x40;
	if ((vdispend + 1) & 512)
		tmp |= 0x20;
	WREG_CRT(VGA_CRTC_MAX_SCAN, tmp);

	/*
	 * Overflow bits for values that don't fit in the standard registers
	 */
	tmp = 16;
	if (vtotal & 256)
		tmp |= 1;
	if (vdispend & 256)
		tmp |= 2;
	if ((vdispend + 1) & 256)
		tmp |= 8;
	if (vtotal & 512)
		tmp |= 32;
	if (vdispend & 512)
		tmp |= 64;
	WREG_CRT(VGA_CRTC_OVERFLOW, tmp);

	tmp = 0;

	/* More overflow bits */

	if ((htotal + 5) & 64)
		tmp |= 16;
	if ((htotal + 5) & 128)
		tmp |= 32;
	if (vtotal & 256)
		tmp |= 64;
	if (vtotal & 512)
		tmp |= 128;

	WREG_CRT(CL_CRT1A, tmp);

	/* Disable Hercules/CGA compatibility */
	WREG_CRT(VGA_CRTC_MODE, 0x03);

	WREG8(SEQ_INDEX, 0x7);
	sr07 = RREG8(SEQ_DATA);
	sr07 &= 0xe0;
	hdr = 0;
	switch (bits_per_pixel) {
	case 8:
		sr07 |= 0x11;
		break;
	case 16:
		sr07 |= 0xc1;
		hdr = 0xc0;
		break;
	case 24:
		sr07 |= 0x15;
		hdr = 0xc5;
		break;
	case 32:
		sr07 |= 0x19;
		hdr = 0xc5;
		break;
	default:
		return -1;
	}

	WREG_SEQ(0x7, sr07);

	/* Program the pitch */
	pitch = mode->hdisplay * ((bits_per_pixel + 7) / 8);
	tmp = pitch / 8;
	WREG_CRT(VGA_CRTC_OFFSET, tmp);

	/* Enable extended blanking and pitch bits, and enable full memory */
	tmp = 0x22;
	tmp |= (pitch >> 7) & 0x10;
	tmp |= (pitch >> 6) & 0x40;
	WREG_CRT(0x1b, tmp);

	/* Enable high-colour modes */
	WREG_GFX(VGA_GFX_MODE, 0x40);

	/* And set graphics mode */
	WREG_GFX(VGA_GFX_MISC, 0x01);

	WREG_HDR(hdr);

	WREG8(0, 0x20); /* disable blanking */
	cirrus_set_start_address(mmio, 0);
	return 0;
}

static void qemu_cirrus_init(device_t dev)
{
	const struct drm_display_mode *mode;
	int bits_per_pixel;
	struct edid edid;
	u32 addr;
	u32 mmio;

	/* find lfb pci bar */
	addr = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	addr &= ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
	if (!addr)
		return;

	/* find mmio pci bar */
	mmio = pci_read_config32(dev, PCI_BASE_ADDRESS_1);
	mmio &= ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
	if (!mmio)
		return;

	printk(BIOS_DEBUG, "QEMU VGA: cirrus, lfb @ 0x%x, mmio @ 0x%x\n",
	       addr, mmio);

	/* initialize mode */
	bits_per_pixel = 32;
	mode = &mode_800_600;
	cirrus_crtc_mode_set(mmio, bits_per_pixel, mode);

	/* setup coreboot framebuffer */
	edid.ha = mode->hdisplay;
	edid.va = mode->vdisplay;
	edid.bpp = bits_per_pixel;
	set_vbe_mode_info_valid(&edid, addr);
}

static struct device_operations qemu_cirrus_graph_ops = {
	.read_resources	  = pci_dev_read_resources,
	.set_resources	  = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init		  = qemu_cirrus_init,
	.scan_bus	  = 0,
};

static const struct pci_driver qemu_cirrus_driver __pci_driver = {
	.ops	= &qemu_cirrus_graph_ops,
	.vendor = 0x1013,
	.device = 0x00b8,
};
