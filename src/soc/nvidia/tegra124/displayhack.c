/* this is too ugly to be allowed to live. But it's what works for now. */
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

#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <lib.h>
#include <stdlib.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <device/device.h>
#include <stdlib.h>
#include <string.h>
#include <cpu/cpu.h>
#include <boot/tables.h>
#include <cbmem.h>
#include <soc/nvidia/tegra/dc.h>
#include "clk_rst.h"
#include <soc/clock.h>
#include "chip.h"
#include "sor.h"
#include <soc/display.h>

//#include <soc/nvidia/tegra/displayport.h>
extern int dump;
unsigned long READL(void *p);
void WRITEL(unsigned long value, void *p);
void debug_dpaux_print(u32 addr, u32 size);
int dpaux_write(u32 addr, u32 size, u32 data);
int dpaux_read(u32 addr, u32 size, u8 * data);

#define DCA_WRITE(reg, val) \
	{ \
		WRITEL(val, (void *)(TEGRA_ARM_DISPLAYA + (reg<<2)));	\
	}
#define DCA_READ_M_WRITE(reg, mask, val) \
	{ \
	u32 _reg_val; \
	_reg_val = READL( (void *)(TEGRA_ARM_DISPLAYA + (reg<<2))); \
	_reg_val &= ~mask; \
	_reg_val |= val; \
	WRITEL(_reg_val,  (void *)(TEGRA_ARM_DISPLAYA + (reg<<2))); \
	}

#define SOR_WRITE(reg, val) \
	{ \
	WRITEL(val,  (void *)(TEGRA_ARM_SOR + (reg<<2))); \
	}

#define SOR_READ(reg) READL((void *)(TEGRA_ARM_SOR + (reg<<2)))

#define SOR_READ_M_WRITE(reg, mask, val) \
	{	\
	u32 _reg_val; \
	_reg_val = READL((void *)(TEGRA_ARM_SOR + (reg<<2))); \
	_reg_val &= ~mask;	\
	_reg_val |= val;		\
	WRITEL(_reg_val,  (void *)(TEGRA_ARM_SOR + (reg<<2))); \
	}

#define DPAUX_WRITE(reg, val) \
	{ \
	WRITEL(val,  (void *)(TEGRA_ARM_DPAUX + (reg<<2))); \
	}
#define DPAUX_READ(reg) READL((void *)(TEGRA_ARM_DPAUX + (reg<<2)))

void init_dca_regs(void)
{
	DCA_WRITE (DC_CMD_DISPLAY_WINDOW_HEADER_0, 0x000000F0);
	DCA_WRITE (DC_WIN_A_WIN_OPTIONS_0, 0x00000000);
	DCA_WRITE (DC_WIN_A_BYTE_SWAP_0, 0x00000000);
	DCA_WRITE (DC_WIN_A_BUFFER_CONTROL_0, 0x00000000);
	DCA_WRITE (DC_WIN_A_COLOR_DEPTH_0, 0x0000000C);

	DCA_WRITE (DC_WIN_A_POSITION_0, 0x00000000);
	DCA_WRITE (DC_WIN_A_SIZE_0, 0x00000000);
	DCA_WRITE (DC_WIN_A_PRESCALED_SIZE_0, 0x00000000);
	DCA_WRITE (DC_WIN_A_H_INITIAL_DDA_0, 0x00000000);
	DCA_WRITE (DC_WIN_A_V_INITIAL_DDA_0, 0x00000000);
	DCA_WRITE (DC_WIN_A_DDA_INCREMENT_0, 0x00000000);
	DCA_WRITE (DC_WIN_A_LINE_STRIDE_0, 0x00000000);
	DCA_WRITE (DC_WIN_A_DV_CONTROL_0, 0x00000000);

	DCA_WRITE (DC_WIN_A_BLEND_LAYER_CONTROL_0, 0x01000000);
	DCA_WRITE (DC_WIN_A_BLEND_MATCH_SELECT_0, 0x00000000);
	DCA_WRITE (DC_WIN_A_BLEND_NOMATCH_SELECT_0, 0x00000000);
	DCA_WRITE (DC_WIN_A_BLEND_ALPHA_1BIT_0, 0x00000000);

	DCA_WRITE (DC_WINBUF_A_START_ADDR_HI_0, 0x00000000);
	DCA_WRITE (DC_WINBUF_A_ADDR_H_OFFSET_0, 0x00000000);
	DCA_WRITE (DC_WINBUF_A_ADDR_V_OFFSET_0, 0x00000000);
	DCA_WRITE (DC_CMD_DISPLAY_WINDOW_HEADER_0, 0x00000000);

	DCA_WRITE (DC_COM_CRC_CONTROL_0, 0x00000000);
	DCA_WRITE (DC_COM_CRC_CHECKSUM_0, 0x00000000);
	DCA_WRITE (DC_COM_PIN_OUTPUT_ENABLE0_0, 0x00000000);
	DCA_WRITE (DC_COM_PIN_OUTPUT_ENABLE1_0, 0x00000000);
	DCA_WRITE (DC_COM_PIN_OUTPUT_ENABLE2_0, 0x00510104);
	DCA_WRITE (DC_COM_PIN_OUTPUT_ENABLE3_0, 0x00000555);
}

