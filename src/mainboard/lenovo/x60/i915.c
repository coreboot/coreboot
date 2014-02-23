/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <device/device.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <delay.h>
#include <pc80/mc146818rtc.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/interrupt.h>
#include <boot/coreboot_tables.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <smbios.h>
#include <device/pci.h>
#include <ec/google/chromeec/ec.h>
#include <cbfs_core.h>

#include <cpu/x86/tsc.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <edid.h>
#include "i915io.h"
#include <drivers/intel/gma/edid.h>
#include <drivers/intel/gma/i915_reg.h>

#define  LVDS_DETECTED	(1 << 1)
#define  LVDS_BORDER_ENABLE	(1 << 15)
#define  LVDS_PORT_ENABLE	(1 << 31)
#define  LVDS_CLOCK_A_POWERUP_ALL	(3 << 8)
#define  LVDS_CLOCK_B_POWERUP_ALL	(3 << 4)
#define  LVDS_CLOCK_BOTH_POWERUP_ALL	(3 << 2)
#define   DISPPLANE_BGRX888			(0x6<<26)
#define   DPLLB_LVDS_P2_CLOCK_DIV_7	(1 << 24) /* i915 */

enum {
	vmsg = 1, vio = 2, vspin = 4,
};

#define PGETLB_CTL 0x2020

static int verbose = 0;

static unsigned int *mmio;
static unsigned int graphics;
static unsigned short addrport;
static unsigned short dataport;
static unsigned int physbase;

#define PGETBL_CTL	0x2020
#define PGETBL_ENABLED	0x00000001


static u32 htotal, hblank, hsync, vtotal, vblank, vsync;

#define READ32(addr) io_i915_READ32(addr)
#define WRITE32(val, addr) io_i915_WRITE32(val, addr)

static char *regname(unsigned long addr)
{
	static char name[16];
	snprintf(name, sizeof (name), "0x%lx", addr);
	return name;
}

unsigned long io_i915_READ32(unsigned long addr)
{
	unsigned long val;
	outl(addr, addrport);
	val = inl(dataport);
	if (verbose & vio)
		printk(BIOS_SPEW, "%s: Got %08lx\n", regname(addr), val);
	return val;
}

void io_i915_WRITE32(unsigned long val, unsigned long addr)
{
	if (verbose & vio)
		printk(BIOS_SPEW, "%s: outl %08lx\n", regname(addr), val);
	outl(addr, addrport);
	outl(val, dataport);
}


/*
  2560
  4 words per
  4 *p
  10240
  4k bytes per page
  4096/p
  2.50
  1700 lines
  1700 * p
  4250.00
  PTEs
*/
static void
setgtt(int start, int end, unsigned long base, int inc)
{
	int i;

	printk(BIOS_INFO, "%s(%d,%d,0x%08lx,%d);\n",
			 __func__, start, end, base, inc);

	for (i = start; i < end; i++) {
		u32 word = base + i*inc;
		WRITE32(word|1, (i*4)|1);
	}
}

int gtt_setup(unsigned int mmiobase);
int gtt_setup(unsigned int mmiobase)
{
	unsigned long PGETBL_save;

	PGETBL_save = read32(mmiobase + PGETBL_CTL) & ~PGETBL_ENABLED;
	PGETBL_save |= PGETBL_ENABLED;

	PGETBL_save |= pci_read_config32(dev_find_slot(0, PCI_DEVFN(2,0)), 0x5c) & 0xfffff000;
	PGETBL_save |= 2; /* set GTT to 256kb */

	write32(mmiobase + GFX_FLSH_CNTL, 0);

	write32(mmiobase + PGETBL_CTL, PGETBL_save);

	/* verify */
	if (read32(mmiobase + PGETBL_CTL) & PGETBL_ENABLED) {
		printk(BIOS_DEBUG, "gtt_setup is enabled.\n");
	} else {
		printk(BIOS_DEBUG, "gtt_setup failed!!!\n");
		return 1;
	}
	write32(mmiobase + GFX_FLSH_CNTL, 0);

	return 0;
}


static unsigned long tickspermicrosecond = 1795;
static unsigned long long globalstart;

static unsigned long
microseconds(unsigned long long start, unsigned long long end)
{
	unsigned long ret;
	ret = ((end - start)/tickspermicrosecond);
	return ret;
}

static unsigned long globalmicroseconds(void)
{
	return microseconds(globalstart, rdtscll());
}

extern struct iodef iodefs[];
extern int niodefs;

/* fill the palette. This runs when the P opcode is hit. */
static void palette(void)
{
	int i;
	unsigned long color = 0;

	for (i = 0; i < 256; i++, color += 0x010101)
		io_i915_WRITE32(color, _LGC_PALETTE_A + (i<<2));
}

static unsigned long times[4096];

