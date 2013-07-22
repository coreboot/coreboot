/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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
#include "hda_verb.h"
#include <smbios.h>
#include <device/pci.h>
#include <ec/google/chromeec/ec.h>
#include <cbfs_core.h>

#include <cpu/x86/tsc.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <edid.h>
#include <drivers/intel/gma/i915.h>
/*
 * Here is the rough outline of how we bring up the display:
 *  1. Upon power-on Sink generates a hot plug detection pulse thru HPD
 *  2. Source determines video mode by reading DPCD receiver capability field
 *     (DPCD 00000h to 0000Dh) including eDP CP capability register (DPCD
 *     0000Dh).
 *  3. Sink replies DPCD receiver capability field.
 *  4. Source starts EDID read thru I2C-over-AUX.
 *  5. Sink replies EDID thru I2C-over-AUX.
 *  6. Source determines link configuration, such as MAX_LINK_RATE and
 *     MAX_LANE_COUNT. Source also determines which type of eDP Authentication
 *     method to use and writes DPCD link configuration field (DPCD 00100h to
 *     0010Ah) including eDP configuration set (DPCD 0010Ah).
 *  7. Source starts link training. Sink does clock recovery and equalization.
 *  8. Source reads DPCD link status field (DPCD 00200h to 0020Bh).
 *  9. Sink replies DPCD link status field. If main link is not stable, Source
 *     repeats Step 7.
 * 10. Source sends MSA (Main Stream Attribute) data. Sink extracts video
 *     parameters and recovers stream clock.
 * 11. Source sends video data.
 */

/* how many bytes do we need for the framebuffer?
 * Well, this gets messy. To get an exact answer, we have
 * to ask the panel, but we'd rather zero the memory
 * and set up the gtt while the panel powers up. So,
 * we take a reasonable guess, secure in the knowledge that the
 * MRC has to overestimate the number of bytes used.
 * 8 MiB is a very safe guess. There may be a better way later, but
 * fact is, the initial framebuffer is only very temporary. And taking
 * a little long is ok; this is done much faster than the AUX
 * channel is ready for IO.
 */
#define FRAME_BUFFER_BYTES (8*MiB)
/* how many 4096-byte pages do we need for the framebuffer?
 * There are hard ways to get this, and easy ways:
 * there are FRAME_BUFFER_BYTES/4096 pages, since pages are 4096
 * on this chip (and in fact every Intel graphics chip we've seen).
 */
#define FRAME_BUFFER_PAGES (FRAME_BUFFER_BYTES/(4096))

static unsigned int *mmio;
static unsigned int graphics;
static unsigned short addrport;
static unsigned short dataport;
static unsigned int physbase;
extern int oprom_is_loaded;

static int ioread = 0, iowrite = 0;

void ug1(int);
void ug2(int);
void ug22(int);
void ug3(int);

unsigned long io_i915_read32(unsigned long addr)
{
	unsigned long val;
	outl(addr, addrport);
	val = inl(dataport);
	ioread += 2;
	return val;
}

void io_i915_write32(unsigned long val, unsigned long addr)
{
	outl(addr, addrport);
	outl(val, dataport);
	iowrite += 2;
}

/* GTT is the Global Translation Table for the graphics pipeline.
 * It is used to translate graphics addresses to physical
 * memory addresses. As in the CPU, GTTs map 4K pages.
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
 * the page takes a up to 200 ms.
 * This call sets the GTT to point to a linear range of pages
 * starting at physbase.
 */

static void
setgtt(int start, int end, unsigned long base, int inc)
{
	int i;

	for(i = start; i < end; i++){
		u32 word = base + i*inc;
		/* note: we've confirmed by checking
		 * the values that mrc does no
		 * useful setup before we run this.
		 */
		io_i915_write32(word|1,(i*4)|1);
	}
}

static int i915_init_done = 0;

/* fill the palette. */
static void palette(void)
{
	int i;
	unsigned long color = 0;

	for(i = 0; i < 256; i++, color += 0x010101){
		io_i915_write32(color, _LGC_PALETTE_A + (i<<2));
	}
}

/* this is code known to be needed for FUI, and useful
 * but not essential otherwise. At some point, we hope,
 * it's always going to be on. It gets the chip
 * into a known good state
 * -- including turning on the power well --
 * which we're not sure is being done correctly.
 * Does it belong here? It belongs somewhere in the
 * northbridge, that we know.
 */