static int dp_poll_register(void *addr, u32 exp_val, u32 mask, u32 timeout_ms)
{
	u32 reg_val = 0;

	do {
		udelay(1000);
		reg_val = READL(addr);
	} while (((reg_val & mask) != exp_val) && (--timeout_ms > 0));

	if ((reg_val & mask) == exp_val)
		return 0;	/* success */
	printk(BIOS_WARNING, "poll_register %p: timeout\n", addr);
	return timeout_ms;
}

static void dp_io_set_dpd(u32 power_down)
{
	/*
	 * power_down:
	 *  0: out of Deep power down
	 *  1: into deep power down
	 */
	u32 val_reg;

	val_reg = READL((void *)(0x7000e400 + 0x1c4));	/* APBDEV_PMC_IO_DPD2_STATUS_0 */
	if ((((val_reg & DP_LVDS) >> DP_LVDS_SHIFT) & 1) == power_down) {
		printk(BIOS_DEBUG, "PAD already POWER=%d\n", 1 - power_down);
		return;
	}

	/* APBDEV_PMC_IO_DPD2_REQ_0: E_DPD = power on */
	WRITEL((DP_LVDS | ((1 + power_down) << 30)), (void *)(0x7000e400 + 0x1c0));

	dp_poll_register((void *)(0x7000e400 + 0x1C4), 0, DP_LVDS, 1000);
	/* APBDEV_PMC_IO_DPD2_STATUS_0 */
}

void dp_io_powerup(void)
{
	SOR_WRITE(SOR_NV_PDISP_SOR_CLK_CNTRL_0, (6 << 2) | 2);//select PLLDP,  lowest speed(6x)
	SOR_WRITE(SOR_NV_PDISP_SOR_DP_PADCTL0_0, 0x00800000);	//set PDCAL
	SOR_WRITE(SOR_NV_PDISP_SOR_PLL0_0, 0x050003D5);	//set PWR,VCOPD
	SOR_WRITE(SOR_NV_PDISP_SOR_PLL1_0, 0x00001100);	//default
	SOR_WRITE(SOR_NV_PDISP_SOR_PLL2_0, 0x01C20000);	//set AUX1,6,7,8; clr AUX2
	SOR_WRITE(SOR_NV_PDISP_SOR_PLL3_0, 0x38002220);

	/* Deassert E_DPD to enable core logic circuits, and wait for > 5us */
	dp_io_set_dpd(0);
	udelay(10);

	/* Deassert PDBG to enable bandgap, and wait for > 20us. */
	SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_PLL2_0,
					 SOR_NV_PDISP_SOR_PLL2_0_AUX6_FIELD,
					 (0 << SOR_NV_PDISP_SOR_PLL2_0_AUX6_SHIFT));
	udelay(25);

	/*
	 * Enable the PLL/charge-pump/VCO, and wait for >200us for the PLL to
	 * lock. Input Clock must be running and stable before PDPLL
	 * de-assertion.
	 */
	SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_PLL0_0,
					 SOR_NV_PDISP_SOR_PLL0_0_PWR_FIELD,
					 (0 << SOR_NV_PDISP_SOR_PLL0_0_PWR_SHIFT));
	SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_PLL0_0,
					 SOR_NV_PDISP_SOR_PLL0_0_VCOPD_FIELD,
					 (0 << SOR_NV_PDISP_SOR_PLL0_0_VCOPD_SHIFT));
	SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_PLL2_0,
					 SOR_NV_PDISP_SOR_PLL2_0_AUX8_FIELD,
					 (0 << SOR_NV_PDISP_SOR_PLL2_0_AUX8_SHIFT));
	udelay(210);

	SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_PLL2_0,
					 SOR_NV_PDISP_SOR_PLL2_0_AUX7_FIELD,
					 (0 << SOR_NV_PDISP_SOR_PLL2_0_AUX7_SHIFT));
	SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_PLL2_0,
					 SOR_NV_PDISP_SOR_PLL2_0_AUX9_FIELD,
					 (1 << SOR_NV_PDISP_SOR_PLL2_0_AUX9_SHIFT));
	udelay(100);

}

