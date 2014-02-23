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
#include <drivers/intel/gma/edid.h>
#include <drivers/intel/gma/i915.h>

#define  LVDS_DETECTED	(1 << 1)
#define  LVDS_BORDER_ENABLE	(1 << 15)
#define  LVDS_PORT_ENABLE	(1 << 31)
#define  LVDS_CLOCK_A_POWERUP_ALL	(3 << 8)
#define  LVDS_CLOCK_B_POWERUP_ALL	(3 << 4)
#define  LVDS_CLOCK_BOTH_POWERUP_ALL	(3 << 2)
#define   DISPPLANE_BGRX888			(0x6<<26)
#define   DPLLB_LVDS_P2_CLOCK_DIV_7	(1 << 24) /* i915 */

#define PGETLB_CTL 0x2020

static int verbose = 0;

static unsigned int *mmio;
static unsigned int graphics;
static unsigned short addrport;
static unsigned short dataport;
static unsigned int physbase;

#define PGETBL_CTL	0x2020
#define PGETBL_ENABLED	0x00000001


#define READ32(addr) io_i915_READ32(addr)
#define WRITE32(val, addr) io_i915_WRITE32(val, addr)

static char *regname(unsigned long addr)
{
	static char name[16];
	snprintf(name, sizeof (name), "0x%lx", addr);
	return name;
}

static unsigned long io_i915_READ32(unsigned long addr)
{
	unsigned long val;
	outl(addr, addrport);
	val = inl(dataport);
	if (verbose & vio)
		printk(BIOS_SPEW, "%s: Got %08lx\n", regname(addr), val);
	return val;
}

static void io_i915_WRITE32(unsigned long val, unsigned long addr)
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

int i915lightup(unsigned int physbase, unsigned int iobase, unsigned int mmio,
		unsigned int gfx);

