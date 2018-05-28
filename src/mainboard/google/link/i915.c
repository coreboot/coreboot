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
#include "onboard.h"
#include "ec.h"
#include <southbridge/intel/bd82x6x/pch.h>
#include <northbridge/intel/sandybridge/gma.h>
#include <smbios.h>
#include <device/pci.h>
#include <ec/google/chromeec/ec.h>

#include <cpu/x86/tsc.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <edid.h>
#include "i915io.h"

enum {
	vmsg = 1, vio = 2, vspin = 4,
};

static int verbose = 0;

static unsigned int *mmio;
static unsigned int graphics;
static unsigned short addrport;
static unsigned short dataport;
static unsigned int physbase;
static u32 htotal, hblank, hsync, vtotal, vblank, vsync;

const u32 link_edid_data[] = {
	0xffffff00, 0x00ffffff, 0x0379e430, 0x00000000,
	0x04011500, 0x96121ba5, 0xa2d54f02, 0x26935259,
	0x00545017, 0x01010000, 0x01010101, 0x01010101,
	0x01010101, 0x6f6d0101, 0xa4a0a000, 0x20306031,
	0xb510003a, 0x19000010, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x4c00fe00,
	0x69442047, 0x616c7073, 0x20200a79, 0xfe000000,
	0x31504c00, 0x45513932, 0x50532d31, 0x24003141,
};

#define READ32(addr) io_i915_READ32(addr)
#define WRITE32(val, addr) io_i915_WRITE32(val, addr)