static int dpaux_check(u32 bytes, u32 data, u32 mask)
{
	u32 status = 0;
	u8 buf[16];
	u32 temp;

	DPAUX_WRITE(DPAUX_DP_AUXDATA_READ_W0, 0);
	status = dpaux_read(0x202, bytes, buf);
	if (status != 0)
		printk(BIOS_ERR, "******AuxRead Error:%04x: status %08x\n",
				 0x202, status);
	else {
		temp = DPAUX_READ(DPAUX_DP_AUXDATA_READ_W0);
		if ((temp & mask) != (data & mask)) {
			printk(BIOS_ERR, "AuxCheck ERROR:(r_data) %08x &"
				" (mask) %08x != (data) %08x & (mask) %08x\n",
				temp, mask, data, mask);
			return -1;
		} else {
			printk(BIOS_DEBUG, "AuxCheck PASS:(bytes=%d, "
				"data=%08x, mask=%08x):0x%08x\n",
				bytes, data, mask, temp);
			return 0;
		}
	}
	return -1;
}

/* Modify the drive parameters for DP.  There are up to four DP
 * lanes. In principle, each lane can have different current,
 * pre-emphasis, and postcur values. Nobody individualizes them; every
 * single driver I've seen drives all the lanes to the same value
 * (across x86 and ARM code). Actualy adjusting them individually and
 * getting it all to work is probably a PhD thesis anyway. So, rather
 * than the very complex code we see many places, the people who wrote
 * this code realize: we can represent the 'volume' as a number in the
 * range 0..3, with '0' as the base and '3' as being 'not to exceed'.
 *
 * So they abstract the values away, take care of the proper values,
 * and set it all in one blow. Very nice. By far the easiest one of
 * these functions we've seen. Sure, they could have constants, but
 * nobody knows what PRE_EMPHASIS_3_5 and the other values actually
 * *mean* anyway. Well, the hardware guys might.
 */
static void pattern_level(u32 current, u32 preemph, u32 postcur)
{
	//calibrating required
	if (current == 0)
		SOR_WRITE(SOR_NV_PDISP_SOR_LANE_DRIVE_CURRENT0_0, 0x20202020);
	if (current == 1)
		SOR_WRITE(SOR_NV_PDISP_SOR_LANE_DRIVE_CURRENT0_0, 0x24242424);
	if (current == 2)
		SOR_WRITE(SOR_NV_PDISP_SOR_LANE_DRIVE_CURRENT0_0, 0x30303030);
	if (current == 3)
		SOR_WRITE(SOR_NV_PDISP_SOR_LANE_DRIVE_CURRENT0_0, 0x40404040);
	if (preemph == 0)
		SOR_WRITE(SOR_NV_PDISP_SOR_LANE_PREEMPHASIS0_0, 0x00000000);
	if (preemph == 1)
		SOR_WRITE(SOR_NV_PDISP_SOR_LANE_PREEMPHASIS0_0, 0x08080808);
	if (preemph == 2)
		SOR_WRITE(SOR_NV_PDISP_SOR_LANE_PREEMPHASIS0_0, 0x10101010);
	if (preemph == 3)
		SOR_WRITE(SOR_NV_PDISP_SOR_LANE_PREEMPHASIS0_0, 0x18181818);
	if (postcur == 0)
		SOR_WRITE(SOR_NV_PDISP_SOR_POSTCURSOR0_0, 0x00000000);
	if (postcur == 1)
		SOR_WRITE(SOR_NV_PDISP_SOR_POSTCURSOR0_0, 0x04040404);
	if (postcur == 2)
		SOR_WRITE(SOR_NV_PDISP_SOR_POSTCURSOR0_0, 0x08080808);
	if (postcur == 3)
		SOR_WRITE(SOR_NV_PDISP_SOR_POSTCURSOR0_0, 0x10101010);
}