static int run(int index)
{
	int i, prev = 0;
	struct iodef *id, *lastidread = 0;
	unsigned long u, t;
	if (index >= niodefs)
		return index;
	/* state machine! */
	for (i = index, id = &iodefs[i]; id->op; i++, id++) {
		switch (id->op) {
		case M:
			if (verbose & vmsg)
				printk(BIOS_SPEW, "%ld: %s\n",
						globalmicroseconds(), id->msg);
			break;
		case P:
			palette();
			break;
		case R:
			u = READ32(id->addr);
			if (verbose & vio)
				printk(BIOS_SPEW, "\texpect %08lx\n", id->data);
			/* we're looking for something. */
			if (lastidread->addr == id->addr) {
				/* they're going to be polling.
				 * just do it 1000 times
				 */
				for (t = 0; t < 1000 && id->data != u; t++)
					u = READ32(id->addr);

				if (verbose & vspin)
					printk(BIOS_SPEW,
					       "%s: # loops %ld got %08lx want %08lx\n",
						regname(id->addr),
						t, u, id->data);
			}
			lastidread = id;
			break;
		case W:
			WRITE32(id->data, id->addr);
			if (id->addr == PCH_PP_CONTROL) {
				if (verbose & vio)
					printk(BIOS_SPEW, "PCH_PP_CONTROL\n");
				switch (id->data & 0xf) {
				case 8:
					break;
				case 7:
					break;
				default:
					udelay(100000);
					if (verbose & vio)
						printk(BIOS_SPEW, "U %d\n",
						       100000);
				}
			}
			break;
		case V:
			if (id->count < 8) {
				prev = verbose;
				verbose = id->count;
			} else {
				verbose = prev;
			}
			printk(BIOS_SPEW, "Change verbosity to %d\n", verbose);
			break;
		case I:
			printk(BIOS_SPEW, "run: return %d\n", i+1);
			return i+1;
			break;
		default:
			printk(BIOS_SPEW, "BAD TABLE, opcode %d @ %d\n",
			       id->op, i);
			return -1;
		}
		if (id->udelay)
			udelay(id->udelay);
		if (i < ARRAY_SIZE(times))
			times[i] = globalmicroseconds();
	}
	printk(BIOS_SPEW, "run: return %d\n", i);
	return i+1;
}

int i915lightup(unsigned int physbase, unsigned int iobase, unsigned int mmio,
		unsigned int gfx);