static void gma_fui_init(int noisy)
{
	printk(BIOS_SPEW, "pci dev(0x0,0x2,0x0,0x8)");
	printk(BIOS_SPEW, "pci dev(0x0,0x1f,0x0,0x10)");
	printk(BIOS_SPEW, "pci dev(0x0,0x2,0x0,0x0)");
	printk(BIOS_SPEW, "pci dev(0x0,0x2,0x0,0x2)");
	printk(BIOS_SPEW, "pci dev(0x0,0x1f,0x0,0x0)");
	printk(BIOS_SPEW, "pci dev(0x0,0x1f,0x0,0x2)");
	io_i915_write32(0x80000000,0x45400);
	intel_dp_wait_reg(0x00045400, 0xc0000000);
	printk(BIOS_SPEW, "pci dev(0x0,0x0,0x0,0x14)");
	printk(BIOS_SPEW, "pci dev(0x0,0x2,0x0,0x17)");
	printk(BIOS_SPEW, "pci dev(0x0,0x2,0x0,0x18)");
	io_i915_write32(0x00000000,_CURACNTR);
	io_i915_write32((/* DISPPLANE_SEL_PIPE(0=A,1=B) */0x0<<24)|0x00000000,_DSPACNTR);
	io_i915_write32(0x00000000,_DSPBCNTR);
	io_i915_write32(0x8000298e,CPU_VGACNTRL);
	io_i915_write32(0x00000000,_DSPASIZE+0xc);
	io_i915_write32(0x00000000,_DSPBSURF);
	io_i915_write32(0x00000000,0x4f008);
	io_i915_write32(0x00000000,0x4f008);
	io_i915_write32(0x00000000,0x4f008);
	io_i915_write32(0x01000001,0x4f040);
	io_i915_write32(0x00000000,0x4f044);
	io_i915_write32(0x00000000,0x4f048);
	io_i915_write32(0x03030000,0x4f04c);
	io_i915_write32(0x00000000,0x4f050);
	io_i915_write32(0x00000001,0x4f054);
	io_i915_write32(0x00000000,0x4f058);
	io_i915_write32(0x03450000,0x4f04c);
	io_i915_write32(0x45450000,0x4f04c);
	io_i915_write32(0x03000400,0x4f000);
	io_i915_write32( DP_LINK_TRAIN_PAT_1 | DP_LINK_TRAIN_PAT_1_CPT | DP_VOLTAGE_0_4 | DP_PRE_EMPHASIS_0 | DP_PORT_WIDTH_1 | DP_PLL_FREQ_270MHZ | DP_SCRAMBLING_DISABLE_IRONLAKE | DP_SYNC_VS_HIGH |0x00000091,DP_A);
	io_i915_write32(0x00200090,_FDI_RXA_MISC);
	io_i915_write32(0x0a000000,_FDI_RXA_MISC);
	io_i915_write32(0x00000070,0x46408);
	io_i915_write32(0x04000000,0x42090);
	io_i915_write32(0xc0000000,0x4f050);
	io_i915_write32(0x00000000,0x9840);
	io_i915_write32(0xa4000000,0x42090);
	io_i915_write32(0x00001000,SOUTH_DSPCLK_GATE_D);
	io_i915_write32(0x00004000,0x42080);
	io_i915_write32(0x00ffffff,0x64f80);
	io_i915_write32(0x0007000e,0x64f84);
	io_i915_write32(0x00d75fff,0x64f88);
	io_i915_write32(0x000f000a,0x64f8c);
	io_i915_write32(0x00c30fff,0x64f90);
	io_i915_write32(0x00060006,0x64f94);
	io_i915_write32(0x00aaafff,0x64f98);
	io_i915_write32(0x001e0000,0x64f9c);
	io_i915_write32(0x00ffffff,0x64fa0);
	io_i915_write32(0x000f000a,0x64fa4);
	io_i915_write32(0x00d75fff,0x64fa8);
	io_i915_write32(0x00160004,0x64fac);
	io_i915_write32(0x00c30fff,0x64fb0);
	io_i915_write32(0x001e0000,0x64fb4);
	io_i915_write32(0x00ffffff,0x64fb8);
	io_i915_write32(0x00060006,0x64fbc);
	io_i915_write32(0x00d75fff,0x64fc0);
	io_i915_write32(0x001e0000,0x64fc4);
	io_i915_write32(0x00ffffff,DDI_BUF_TRANS_A);
	io_i915_write32(0x0006000e,DDI_BUF_TRANS_A+0x4);
	io_i915_write32(0x00d75fff,DDI_BUF_TRANS_A+0x8);
	io_i915_write32(0x0005000a,DDI_BUF_TRANS_A+0xc);
	io_i915_write32(0x00c30fff,DDI_BUF_TRANS_A+0x10);
	io_i915_write32(0x00040006,DDI_BUF_TRANS_A+0x14);
	io_i915_write32(0x80aaafff,DDI_BUF_TRANS_A+0x18);
	io_i915_write32(0x000b0000,DDI_BUF_TRANS_A+0x1c);
	io_i915_write32(0x00ffffff,DDI_BUF_TRANS_A+0x20);
	io_i915_write32(0x0005000a,DDI_BUF_TRANS_A+0x24);
	io_i915_write32(0x00d75fff,DDI_BUF_TRANS_A+0x28);
	io_i915_write32(0x000c0004,DDI_BUF_TRANS_A+0x2c);
	io_i915_write32(0x80c30fff,DDI_BUF_TRANS_A+0x30);
	io_i915_write32(0x000b0000,DDI_BUF_TRANS_A+0x34);
	io_i915_write32(0x00ffffff,DDI_BUF_TRANS_A+0x38);
	io_i915_write32(0x00040006,DDI_BUF_TRANS_A+0x3c);
	io_i915_write32(0x80d75fff,DDI_BUF_TRANS_A+0x40);
	io_i915_write32(0x000b0000,DDI_BUF_TRANS_A+0x44);
	io_i915_write32( DIGITAL_PORTA_HOTPLUG_ENABLE |0x00000010,DIGITAL_PORT_HOTPLUG_CNTRL);
	io_i915_write32( PORTD_HOTPLUG_ENABLE | PORTB_HOTPLUG_ENABLE |0x10100010,SDEISR+0x30);
	io_i915_write32((PCH_PP_UNLOCK&0xabcd0000)| EDP_FORCE_VDD |0xabcd0008,PCH_PP_CONTROL);
	mdelay(200);
	io_i915_write32(0x0004af06,PCH_PP_DIVISOR);
	/* we may need more but let's see. */
}