static int dp_training(u32 level, u32 check, u32 speed)
{
	/* The levels are one of four choices. This code
	 * packs them into the three lowest nibl's. We may change this.
	 */
	u32 dc_lv = level & 0x0f;
	u32 pe_lv = (level >> 4) & 0x0f;
	u32 pc_lv = (level >> 8) & 0x0f;
	u32 cnt = 0;
	u32 cfg, cfg_d = 0;
	u32 wcfg;
	u8 buf[16];

	while (cnt <= 5) {
		pattern_level(dc_lv, pe_lv, pc_lv);
		wcfg = (pe_lv << 3) | dc_lv;
		if (dc_lv == 3)
			wcfg = wcfg | 0x04;
		if (pe_lv == 3)
			wcfg = wcfg | 0x20;
		wcfg = wcfg | (wcfg << 8) | (wcfg << 16) | (wcfg << 24);
		dpaux_write(0x103, 4, wcfg);
		udelay(100);
		DPAUX_WRITE(DPAUX_DP_AUXDATA_READ_W0, 0);
		if (!dpaux_check(2, check, check))
			cnt = 100;
		else {
			dpaux_read(0x206, 1, buf);
			cfg = DPAUX_READ(DPAUX_DP_AUXDATA_READ_W0);
			cfg &= 0x00ff;
			if (cfg == cfg_d) {
				++cnt;
				if (cnt > 5)
					printk(BIOS_ERR, "Error: link training FAILED\n");
			} else {
				cnt = 0;
				cfg_d = cfg;
				dc_lv = cfg & 0x3;
				pe_lv = (cfg >> 2) & 0x3;
				if (speed == 20) {
					dpaux_read(0x20C, 1, buf);
					cfg = DPAUX_READ(DPAUX_DP_AUXDATA_READ_W0);
					pc_lv = cfg & 0x3;
				} else {
					pc_lv = 0;
				}
			}

		}
		debug_dpaux_print(0x200, 16);
	}

	return ((pc_lv << 8) | (pe_lv << 4) | (dc_lv));

}

void dp_link_training(u32 lanes, u32 speed);
void dp_link_training(u32 lanes, u32 speed)
{
	u32 lane_on;
	u32 mask, level;
	u32 reg_val;

	printk(BIOS_DEBUG, "\nLink training start\n");

	switch (lanes) {
		case 1:
			lane_on = 0x04;
			break;
		case 2:
			lane_on = 0x06;
			break;
		case 4:
			lane_on = 0x0f;
			break;
		default:
			printk(BIOS_DEBUG, "dp: invalid lane count: %d\n",
				lanes);
			return;
	}

	SOR_WRITE(SOR_NV_PDISP_SOR_DP_PADCTL0_0, (0x008000000 | lane_on));
	SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_DP_PADCTL0_0,
		 SOR_NV_PDISP_SOR_DP_PADCTL0_0_TX_PU_VALUE_FIELD,
		 (6 << SOR_NV_PDISP_SOR_DP_PADCTL0_0_TX_PU_VALUE_SHIFT));
	SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_DP_PADCTL0_0,
		 SOR_NV_PDISP_SOR_DP_PADCTL0_0_TX_PU_FIELD,
		 (1 << SOR_NV_PDISP_SOR_DP_PADCTL0_0_TX_PU_SHIFT));
	SOR_WRITE(SOR_NV_PDISP_SOR_LVDS_0, 0);

	SOR_WRITE(SOR_NV_PDISP_SOR_CLK_CNTRL_0, ((speed << 2) | 2));
	udelay(100 * 1000);

	sor_clock_start();

	SOR_WRITE(SOR_NV_PDISP_SOR_DP_LINKCTL0_0,
			  (((0xF >> (4 - lanes)) << 16) | 1));

	SOR_WRITE(SOR_NV_PDISP_SOR_LANE_SEQ_CTL_0, 0x80100000);
	printk(BIOS_DEBUG, "Polling SOR_NV_PDISP_SOR_LANE_SEQ_CTL_0.DONE\n");

	dp_poll_register((void *)0x54540084, 0x00000000, 0x80000000, 1000);

	debug_dpaux_print(0x202, 4);

	printk(BIOS_DEBUG, "set link rate and lane number: %dMHz, %d lanes\n",
		   (speed * 27), lanes);

	dpaux_write(0x100, 2, ((lanes << 8) | speed));
	printk(BIOS_DEBUG, "precharge lane 10us\n");
	reg_val = SOR_READ(SOR_NV_PDISP_SOR_DP_PADCTL0_0);
	SOR_WRITE(SOR_NV_PDISP_SOR_DP_PADCTL0_0, (0x000000f0 | reg_val));
	udelay(100 * 1000);
	SOR_WRITE(SOR_NV_PDISP_SOR_DP_PADCTL0_0, reg_val);

	printk(BIOS_DEBUG, "link training cr start\n");
	SOR_WRITE(SOR_NV_PDISP_SOR_DP_TPG_0, 0x41414141);
	dpaux_write(0x102, 1, 0x21);

	mask = 0x0000ffff >> ((4 - lanes) * 4);
	level = 0;
	level = dp_training(level, 0x1111 & mask, speed);
	printk(BIOS_DEBUG, "level:%x\n", level);

	debug_dpaux_print(0x210, 16);

	printk(BIOS_DEBUG, "link training eq start\n");
	if (speed == 20) {
		SOR_WRITE(SOR_NV_PDISP_SOR_DP_TPG_0, 0x43434343);
		dpaux_write(0x102, 1, 0x23);
	} else {
		SOR_WRITE(SOR_NV_PDISP_SOR_DP_TPG_0, 0x42424242);
		dpaux_write(0x102, 1, 0x22);
	}

	level = dp_training(level, (0x7777 & mask) | 0x10000, speed);
	printk(BIOS_DEBUG, "level:%x\n", level);

	debug_dpaux_print(0x210, 16);

	SOR_WRITE(SOR_NV_PDISP_SOR_DP_TPG_0, 0x50505050);
	dpaux_write(0x102, 1, 0);
	dpaux_write(0x600, 1, 1);

	debug_dpaux_print(0x200, 16);
	debug_dpaux_print(0x210, 16);

	printk(BIOS_DEBUG, "Link training done\n\n");
}