int i915lightup(unsigned int pphysbase, unsigned int piobase,
		unsigned int pmmio, unsigned int pgfx)
{
	struct edid edid;
	u8 x60_edid_data[256];
	int i, j;

	int index;
	unsigned long temp;
	int hpolarity, vpolarity;
	u32 candp1, candn;
	u32 best_delta = 0xffffffff;
	u32 target_frequency;
	u32 pixel_p1 = 1;
	u32 pixel_n = 1;
	u32 pixel_m1 = 1;
	u32 pixel_m2 = 1;
	u32 hactive, vactive, right_border, bottom_border;
	u32 hfront_porch, vfront_porch;

	mmio = (void *)pmmio;
	addrport = piobase;
	dataport = addrport + 4;
	physbase = pphysbase;
	graphics = pgfx;
	printk(BIOS_SPEW,
		"i915lightup: graphics %p mmio %p addrport %04x physbase %08x\n",
		(void *)graphics, mmio, addrport, physbase);
	globalstart = rdtscll();

	intel_gmbus_read_edid(pmmio, 3, 0x50, x60_edid_data);
	decode_edid(x60_edid_data,
		    sizeof(x60_edid_data), &edid);

	hpolarity = (edid.phsync == '-');
	vpolarity = (edid.pvsync == '-');
	hactive = edid.x_resolution;
	vactive = edid.y_resolution;
	right_border = edid.hborder;
	bottom_border = edid.vborder;
	hfront_porch = edid.hso;
	vfront_porch = edid.vso;

	htotal = (edid.ha - 1) | ((edid.ha + edid.hbl - 1) << 16);
	printk(BIOS_SPEW, "I915_WRITE(HTOTAL(pipe), %08x)\n", htotal);

	hblank = (edid.ha  - 1) | ((edid.ha + edid.hbl - 1) << 16);
	printk(BIOS_SPEW, "I915_WRITE(HBLANK(pipe),0x%08x)\n", hblank);

	hsync = (edid.ha + edid.hso  - 1) |
		((edid.ha + edid.hso + edid.hspw - 1) << 16);
	printk(BIOS_SPEW, "I915_WRITE(HSYNC(pipe),0x%08x)\n", hsync);

	vtotal = (edid.va - 1) | ((edid.va + edid.vbl - 1) << 16);
	printk(BIOS_SPEW, "I915_WRITE(VTOTAL(pipe), %08x)\n", vtotal);

	vblank = (edid.va  - 1) | ((edid.va + edid.vbl - 1) << 16);
	printk(BIOS_SPEW, "I915_WRITE(VBLANK(pipe),0x%08x)\n", vblank);

	vsync = (edid.va + edid.vso  - 1) |
		((edid.va + edid.vso + edid.vspw - 1) << 16);
	printk(BIOS_SPEW, "I915_WRITE(VSYNC(pipe),0x%08x)\n", vsync);

	printk(BIOS_SPEW, "Table has %d elements\n", niodefs);

	for (i = 0; i < 2; i++)
		for (j = 0; j < 0x100; j++)
			write32(pmmio + PALETTE(i) + 4 * j, 0x10101 * j);


	index = run(0);
	printk(BIOS_SPEW, "Run returns %d\n", index);

#define IS_DUAL_CHANNEL 0
#define USE_SSC 1

	target_frequency = IS_DUAL_CHANNEL ? edid.pixel_clock
		: (2 * edid.pixel_clock);

	/* Find suitable divisors.  */
	for (candp1 = 1; candp1 <= 8; candp1++) {
		for (candn = 5; candn <= 10; candn++) {
			u32 cur_frequency;
			u32 m; /* 77 - 131.  */
			u32 denom; /* 35 - 560.  */
			u32 current_delta;

			denom = candn * candp1 * 7;
			/* Doesnt overflow for up to
			   5000000 kHz = 5 GHz.  */
			m = (target_frequency * denom + 60000) / 120000;

			if (m < 77 || m > 131)
				continue;

			cur_frequency = (120000 * m) / denom;
			if (target_frequency > cur_frequency)
				current_delta = target_frequency - cur_frequency;
			else
				current_delta = cur_frequency - target_frequency;


			if (best_delta > current_delta) {
				best_delta = current_delta;
				pixel_n = candn;
				pixel_p1 = candp1;
				pixel_m2 = ((m + 3) % 5) + 7;
				pixel_m1 = (m - pixel_m2) / 5;
			}
		}
	}

	if (best_delta == 0xffffffff) {
		printk (BIOS_ERR, "Couldn't find GFX clock divisors\n");
		return -1;
	}

	write32(pmmio + TV_CTL, 0);
	write32(pmmio + TV_DAC, 0);

	write32(pmmio + PIPECONF(1), PIPECONF_DISABLE);

	write32(pmmio + PIPESRC(0), ((hactive - 1) << 16) | (vactive - 1));
	write32(pmmio + PF_CTL(0),0);
	write32(pmmio + PF_WIN_SZ(0), 0);
	write32(pmmio + PF_WIN_POS(0), 0);

	write32(pmmio + HTOTAL(1),
		((hactive + right_border + hblank - 1) << 16)
		| (hactive - 1));
	write32(pmmio + HBLANK(1),
		((hactive + right_border + hblank - 1) << 16)
		| (hactive + right_border - 1));
	write32(pmmio + HSYNC(1),
		((hactive + right_border + hfront_porch + hsync - 1) << 16)
		| (hactive + right_border + hfront_porch - 1));

	write32(pmmio + VTOTAL(1), ((vactive + bottom_border + vblank - 1) << 16)
		| (vactive - 1));
	write32(pmmio + VBLANK(1), ((vactive + bottom_border + vblank - 1) << 16)
		| (vactive + bottom_border - 1));
	write32(pmmio + VSYNC(1),
		(vactive + bottom_border + vfront_porch + vsync - 1)
		| (vactive + bottom_border + vfront_porch - 1));

	write32(pmmio + DSPCNTR(0), DISPPLANE_BGRX888
		| DISPPLANE_SEL_PIPE_B | DISPPLANE_GAMMA_ENABLE);

	mdelay(1);
	write32(pmmio + PP_CONTROL, PANEL_UNLOCK_REGS
		| (read32(pmmio + PP_CONTROL) & ~PANEL_UNLOCK_MASK));
	write32(pmmio + FP0(1),
		((pixel_n - 2) << 16)
		| ((pixel_m1 - 2) << 8) | pixel_m2);
	write32(pmmio + DPLL(1),
		DPLL_VCO_ENABLE | DPLLB_MODE_LVDS
		| (IS_DUAL_CHANNEL ? DPLLB_LVDS_P2_CLOCK_DIV_7
		   : DPLLB_LVDS_P2_CLOCK_DIV_14)
		| (0x10000 << (pixel_p1 - 1))
		| ((USE_SSC ? 3 : 0) << 13)
		| (0x1 << (pixel_p1 - 1)));
	mdelay(1);
	write32(pmmio + DPLL(1),
		DPLL_VCO_ENABLE | DPLLB_MODE_LVDS
		| (IS_DUAL_CHANNEL ? DPLLB_LVDS_P2_CLOCK_DIV_7
		   : DPLLB_LVDS_P2_CLOCK_DIV_14)
		| (0x10000 << (pixel_p1 - 1))
		| ((USE_SSC ? 3 : 0) << 13)
		| (0x1 << (pixel_p1 - 1)));

	edid.bytes_per_line = (edid.bytes_per_line + 63) & ~63;
	write32(pmmio + DSPADDR(0), 0x20000);
	write32(pmmio + DSPSURF(0), 0);
	write32(pmmio + DSPSIZE(0), (hactive - 1) | ((vactive - 1) << 16));
	write32(pmmio + DSPSTRIDE(0), edid.bytes_per_line);
	write32(pmmio + DSPSTRIDE(0) + 4, 0);
	write32(pmmio + DSPCNTR(0), DISPLAY_PLANE_ENABLE | DISPPLANE_BGRX888
		| DISPPLANE_SEL_PIPE_B | DISPPLANE_GAMMA_ENABLE);
	mdelay(1);

	write32(pmmio + PIPECONF(1), (1 << 31));
	write32(pmmio + LVDS,
		LVDS_PORT_ENABLE
		| (hpolarity << 20) | (vpolarity << 21)
		| (IS_DUAL_CHANNEL ? LVDS_CLOCK_B_POWERUP_ALL
		   | LVDS_CLOCK_BOTH_POWERUP_ALL : 0)
		| LVDS_CLOCK_A_POWERUP_ALL | (1 << 30));

	write32(pmmio + PP_CONTROL, PANEL_UNLOCK_REGS | PANEL_POWER_OFF);
	write32(pmmio + PP_CONTROL, PANEL_UNLOCK_REGS | PANEL_POWER_RESET);
	mdelay(1);
	write32(pmmio + PP_CONTROL, PANEL_UNLOCK_REGS
		| PANEL_POWER_ON | PANEL_POWER_RESET);

	printk (BIOS_DEBUG, "waiting for panel powerup\n");
	while (1) {
		u32 reg32;
		reg32 = read32(pmmio + PP_STATUS);
		if (((reg32 >> 28) & 3) == 0)
			break;
	}
	printk (BIOS_DEBUG, "panel powered up\n");

	write32(pmmio + PP_CONTROL, PANEL_POWER_ON | PANEL_POWER_RESET);

	/* Clear interrupts. */
	write32(pmmio + DEIIR, 0xffffffff);
	write32(pmmio + SDEIIR, 0xffffffff);
	write32(pmmio + IIR, 0xffffffff);
	write32(pmmio + IMR, 0xffffffff);
	write32(pmmio + IER, 0xffffffff);
	write32(pmmio + EMR, 0xffffffff);

	verbose = 0;
	/* GTT is the Global Translation Table for the graphics pipeline.
	 * It is used to translate graphics addresses to physical
	 * memory addresses. As in the CPU, GTTs map 4K pages.
	 * There are 32 bits per pixel, or 4 bytes,
	 * which means 1024 pixels per page.
	 * There are 4250 GTTs on Link:
	 * 2650 (X) * 1700 (Y) pixels / 1024 pixels per page.
	 * The setgtt function adds a further bit of flexibility:
	 * it allows you to set a range (the first two parameters) to point
	 * to a physical address (third parameter);the physical address is
	 * incremented by a count (fourth parameter) for each GTT in the
	 * range.
	 * Why do it this way? For ultrafast startup,
	 * we can point all the GTT entries to point to one page,
	 * and set that page to 0s:
	 * memset(physbase, 0, 4096);
	 * setgtt(0, 4250, physbase, 0);
	 * this takes about 2 ms, and is a win because zeroing
	 * the page takes a up to 200 ms. We will be exploiting this
	 * trick in a later rev of this code.
	 * This call sets the GTT to point to a linear range of pages
	 * starting at physbase.
	 */

	if (gtt_setup(pmmio)) {
		printk(BIOS_ERR, "ERROR: GTT Setup Failed!!!\n");
		return 0;
	}

	setgtt(0, 800 , physbase, 4096);

	temp = READ32(PGETLB_CTL);
	printk(BIOS_INFO, "GTT PGETLB_CTL register: 0x%lx\n", temp);

	if (temp & 1)
		printk(BIOS_INFO, "GTT Enabled\n");
	else
		printk(BIOS_ERR, "ERROR: GTT is still Disabled!!!\n");

	printk(BIOS_SPEW, "memset %p to 0x00 for %d bytes\n",
		(void *)graphics, FRAME_BUFFER_BYTES);
	memset((void *)graphics, 0x00, FRAME_BUFFER_BYTES);

	printk(BIOS_SPEW, "%ld microseconds\n", globalmicroseconds());

	set_vbe_mode_info_valid(&edid, graphics);

	return 0;
}