void dp_init_dim_regs(struct intel_dp *dp);
void dp_init_dim_regs(struct intel_dp *dp)
{
	struct edid *edid = &(dp->edid);

	dp->bytes_per_pixel = edid->bpp / 8;

	dp->stride = edid->bytes_per_line;

	dp->htotal = (edid->ha - 1) | ((edid->ha + edid->hbl - 1) << 16);

	dp->hblank = (edid->ha - 1) | ((edid->ha + edid->hbl - 1) << 16);

	dp->hsync = (edid->ha + edid->hso - 1) |
		((edid->ha + edid->hso + edid->hspw - 1) << 16);

	dp->vtotal = (edid->va - 1) | ((edid->va + edid->vbl - 1) << 16);

	dp->vblank = (edid->va - 1) | ((edid->va + edid->vbl - 1) << 16);

	dp->vsync = (edid->va + edid->vso - 1) |
		((edid->va + edid->vso + edid->vspw - 1) << 16);

	/* PIPEASRC is wid-1 x ht-1 */
	dp->pipesrc = (edid->ha-1)<<16 | (edid->va-1);

	dp->pfa_pos = 0;

	dp->pfa_ctl = 0x80800000;

	dp->pfa_sz = (edid->ha << 16) | (edid->va);

	intel_dp_compute_m_n(dp->bpp,
			     dp->lane_count,
			     dp->edid.pixel_clock,
			     dp->edid.link_clock,
			     &dp->m_n);

	printk(BIOS_SPEW, "dp->stride  = 0x%08x\n",dp->stride);
	printk(BIOS_SPEW, "dp->htotal  = 0x%08x\n", dp->htotal);
	printk(BIOS_SPEW, "dp->hblank  = 0x%08x\n", dp->hblank);
	printk(BIOS_SPEW, "dp->hsync   = 0x%08x\n", dp->hsync);
	printk(BIOS_SPEW, "dp->vtotal  = 0x%08x\n", dp->vtotal);
	printk(BIOS_SPEW, "dp->vblank  = 0x%08x\n", dp->vblank);
	printk(BIOS_SPEW, "dp->vsync   = 0x%08x\n", dp->vsync);
	printk(BIOS_SPEW, "dp->pipesrc = 0x%08x\n", dp->pipesrc);
	printk(BIOS_SPEW, "dp->pfa_pos = 0x%08x\n", dp->pfa_pos);
	printk(BIOS_SPEW, "dp->pfa_ctl = 0x%08x\n", dp->pfa_ctl);
	printk(BIOS_SPEW, "dp->pfa_sz  = 0x%08x\n", dp->pfa_sz);
	printk(BIOS_SPEW, "dp->link_m  = 0x%08x\n", dp->m_n.link_m);
	printk(BIOS_SPEW, "dp->link_n  = 0x%08x\n", dp->m_n.link_n);
	printk(BIOS_SPEW, "0x6f030     = 0x%08x\n", TU_SIZE(dp->m_n.tu) | dp->m_n.gmch_m);
	printk(BIOS_SPEW, "0x6f030     = 0x%08x\n", dp->m_n.gmch_m);
	printk(BIOS_SPEW, "0x6f034     = 0x%08x\n", dp->m_n.gmch_n);
}

int intel_dp_bw_code_to_link_rate(u8 link_bw);