static u32 div_f(u32 a, u32 b, u32 one)
{
	u32 d = (((a - (a / b * b)) * one) + (b / 2)) / b;
	return (d);
}

u32 dp_setup_timing(u32 width, u32 height)
{
	u32 pclk_freq = 0;

	u32 PLL_FREQ = (12 / 12 * 283) / 1 / 2;	/* 141.5 */
	u32 PLL_DIV = 2;
	u32 SYNC_WIDTH = (8 << 16) | 46;
	u32 BACK_PORCH = (6 << 16) | 44;
	u32 FRONT_PORCH = (6 << 16) | 44;
	u32 HSYNC_NEG = 1;
	u32 VSYNC_NEG = 1;

	u32 SHIFT_CLK_DIVIDER = PLL_DIV * 2 - 2;
	u32 DISP_ACTIVE = (height << 16) | width;
	u32 DISP_TOTAL = DISP_ACTIVE + SYNC_WIDTH + BACK_PORCH + FRONT_PORCH;
	u32 SYNC_END = SYNC_WIDTH - 0x10001;
	u32 BLANK_END = SYNC_END + BACK_PORCH;
	u32 BLANK_START = BLANK_END + DISP_ACTIVE;
	u32 TOTAL_PIXELS = (DISP_TOTAL & 0xffff) * (DISP_TOTAL >> 16);

	u32 PLL_FREQ_I, PLL_FREQ_F;
	u32 PCLK_FREQ_I, PCLK_FREQ_F;
	u32 FRATE_I, FRATE_F;

	PLL_FREQ = PLL_FREQ * 1000000;
	pclk_freq = PLL_FREQ / PLL_DIV;
	PLL_FREQ_I = PLL_FREQ / 1000000;
	PLL_FREQ_F = div_f(PLL_FREQ, 1000000, 100);
	PCLK_FREQ_I = PLL_FREQ / (PLL_DIV * 1000000);
	PCLK_FREQ_F = div_f(PLL_FREQ, PLL_DIV * 1000000, 100);
	FRATE_I = PLL_FREQ / (PLL_DIV * TOTAL_PIXELS);
	FRATE_F = div_f(PLL_FREQ, (PLL_DIV * TOTAL_PIXELS), 100);
	/* nv_bug 1021453 */
	BACK_PORCH = BACK_PORCH - 0x10000;
	FRONT_PORCH = FRONT_PORCH + 0x10000;

	printk(BIOS_DEBUG, "ACTIVE:      %dx%d\n", (DISP_ACTIVE & 0xFFFF),
		   (DISP_ACTIVE >> 16));
	printk(BIOS_DEBUG, "TOTAL:       %dx%d\n", (DISP_TOTAL & 0xffff),
		   (DISP_TOTAL >> 16));
	printk(BIOS_DEBUG, "PLL Freq:    %d.%d MHz\n", PLL_FREQ_I, PLL_FREQ_F);
	printk(BIOS_DEBUG, "Pclk Freq:   %d.%d MHz\n", PCLK_FREQ_I,
		   PCLK_FREQ_F);
	printk(BIOS_DEBUG, "Frame Rate:  %d.%d Hz\n", FRATE_I, FRATE_F);
	printk(BIOS_DEBUG, "\n");

	DCA_WRITE(DC_CMD_STATE_ACCESS_0, 0x00000004);
	DCA_WRITE(DC_DISP_DISP_CLOCK_CONTROL_0, SHIFT_CLK_DIVIDER);
	//Raster Timing
	DCA_WRITE(DC_DISP_DISP_TIMING_OPTIONS_0, 0x00000001);
	DCA_WRITE(DC_DISP_REF_TO_SYNC_0, 0x00010001);
	DCA_WRITE(DC_DISP_SYNC_WIDTH_0, SYNC_WIDTH);
	DCA_WRITE(DC_DISP_BACK_PORCH_0, BACK_PORCH);
	DCA_WRITE(DC_DISP_DISP_ACTIVE_0, DISP_ACTIVE);
	DCA_WRITE(DC_DISP_FRONT_PORCH_0, FRONT_PORCH);

	//REG(DC_DISP_DISP_WIN_OPTIONS_0, SOR_ENABLE , 1)
	DCA_READ_M_WRITE(DC_DISP_DISP_WIN_OPTIONS_0,
					 DC_DISP_DISP_WIN_OPTIONS_0_SOR_ENABLE_FIELD,
					 (1 << DC_DISP_DISP_WIN_OPTIONS_0_SOR_ENABLE_SHIFT));

	SOR_WRITE(SOR_NV_PDISP_HEAD_STATE1_0, DISP_TOTAL);
	SOR_WRITE(SOR_NV_PDISP_HEAD_STATE2_0, SYNC_END);
	SOR_WRITE(SOR_NV_PDISP_HEAD_STATE3_0, BLANK_END);
	SOR_WRITE(SOR_NV_PDISP_HEAD_STATE4_0, BLANK_START);

	SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_STATE1_0,
					 SOR_NV_PDISP_SOR_STATE1_0_ASY_HSYNCPOL_FIELD,
					 (HSYNC_NEG <<
					  SOR_NV_PDISP_SOR_STATE1_0_ASY_HSYNCPOL_SHIFT));

	SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_STATE1_0,
					 SOR_NV_PDISP_SOR_STATE1_0_ASY_VSYNCPOL_FIELD,
					 (VSYNC_NEG <<
					  SOR_NV_PDISP_SOR_STATE1_0_ASY_VSYNCPOL_SHIFT));

	SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_STATE1_0,
					 SOR_NV_PDISP_SOR_STATE1_0_ASY_PROTOCOL_FIELD,
					 (SOR_NV_PDISP_SOR_STATE1_0_ASY_PROTOCOL_DP_A <<
					  SOR_NV_PDISP_SOR_STATE1_0_ASY_PROTOCOL_SHIFT));

	SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_STATE1_0,
					 SOR_NV_PDISP_SOR_STATE1_0_ASY_CRCMODE_FIELD,
					 (SOR_NV_PDISP_SOR_STATE1_0_ASY_CRCMODE_COMPLETE_RASTER <<
					  SOR_NV_PDISP_SOR_STATE1_0_ASY_CRCMODE_SHIFT));

	SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_STATE1_0,
					 SOR_NV_PDISP_SOR_STATE1_0_ASY_SUBOWNER_FIELD,
					 (SOR_NV_PDISP_SOR_STATE1_0_ASY_SUBOWNER_NONE <<
					  SOR_NV_PDISP_SOR_STATE1_0_ASY_SUBOWNER_SHIFT));

	SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_STATE1_0,
					 SOR_NV_PDISP_SOR_STATE1_0_ASY_OWNER_FIELD,
					 (SOR_NV_PDISP_SOR_STATE1_0_ASY_OWNER_HEAD0 <<
					  SOR_NV_PDISP_SOR_STATE1_0_ASY_OWNER_SHIFT));
	return pclk_freq;
}