int i915lightup(unsigned int pphysbase, unsigned int piobase,
		unsigned int pmmio, unsigned int pgfx)
{
	struct edid edid;
	u8 x60_edid_data[256];
	unsigned long temp;
	int hpolarity, vpolarity;
	u32 candp1, candn;
	u32 best_delta = 0xffffffff;
	u32 target_frequency;
	u32 pixel_p1 = 1;
	u32 pixel_n = 1;
	u32 pixel_m1 = 1;
	u32 pixel_m2 = 1;
	u32 hactive, vactive;

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

#define IS_DUAL_CHANNEL 0
#define USE_SSC 1

	read32(pmmio+PP_ON_DELAYS); // 0x00000000
	read32(pmmio+PP_OFF_DELAYS); // 0x00000000
	write32(pmmio+PP_ON_DELAYS,0x00000000);
	write32(pmmio+PP_OFF_DELAYS,0x00000000);
	write32(pmmio+INSTPM+0x24, MI_ARB_C3_LP_WRITE_ENABLE | 0x08000800);
	write32(pmmio+RENDER_RING_BASE,0x00000000);
	write32(pmmio+RENDER_RING_BASE+0x4,0x00000000);
	write32(pmmio+RENDER_RING_BASE+0x8,0x00000000);
	write32(pmmio+RENDER_RING_BASE+0xc,0x00000000);
	write32(pmmio+RENDER_RING_BASE+0x10,0x00000000);
	write32(pmmio+RENDER_RING_BASE+0x14,0x00000000);
	write32(pmmio+RENDER_RING_BASE+0x18,0x00000000);
	write32(pmmio+RENDER_RING_BASE+0x1c,0x00000000);
	write32(pmmio+FENCE_REG_965_0,0x00000000);
	write32(pmmio+FENCE_REG_965_0+0x4,0x00000000);
	write32(pmmio+FENCE_REG_965_0+0x8,0x00000000);
	write32(pmmio+FENCE_REG_965_0+0xc,0x00000000);
	write32(pmmio+FENCE_REG_965_0+0x10,0x00000000);
	write32(pmmio+FENCE_REG_965_0+0x14,0x00000000);
	write32(pmmio+FENCE_REG_965_0+0x18,0x00000000);
	write32(pmmio+FENCE_REG_965_0+0x1c,0x00000000);
	write32(pmmio+VGACNTRL, VGA_DISP_DISABLE | 0x80000000);
	read32(pmmio+PFIT_CONTROL); // 0x00000000
	read32(pmmio+PP_CONTROL); // 0x00000000
	write32(pmmio+PP_CONTROL,0xabcd0000);
	read32(pmmio+BLC_PWM_CTL); // 0x00000000
	read32(pmmio+DVOB); // 0x00300000
	read32(pmmio+DVOC); // 0x00300000
	read32(pmmio+TV_CTL); // 0x00000000
	read32(pmmio+TV_DAC); // 0x00000000
	write32(pmmio+TV_DAC,0x08000000);
	read32(pmmio+TV_DAC); // 0x08000000
	write32(pmmio+TV_DAC,0x00000000);
	read32(pmmio+TV_DAC); // 0x00000000
	write32(pmmio+TV_DAC,0x00000000);
	write32(pmmio+HWS_PGA,0x352d2000);
	write32(pmmio+PRB0_CTL,0x00000000);
	write32(pmmio+PRB0_HEAD,0x00000000);
	write32(pmmio+PRB0_TAIL,0x00000000);
	read32(pmmio+PRB0_HEAD); // 0x00000000
	write32(pmmio+PRB0_START,0x00000000);
	write32(pmmio+PRB0_CTL,0x0001f001);
	read32(pmmio+PRB0_CTL); // 0x0001f001
	read32(pmmio+PRB0_START); // 0x00000000
	read32(pmmio+PRB0_HEAD); // 0x00000000
	read32(pmmio+PRB0_TAIL); // 0x00000000
	read32(pmmio+0x6104); // 0x00000000
	write32(pmmio+0x6104,0x0000000b);
	write32(pmmio+ECOSKPD,0x00010000);
	read32(pmmio+_PIPEBCONF); // 0x00000000
	read32(pmmio+LVDS); // 0x40000000
	read32(pmmio+_FDI_TXB_CTL); // 0x00000000
	read32(pmmio+TV_CTL); // 0x00000000
	read32(pmmio+LVDS); // 0x40000000
	read32(pmmio+_FDI_TXB_CTL); // 0x00000000
	read32(pmmio+TV_CTL); // 0x00000000
	read32(pmmio+_DSPACNTR); // 0x00000000
	read32(pmmio+_PIPEBCONF); // 0x00000000
	write32(pmmio+_PIPEBCONF,0x00000000);
	read32(pmmio+_DSPBCNTR); // 0x00000000
	read32(pmmio+LVDS); // 0x40000000
	read32(pmmio+_FDI_TXB_CTL); // 0x00000000
	read32(pmmio+TV_CTL); // 0x00000000
	read32(pmmio+LVDS); // 0x40000000
	read32(pmmio+_FDI_TXB_CTL); // 0x00000000
	read32(pmmio+TV_CTL); // 0x00000000
	read32(pmmio+_PIPEBCONF); // 0x00000000
	write32(pmmio+PORT_HOTPLUG_EN,0x00000000);
	read32(pmmio+PORT_HOTPLUG_STAT); // 0x00000000
	write32(pmmio+PORT_HOTPLUG_STAT,0x00000000);
	write32(pmmio+HWSTAM,0x0000effe);
	write32(pmmio+_PIPEASTAT,0x00000000);
	write32(pmmio+_PIPEBSTAT,0x00000000);
	read32(pmmio+PORT_HOTPLUG_EN); // 0x00000000
	write32(pmmio+PORT_HOTPLUG_EN, CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000220);
	read32(pmmio+PORT_HOTPLUG_EN); //  CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000220
	write32(pmmio+PORT_HOTPLUG_EN, CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_FORCE_DETECT | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000228);
	read32(pmmio+PORT_HOTPLUG_EN); //  CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_FORCE_DETECT | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000228
	read32(pmmio+PORT_HOTPLUG_EN); //  CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000220
	read32(pmmio+PORT_HOTPLUG_STAT); // 0x00000000
	write32(pmmio+PORT_HOTPLUG_STAT, CRT_HOTPLUG_INT_STATUS | 0x00000800);
	write32(pmmio+PORT_HOTPLUG_EN, CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000220);
	write32(pmmio+_DSPBSIZE,0x03ff04ff);
	write32(pmmio+_DSPBPOS,0x00000000);
	write32(pmmio+_PIPEACONF,0x00000000);
	write32(pmmio+_DSPBCNTR,0x40000000);
	read32(pmmio+_DSPBCNTR); // 0x40000000
	write32(pmmio+_DSPBCNTR,0x58000000);
	write32(pmmio+_DSPBADDR,0x00020000);
	read32(pmmio+DSPARB); // (DSPARB_CSTART_SHIFT & 0x4) | 0x00001d9c
	write32(pmmio+FW_BLC_SELF, FW_BLC_SELF_EN_MASK | 0x80000000);
	write32(pmmio+FW_BLC,0x011d011a);
	write32(pmmio+FW_BLC2,0x00000102);
	read32(pmmio+TV_CTL); // 0x00000000
	write32(pmmio+TV_H_CTL_1,0x00400359);
	write32(pmmio+TV_H_CTL_2,0x80480022);
	write32(pmmio+TV_H_CTL_3,0x007c0344);
	write32(pmmio+TV_V_CTL_1,0x00f01415);
	write32(pmmio+TV_V_CTL_2,0x00060607);
	write32(pmmio+TV_V_CTL_3,0x80120001);
	write32(pmmio+TV_V_CTL_4,0x000900f0);
	write32(pmmio+TV_V_CTL_5,0x000a00f0);
	write32(pmmio+TV_V_CTL_6,0x000900f0);
	write32(pmmio+TV_V_CTL_7,0x000a00f0);
	write32(pmmio+TV_SC_CTL_1,0xc1710087);
	write32(pmmio+TV_SC_CTL_2,0x6b405140);
	write32(pmmio+TV_SC_CTL_3,0x00000000);
	write32(pmmio+TV_CSC_Y,0x0332012d);
	write32(pmmio+TV_CSC_Y2,0x07d30104);
	write32(pmmio+TV_CSC_U,0x0733052d);
	write32(pmmio+TV_CSC_U2,0x05c70200);
	write32(pmmio+TV_CSC_V,0x0340030c);
	write32(pmmio+TV_CSC_V2,0x06d00200);
	write32(pmmio+TV_CLR_KNOBS,0x00606000);
	write32(pmmio+TV_CLR_LEVEL,0x010b00e1);
	write32(pmmio+_PFA_CTL_1,0x00000000);
	write32(pmmio+_PFA_WIN_POS,0x00000000);
	write32(pmmio+_PFA_WIN_SZ,0x00000000);
	read32(pmmio+TV_DAC); // 0x70000000
	write32(pmmio+TV_DAC,0x00000000);
	write32(pmmio+TV_CTL,0x000c0000);
	read32(pmmio+DSPARB); // (DSPARB_CSTART_SHIFT & 0x4) | 0x00001d9c
	write32(pmmio+FW_BLC_SELF, FW_BLC_SELF_EN_MASK | 0x80000000);
	write32(pmmio+FW_BLC_SELF,0x0001002f);
	write32(pmmio+FW_BLC,0x0101011a);
	write32(pmmio+FW_BLC2,0x00000102);
	write32(pmmio+FW_BLC_SELF, FW_BLC_SELF_EN_MASK | 0x80008000);
	read32(pmmio+PP_CONTROL); // 0xabcd0000
	read32(pmmio+LVDS); // 0x40000000
	read32(pmmio+_DSPBCNTR); // 0x58000000
	write32(pmmio+_DSPBCNTR,0xd8000000);
	read32(pmmio+_DSPBADDR); // 0x00020000
	write32(pmmio+_DSPBADDR,0x00020000);
	read32(pmmio+_PIPEASTAT); //  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203
	write32(pmmio+_PIPEASTAT, PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203);
	read32(pmmio+_PIPEASTAT); // 0x00000000
	read32(pmmio+_PIPEASTAT); //  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203
	read32(pmmio+TV_CTL); // 0x000c0000
	write32(pmmio+TV_CTL,0x800c0000);
	read32(pmmio+LVDS); // 0x40000000
	read32(pmmio+_FDI_TXB_CTL); // 0x00000000
	read32(pmmio+TV_CTL); // 0x800c0000
	read32(pmmio+LVDS); // 0x40000000
	read32(pmmio+_FDI_TXB_CTL); // 0x00000000
	read32(pmmio+TV_CTL); // 0x800c0000
	read32(pmmio+_PIPEBCONF); // 0x00000000
	read32(pmmio+_PIPEASTAT); //  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203
	write32(pmmio+_PIPEASTAT, PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203);
	read32(pmmio+_PIPEASTAT); // 0x00000000
	read32(pmmio+_PIPEASTAT); //  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203
	read32(pmmio+TV_DAC); // 0x70000000
	read32(pmmio+TV_CTL); // 0x800c0000
	write32(pmmio+TV_CTL,0x000c0007);
	write32(pmmio+TV_DAC,0x0f0000aa);
	read32(pmmio+_PIPEASTAT); //  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203
	write32(pmmio+_PIPEASTAT, PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203);
	read32(pmmio+_PIPEASTAT); // 0x00000000
	read32(pmmio+_PIPEASTAT); //  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203
	read32(pmmio+TV_DAC); // 0x7f0000aa
	write32(pmmio+TV_DAC,0x70000000);
	write32(pmmio+TV_CTL,0x800c0000);
	read32(pmmio+_PIPEASTAT); //  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203
	write32(pmmio+_PIPEASTAT, PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203);
	read32(pmmio+_PIPEASTAT); // 0x00000000
	read32(pmmio+_PIPEASTAT); //  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203
	read32(pmmio+TV_CTL); // 0x800c0000
	write32(pmmio+TV_CTL,0x000c0000);
	write32(pmmio+INSTPM,0x08000800);
	read32(pmmio+_DSPBCNTR); // 0xd8000000
	write32(pmmio+_DSPBCNTR,0x58000000);
	read32(pmmio+_DSPBADDR); // 0x00020000
	write32(pmmio+_DSPBADDR,0x00020000);
	read32(pmmio+_PIPEASTAT); //  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203
	write32(pmmio+_PIPEASTAT, PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203);
	read32(pmmio+_PIPEASTAT); // 0x00000000
	read32(pmmio+_PIPEASTAT); //  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203
	read32(pmmio+_DSPACNTR); // 0x00000000
	read32(pmmio+_DSPBCNTR); // 0x58000000
	read32(pmmio+DSPARB); // (DSPARB_CSTART_SHIFT & 0x4) | 0x00001d9c
	write32(pmmio+FW_BLC_SELF, FW_BLC_SELF_EN_MASK | 0x80000000);
	write32(pmmio+FW_BLC,0x011d011a);
	write32(pmmio+FW_BLC2,0x00000102);
	read32(pmmio+LVDS); // 0x40000000
	read32(pmmio+_FDI_TXB_CTL); // 0x00000000
	read32(pmmio+TV_CTL); // 0x000c0000
	read32(pmmio+LVDS); // 0x40000000
	read32(pmmio+_FDI_TXB_CTL); // 0x00000000
	read32(pmmio+TV_CTL); // 0x000c0000
	read32(pmmio+_PIPEBCONF); // 0x00000000
	read32(pmmio+_PIPEBCONF); // 0x00000000
	read32(pmmio+LVDS); // 0x40000000
	write32(pmmio+_DPLL_B, DPLL_VGA_MODE_DIS | DPLLB_MODE_LVDS | 0x18046000);
	read32(pmmio+LVDS); // 0x40000000
	write32(pmmio+LVDS, LVDS_ON | PLL_P1_DIVIDE_BY_TWO | DISPLAY_RATE_SELECT_FPA1 | 0xc0300300);
	write32(pmmio+_DPLL_B, DPLL_VCO_ENABLE | DPLL_VGA_MODE_DIS | DPLLB_MODE_LVDS | 0x98046000);
	read32(pmmio+_PIPEBCONF); // 0x00000000
	write32(pmmio+_HTOTAL_B,0x053f03ff);
	write32(pmmio+_HBLANK_B,0x053f03ff);
	write32(pmmio+_HSYNC_B,0x049f0417);
	write32(pmmio+_VTOTAL_B,0x032502ff);
	write32(pmmio+_VBLANK_B,0x032502ff);
	write32(pmmio+_VSYNC_B,0x03080302);
	write32(pmmio+_PIPEBSRC,0x03ff02ff);
	write32(pmmio+_DSPASIZE,0x02ff03ff);
	write32(pmmio+_DSPASTRIDE+0x4,0x00000000);
	read32(pmmio+DSPARB); // (DSPARB_CSTART_SHIFT & 0x4) | 0x00001d9c
	write32(pmmio+FW_BLC_SELF, FW_BLC_SELF_EN_MASK | 0x80000000);
	write32(pmmio+FW_BLC,0x011d011a);
	write32(pmmio+FW_BLC2,0x00000102);
	read32(pmmio+DSPARB); // (DSPARB_CSTART_SHIFT & 0x4) | 0x00001d9c
	write32(pmmio+FW_BLC_SELF, FW_BLC_SELF_EN_MASK | 0x80000000);
	write32(pmmio+FW_BLC_SELF,0x0001003f);
	write32(pmmio+FW_BLC,0x011d0109);
	write32(pmmio+FW_BLC2,0x00000102);
	write32(pmmio+FW_BLC_SELF, FW_BLC_SELF_EN_MASK | 0x80008000);
	write32(pmmio+PFIT_PGM_RATIOS,0x00000000);
	write32(pmmio+PFIT_CONTROL,(PFIT_PIPE_SHIFT & 0x8) | 0x00000008);
	read32(pmmio+BLC_PWM_CTL); // 0x00000000
	write32(pmmio+BLC_PWM_CTL,0x00000002);

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
	write32(pmmio + DSPSTRIDE(0), edid.bytes_per_line);
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
		(void *)graphics, hactive * vactive * 4);
	memset((void *)graphics, 0x00, hactive * vactive * 4);

	printk(BIOS_SPEW, "%ld microseconds\n", globalmicroseconds());

	set_vbe_mode_info_valid(&edid, graphics);

	return 0;
}