int intel_dp_bw_code_to_link_rate(u8 link_bw)
{
	switch (link_bw) {
        case DP_LINK_BW_1_62:
        default:
		return 162000;
        case DP_LINK_BW_2_7:
		return 270000;
        case DP_LINK_BW_5_4:
                return 540000;
	}
}

int i915lightup(unsigned int physbase, unsigned int iobase, unsigned int mmio,
		unsigned int gfx);

int i915lightup(unsigned int pphysbase, unsigned int piobase,
		unsigned int pmmio, unsigned int pgfx)
{
	int must_cycle_power = 0;
	struct intel_dp adp, *dp = &adp;
	/* frame buffer pointer */
	/* u32 *l; */
	int i;
	int edid_ok;
	/* u32 tcolor = 0xff; */
	int pixels = FRAME_BUFFER_BYTES/64;

	mmio = (void *)pmmio;
	addrport = piobase;
	dataport = addrport + 4;
	physbase = pphysbase;
	graphics = pgfx;
	printk(BIOS_SPEW,
	       "i915lightup: graphics %p mmio %p"
	       "addrport %04x physbase %08x\n",
	       (void *)graphics, mmio, addrport, physbase);

	void runio(struct intel_dp *dp);
	void runlinux(struct intel_dp *dp);
	dp->gen = 8; // ??
	dp->is_haswell = 1;
	dp->DP = 0x2;
	/* These values are used for training the link */
	dp->lane_count = 2;
	dp->link_bw = DP_LINK_BW_2_7;
	dp->panel_power_down_delay = 600;
	dp->panel_power_up_delay = 200;
	dp->panel_power_cycle_delay = 600;
	dp->pipe = 0;
	dp->port = 0;
	dp->clock = 160000;
	dp->bpp = 32;
	dp->type = INTEL_OUTPUT_EDP;
	dp->output_reg = DP_A;
	/* observed from YABEL. */
	dp->aux_clock_divider = 0xe1;
	dp->precharge = 3;

	gma_fui_init(0);
	//intel_prepare_ddi_buffers(0, 0);
	//ironlake_edp_panel_vdd_on(dp);
	dp->address = 0x50;

	if ( !intel_dp_get_dpcd(dp) )
		goto fail;

	intel_dp_i2c_aux_ch(dp, MODE_I2C_WRITE, 0, NULL);
	for(dp->edidlen = i = 0; i < sizeof(dp->rawedid); i++){
		if (intel_dp_i2c_aux_ch(dp, MODE_I2C_READ,
					0x50, &dp->rawedid[i]) < 0)
			break;
		dp->edidlen++;
	}

	edid_ok = decode_edid(dp->rawedid, dp->edidlen, &dp->edid);
	printk(BIOS_SPEW, "decode edid returns %d\n", edid_ok);

	dp->edid.link_clock = intel_dp_bw_code_to_link_rate(dp->link_bw);

	printk(BIOS_SPEW, "pixel_clock is %i, link_clock is %i\n",dp->edid.pixel_clock, dp->edid.link_clock);

	dp_init_dim_regs(dp);

	/* more undocumented stuff. */
	/* possibly not even needed. */
	io_i915_write32(0x00000021,0x6f410);

	runio(dp);
	palette();

	setgtt(0, FRAME_BUFFER_PAGES, physbase, 4096);

	pixels = dp->edid.ha * (dp->edid.va-4) * 4;
	printk(BIOS_SPEW, "ha=%d, va=%d\n",dp->edid.ha, dp->edid.va);

	/* for (i = 0; i < (dp->edid.va - 4); i++) { */
	/* 	int j; */
	/* 	tcolor = 0x0ff; */
	/* 	for (j = 0; j < (dp->edid.ha-4); j++) { */
	/* 		if (j == (dp->edid.ha/2)) { */
	/* 			tcolor = 0xff00; */
	/* 		} */
	/* 		l = (u32*)(graphics + i * dp->stride + j * sizeof(tcolor)); */
	/* 		memcpy(l,&tcolor,sizeof(tcolor)); */
	/* 	} */
	/* }	 */

	set_vbe_mode_info_valid(&dp->edid, graphics);
	i915_init_done = 1;
	oprom_is_loaded = 1;
	//io_i915_write32( 0x80000000,BLC_PWM_CPU_CTL2);
	//io_i915_write32( 0x80000000,BLC_PWM_PCH_CTL1);
	return 1;

fail:
	printk(BIOS_SPEW, "Graphics could not be started;");
	if (0 && must_cycle_power){
		printk(BIOS_SPEW, "Turn off power and wait ...");
		io_i915_write32(0xabcd0000, PCH_PP_CONTROL);
		udelay(600000);
		io_i915_write32(0xabcd000f, PCH_PP_CONTROL);
	}
	printk(BIOS_SPEW, "Returning.\n");
	return 0;
}