static u32 calc_config(u32 ts, u32 a, u32 b, u32 bpp)
{
	u32 act_cnt = (ts * a) / b;
	u32 diff = (ts * a) - (act_cnt * b);
	u32 act_pol;
	u32 act_frac;
	u32 err;
	u32 water_mark;

	printk(BIOS_DEBUG, "calc_config ts %d a %d b %d bpp %d\n",
			ts, a, b, bpp);
	if (diff != 0) {
		if (diff > (b / 2)) {
			diff = b - diff;
			act_pol = 1;
			act_frac = (b + diff - 1) / diff;
			err = diff * act_frac - b;
		} else {
			act_pol = 0;
			act_frac = b / diff;
			err = b - (diff * act_frac);
		}
		if (act_frac > 15) {
			act_pol = 1 - act_pol;
			act_frac = 1;
			err = diff;
		}
	} else {
		act_pol = 1;
		act_frac = 1;
		err = 0;
	}

	if (bpp) {
		water_mark = (a * (b - a) * ts / (b * b)) + (2 * bpp / 8);
		if (water_mark > 30)
			water_mark = 30;

		SOR_WRITE(SOR_NV_PDISP_SOR_DP_CONFIG0_0, 0x84000000);
		SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_DP_CONFIG0_0,
				 SOR_NV_PDISP_SOR_DP_CONFIG0_0_ACTIVESYM_POLARITY_FIELD,
				 (act_pol <<
				  SOR_NV_PDISP_SOR_DP_CONFIG0_0_ACTIVESYM_POLARITY_SHIFT));
		SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_DP_CONFIG0_0,
				 SOR_NV_PDISP_SOR_DP_CONFIG0_0_ACTIVESYM_FRAC_FIELD,
				 (act_frac <<
				  SOR_NV_PDISP_SOR_DP_CONFIG0_0_ACTIVESYM_FRAC_SHIFT));
		SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_DP_CONFIG0_0,
				 SOR_NV_PDISP_SOR_DP_CONFIG0_0_ACTIVESYM_COUNT_FIELD,
				 (act_cnt <<
				  SOR_NV_PDISP_SOR_DP_CONFIG0_0_ACTIVESYM_COUNT_SHIFT));
		SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_DP_CONFIG0_0,
				 SOR_NV_PDISP_SOR_DP_CONFIG0_0_WATERMARK_FIELD,
				 (water_mark <<
				  SOR_NV_PDISP_SOR_DP_CONFIG0_0_WATERMARK_SHIFT));

		printk(BIOS_DEBUG,
		       "SOR_DP_CONFIG0:TU,CNT,POL,FRAC,WMK,ERR=%d,%d,%d,%d,%d,%d/%d\n",
		       ts, act_cnt, act_pol, act_frac, water_mark, err, b);
	}
	return (err);
}