static char *regname(unsigned long addr)
{
	static char name[16];
	snprintf(name, sizeof(name), "0x%lx", addr);
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

	for(i = start; i < end; i++){
		u32 word = base + i*inc;
		WRITE32(word|1,(i*4)|1);
	}
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

static int i915_init_done = 0;

/* fill the palette. This runs when the P opcode is hit. */
/* and, yes, it's needed for even 32 bits per pixel */
static void palette(void)
{
	int i;
	unsigned long color = 0;

	for(i = 0; i < 256; i++, color += 0x010101){
		io_i915_WRITE32(color, _LGC_PALETTE_A + (i << 2));
	}
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
	for(i = index, id = &iodefs[i]; id->op; i++, id++){
		switch(id->op){
		case M:
			if (verbose & vmsg) printk(BIOS_SPEW, "%ld: %s\n",
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
			if (lastidread->addr == id->addr){
				/* they're going to be polling.
				 * just do it 1000 times
				 */
				for (t = 0; t < 1000 && id->data != u; t++){
					u = READ32(id->addr);
				}
				if (verbose & vspin) printk(BIOS_SPEW,
						"%s: # loops %ld got %08lx want %08lx\n",
						regname(id->addr),
						t, u, id->data);
			}
			lastidread = id;
			break;
		case W:
			WRITE32(id->data, id->addr);
			if (id->addr == PCH_PP_CONTROL){
				if (verbose & vio)
					printk(BIOS_SPEW, "PCH_PP_CONTROL\n");
				switch(id->data & 0xf){
				case 8: break;
				case 7: break;
				default: udelay(100000);
					if (verbose & vio)
						printk(BIOS_SPEW, "U %d\n", 100000);
				}
			}
			break;
		case V:
			if (id->count < 8){
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
			printk(BIOS_SPEW, "BAD TABLE, opcode %d @ %d\n", id->op, i);
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

int i915lightup_sandy(const struct i915_gpu_controller_info *info,
		      u32 pphysbase, u16 piobase, u8 *pmmio, u32 pgfx)
{
	static struct edid edid;
	const struct edid_mode *mode;
	int edid_ok;

	int index;
	u32 auxin[16], auxout[16];
	mmio = (void *)pmmio;
	addrport = piobase;
	dataport = addrport + 4;
	physbase = pphysbase;
	graphics = pgfx;
	printk(BIOS_SPEW, "i915lightup: graphics %p mmio %p"
		"addrport %04x physbase %08x\n",
		(void *)graphics, mmio, addrport, physbase);
	globalstart = rdtscll();


	edid_ok = decode_edid((unsigned char *)&link_edid_data,
			      sizeof(link_edid_data), &edid);
	mode = &edid.mode;
	printk(BIOS_SPEW, "decode edid returns %d\n", edid_ok);
	edid.framebuffer_bits_per_pixel = 32;

	htotal = (mode->ha - 1) | ((mode->ha + mode->hbl - 1) << 16);
	printk(BIOS_SPEW, "I915_WRITE(HTOTAL(pipe), %08x)\n", htotal);

	hblank = (mode->ha - 1) | ((mode->ha + mode->hbl - 1) << 16);
	printk(BIOS_SPEW, "I915_WRITE(HBLANK(pipe),0x%08x)\n", hblank);

	hsync = (mode->ha + mode->hso - 1) |
		((mode->ha + mode->hso + mode->hspw - 1) << 16);
	printk(BIOS_SPEW, "I915_WRITE(HSYNC(pipe),0x%08x)\n", hsync);

	vtotal = (mode->va - 1) | ((mode->va + mode->vbl - 1) << 16);
	printk(BIOS_SPEW, "I915_WRITE(VTOTAL(pipe), %08x)\n", vtotal);

	vblank = (mode->va - 1) | ((mode->va + mode->vbl - 1) << 16);
	printk(BIOS_SPEW, "I915_WRITE(VBLANK(pipe),0x%08x)\n", vblank);

	vsync = (mode->va + mode->vso - 1) |
		((mode->va + mode->vso + mode->vspw - 1) << 16);
	printk(BIOS_SPEW, "I915_WRITE(VSYNC(pipe),0x%08x)\n", vsync);

	printk(BIOS_SPEW, "Table has %d elements\n", niodefs);

	index = run(0);
	printk(BIOS_SPEW, "Run returns %d\n", index);
	auxout[0] = 1 << 31 /* dp */|0x1 << 28/*R*/|DP_DPCD_REV << 8|0xe;
	intel_dp_aux_ch(DPA_AUX_CH_CTL, DPA_AUX_CH_DATA1, auxout, 4, auxin, 14);
	auxout[0] = 0 << 31 /* i2c */|1 << 30|0x0 << 28/*W*/|0x0 << 8|0x0;
	intel_dp_aux_ch(DPA_AUX_CH_CTL, DPA_AUX_CH_DATA1, auxout, 3, auxin, 0);
	index = run(index);
	printk(BIOS_SPEW, "Run returns %d\n", index);
	auxout[0] = 0 << 31 /* i2c */|0 << 30|0x0 << 28/*W*/|0x0 << 8|0x0;
	intel_dp_aux_ch(DPA_AUX_CH_CTL, DPA_AUX_CH_DATA1, auxout, 3, auxin, 0);
	index = run(index);
	printk(BIOS_SPEW, "Run returns %d\n", index);
	auxout[0] = 1 << 31 /* dp */|0x0 << 28/*W*/|DP_SET_POWER << 8|0x0;
	auxout[1] = 0x01000000;
	/* DP_SET_POWER_D0 | DP_PSR_SINK_INACTIVE */
	intel_dp_aux_ch(DPA_AUX_CH_CTL, DPA_AUX_CH_DATA1, auxout, 5, auxin, 0);
	index = run(index);
	auxout[0] = 1 << 31 /* dp */|0x0 << 28/*W*/|DP_LINK_BW_SET << 8|0x8;
	auxout[1] = 0x0a840000;
	/*( DP_LINK_BW_2_7 &0xa)|0x0000840a*/
	auxout[2] = 0x00000000;
	auxout[3] = 0x01000000;
	intel_dp_aux_ch(DPA_AUX_CH_CTL, DPA_AUX_CH_DATA1, auxout, 13, auxin, 0);
	index = run(index);
	auxout[0] = 1 << 31 /* dp */|0x0 << 28/*W*/|DP_TRAINING_PATTERN_SET << 8|0x0;
	auxout[1] = 0x21000000;
	/* DP_TRAINING_PATTERN_1 | DP_LINK_SCRAMBLING_DISABLE |
	 *	DP_SYMBOL_ERROR_COUNT_BOTH |0x00000021*/
	intel_dp_aux_ch(DPA_AUX_CH_CTL, DPA_AUX_CH_DATA1, auxout, 5, auxin, 0);
	index = run(index);
	auxout[0] = 1 << 31 /* dp */|0x0 << 28/*W*/|DP_TRAINING_LANE0_SET << 8|0x3;
	auxout[1] = 0x00000000;
	/* DP_TRAIN_VOLTAGE_SWING_400 | DP_TRAIN_PRE_EMPHASIS_0 |0x00000000*/
	intel_dp_aux_ch(DPA_AUX_CH_CTL, DPA_AUX_CH_DATA1, auxout, 8, auxin, 0);
	index = run(index);
	auxout[0] = 1 << 31 /* dp */|0x1 << 28/*R*/|DP_LANE0_1_STATUS << 8|0x5;
	intel_dp_aux_ch(DPA_AUX_CH_CTL, DPA_AUX_CH_DATA1, auxout, 4, auxin, 5);
	index = run(index);
	auxout[0] = 1 << 31 /* dp */|0x0 << 28/*W*/|DP_TRAINING_PATTERN_SET << 8|0x0;
	auxout[1] = 0x22000000;
	/* DP_TRAINING_PATTERN_2 | DP_LINK_SCRAMBLING_DISABLE |
	 *	DP_SYMBOL_ERROR_COUNT_BOTH |0x00000022*/
	intel_dp_aux_ch(DPA_AUX_CH_CTL, DPA_AUX_CH_DATA1, auxout, 5, auxin, 0);
	index = run(index);
	auxout[0] = 1 << 31 /* dp */|0x0 << 28/*W*/|DP_TRAINING_LANE0_SET << 8|0x3;
	auxout[1] = 0x00000000;
	/* DP_TRAIN_VOLTAGE_SWING_400 | DP_TRAIN_PRE_EMPHASIS_0 |0x00000000*/
	intel_dp_aux_ch(DPA_AUX_CH_CTL, DPA_AUX_CH_DATA1, auxout, 8, auxin, 0);
	index = run(index);
	auxout[0] = 1 << 31 /* dp */|0x1 << 28/*R*/|DP_LANE0_1_STATUS << 8|0x5;
	intel_dp_aux_ch(DPA_AUX_CH_CTL, DPA_AUX_CH_DATA1, auxout, 4, auxin, 5);
	index = run(index);
	auxout[0] = 1 << 31 /* dp */|0x0 << 28/*W*/|DP_TRAINING_PATTERN_SET << 8|0x0;
	auxout[1] = 0x00000000;
	/* DP_TRAINING_PATTERN_DISABLE | DP_LINK_QUAL_PATTERN_DISABLE |
	 *	DP_SYMBOL_ERROR_COUNT_BOTH |0x00000000*/
	intel_dp_aux_ch(DPA_AUX_CH_CTL, DPA_AUX_CH_DATA1, auxout, 5, auxin, 0);
	index = run(index);

	if (index != niodefs)
		printk(BIOS_ERR, "Left over IO work in i915_lightup"
			" -- this is likely a table error. "
			"Only %d of %d were done.\n", index, niodefs);
	printk(BIOS_SPEW, "DONE startup\n");
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
	setgtt(0, FRAME_BUFFER_PAGES, physbase, 4096);
	printk(BIOS_SPEW, "memset %p to 0 for %d bytes\n",
		(void *)graphics, FRAME_BUFFER_BYTES);
	memset((void *)graphics, 0, FRAME_BUFFER_BYTES);
	printk(BIOS_SPEW, "%ld microseconds\n", globalmicroseconds());
	set_vbe_mode_info_valid(&edid, (uintptr_t)graphics);
	i915_init_done = 1;
	return i915_init_done;
}