static u32 dp_buf_config(u32 pclkfreq, u32 linkfreq, u32 lanes, u32 bpp)
{
	//to avoid 32bit overflow
	u32 tusize = 0;
	u32 pf = pclkfreq;
	u32 lf = linkfreq;
	u32 i;
	u32 a, b;
	u32 min_err = 1000000000;
	u32 ts = 64;
	u32 c_err;

	printk(BIOS_DEBUG, "dp buf config pclkfreq %d linkfreq %d lanes %d bpp %d\n",
		   pclkfreq, linkfreq, lanes, bpp);
	for (i = 2; i <= 7; ++i) {
		while (((pf / i * i) == pf) && ((lf / i * i) == lf)) {
			pf = pf / i;
			lf = lf / i;
		}
	}

	a = pf * bpp / 8;
	b = lf * lanes;
	printk(BIOS_DEBUG, "ratio:%d/%d\n", a, b);
	if (a > (b * 98 / 100))
		printk(BIOS_ERR, "Error:link speed not enough\n");

	//search best tusize
	//min_err = 1000000000;
	//ts = 64;
	while (ts >= 32) {
		c_err = calc_config(ts, a, b, 0);
		if (c_err < min_err) {
			if (c_err == 0) {
				tusize = ts;
				ts = 1;
			} else {
				min_err = c_err;
				tusize = ts;
			}
		}
		--ts;
	}

	SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_DP_LINKCTL0_0,
			 SOR_NV_PDISP_SOR_DP_LINKCTL0_0_TUSIZE_FIELD,
			 (tusize << SOR_NV_PDISP_SOR_DP_LINKCTL0_0_TUSIZE_SHIFT));
	calc_config(tusize, a, b, bpp);

	return (tusize);
}

/*
void dp_misc_setting(u32 panel_bpp, u32 width, u32 height, u32 winb_addr,
		     u32 lane_count, u32 enhanced_framing, u32 panel_edp,
		     u32 pclkfreq, u32 linkfreq);
*/
void dp_misc_setting(u32 panel_bpp, u32 width, u32 height, u32 winb_addr,
		     u32 lane_count, u32 enhanced_framing, u32 panel_edp,
		     u32 pclkfreq, u32 linkfreq)
{
	u32 tusize;
	u32 linkctl;

	printk(BIOS_DEBUG, "%s: winb: 0x%08x, panel_bpp %d ",
			__func__, winb_addr, panel_bpp);

	if (panel_bpp == 18) {
		//0x54540010
		SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_STATE1_0,
				 SOR_NV_PDISP_SOR_STATE1_0_ASY_PIXELDEPTH_FIELD,
				 (SOR_NV_PDISP_SOR_STATE1_0_ASY_PIXELDEPTH_BPP_18_444 <<
				  SOR_NV_PDISP_SOR_STATE1_0_ASY_PIXELDEPTH_SHIFT));
	}
	if (panel_bpp == 24) {
		SOR_READ_M_WRITE(SOR_NV_PDISP_SOR_STATE1_0,
				 SOR_NV_PDISP_SOR_STATE1_0_ASY_PIXELDEPTH_FIELD,
				 (SOR_NV_PDISP_SOR_STATE1_0_ASY_PIXELDEPTH_BPP_24_444 <<
				  SOR_NV_PDISP_SOR_STATE1_0_ASY_PIXELDEPTH_SHIFT));
	}

	DCA_WRITE(DC_CMD_DISPLAY_WINDOW_HEADER_0, 0x00000010);
	DCA_WRITE(DC_WIN_A_SIZE_0, ((height << 16) | width));
	DCA_WRITE(DC_WIN_A_PRESCALED_SIZE_0,
			  ((height << 16) | (width * SRC_BPP / 8)));
	DCA_WRITE(DC_WIN_A_LINE_STRIDE_0,
			  ((width * SRC_BPP / 8 + 31) / 32 * 32));
	DCA_WRITE(DC_WIN_A_COLOR_DEPTH_0, COLORDEPTH);
	DCA_WRITE(DC_WINBUF_A_START_ADDR_LO_0, winb_addr);
	DCA_WRITE(DC_WIN_A_DDA_INCREMENT_0, 0x10001000);

	SOR_WRITE(SOR_NV_PDISP_SOR_CRC_CNTRL_0, 0x00000001);
	DCA_WRITE(DC_COM_CRC_CONTROL_0, 0x00000009);	//CRC_ALWAYS+CRC_ENABLE
	DCA_WRITE(DC_COM_PIN_OUTPUT_ENABLE2_0, 0x00000000);
	DCA_WRITE(DC_COM_PIN_OUTPUT_ENABLE3_0, 0x00000000);
	DCA_WRITE(DC_DISP_DISP_SIGNAL_OPTIONS0_0, 0x00000000);
	DCA_WRITE(DC_DISP_BLEND_BACKGROUND_COLOR_0, COLOR_WHITE);
	DCA_WRITE(DC_CMD_DISPLAY_COMMAND_0, 0x00000020);
	SOR_WRITE(SOR_NV_PDISP_SOR_DP_AUDIO_VBLANK_SYMBOLS_0, 0x00000e48);

	dpaux_write(0x101, 1, (enhanced_framing << 7) | lane_count);
	if (panel_edp)
		dpaux_write(0x10A, 1, 1);

	tusize =
		dp_buf_config(pclkfreq, (linkfreq * 1000000), lane_count, panel_bpp);

	linkctl =
		((0xF >> (4 - lane_count)) << 16) | (enhanced_framing << 14) | (tusize
										<< 2) |
		1;

	SOR_WRITE(SOR_NV_PDISP_SOR_DP_LINKCTL0_0, linkctl);
	SOR_WRITE(SOR_NV_PDISP_SOR_DP_SPARE0_0, ((panel_edp << 1) | 0x05));

	SOR_WRITE(SOR_NV_PDISP_SOR_PWR_0, 0x80000001);
	printk(BIOS_DEBUG, "Polling SOR_NV_PDISP_SOR_PWR_0.DONE\n");
	dp_poll_register((void *)0x54540054, 0x00000000, 0x80000000, 1000);
	//SOR_NV_PDISP_SOR_PWR_0
	//sor_update
	SOR_WRITE(SOR_NV_PDISP_SOR_STATE0_0, 0x00000000);
	SOR_WRITE(SOR_NV_PDISP_SOR_SUPER_STATE1_0, 0x00000006);
	//sor_super_update
	SOR_WRITE(SOR_NV_PDISP_SOR_SUPER_STATE0_0, 0x00000000);
	SOR_WRITE(SOR_NV_PDISP_SOR_SUPER_STATE1_0, 0x0000000e);
	//sor_super_update
	SOR_WRITE(SOR_NV_PDISP_SOR_SUPER_STATE0_0, 0x00000000);
	printk(BIOS_DEBUG, "Polling SOR_NV_PDISP_SOR_TEST_0.ATTACHED\n");
	dp_poll_register((void *)0x54540058, 0x00000400, 0x00000400, 1000);
	//SOR_NV_PDISP_SOR_TEST_0

	DCA_WRITE(DC_CMD_STATE_CONTROL_0, 0x00009f00);
	DCA_WRITE(DC_CMD_STATE_CONTROL_0, 0x0000009f);
	DCA_WRITE(DC_CMD_DISPLAY_POWER_CONTROL_0, 0x00050155);

	printk(BIOS_DEBUG, "Polling SOR_NV_PDISP_SOR_TEST_0.AWAKE\n");
	dp_poll_register((void *)0x54540058, 0x00000200, 0x00000300, 1000);
	//SOR_NV_PDISP_SOR_TEST_0

	//  DCA_WRITE (DC_CMD_STATE_ACCESS_0   ,0);
	DCA_WRITE(DC_CMD_STATE_ACCESS_0, 4);
	DCA_WRITE(DC_CMD_STATE_CONTROL_0, 0x0000ffff);

	DCA_READ_M_WRITE(DC_WIN_A_WIN_OPTIONS_0,
			 DC_WIN_A_WIN_OPTIONS_0_A_WIN_ENABLE_FIELD,
			 (DC_WIN_A_WIN_OPTIONS_0_A_WIN_ENABLE_ENABLE <<
			  DC_WIN_A_WIN_OPTIONS_0_A_WIN_ENABLE_SHIFT));
}
