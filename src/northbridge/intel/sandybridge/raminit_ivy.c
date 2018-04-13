/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Damien Zammit <damien@zamaudio.com>
 * Copyright (C) 2014 Vladimir Serbinenko <phcoder@gmail.com>
 * Copyright (C) 2016 Patrick Rudolph <siro@das-labor.org>
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

#include <console/console.h>
#include <console/usb.h>
#include <cpu/x86/msr.h>
#include <delay.h>
#include <arch/io.h>
#include <device/pci_ops.h>
#include "raminit_native.h"
#include "raminit_common.h"

/* Frequency multiplier.  */
static u32 get_FRQ(u32 tCK, u8 base_freq)
{
	u32 FRQ;

	FRQ = 256000 / (tCK * base_freq);

	if (base_freq == 100) {
		if (FRQ > 12)
			return 12;
		if (FRQ < 7)
			return 7;
	} else {
		if (FRQ > 10)
			return 10;
		if (FRQ < 3)
			return 3;
	}

	return FRQ;
}

static u32 get_REFI(u32 tCK, u8 base_freq)
{
	u32 refi;

	if (base_freq == 100) {
		/* Get REFI based on MCU frequency using the following rule:
		 * tREFI = 7.8usec
		 *         _________________________________________
		 * FRQ  : | 7    | 8    | 9    | 10   | 11   | 12   |
		 * REFI : | 5460 | 6240 | 7020 | 7800 | 8580 | 9360 |
		 */
		static const u32 frq_xs_map[] =
		    { 5460, 6240, 7020, 7800, 8580, 9360 };
		refi = frq_xs_map[get_FRQ(tCK, 100) - 7];
	} else {
		/* Get REFI based on MCU frequency using the following rule:
		 * tREFI = 7.8usec
		 *        ________________________________________________________
		 * FRQ : | 3    | 4    | 5    | 6    | 7    | 8    | 9    | 10    |
		 * REFI: | 3120 | 4160 | 5200 | 6240 | 7280 | 8320 | 9360 | 10400 |
		 */
		static const u32 frq_refi_map[] =
		    { 3120, 4160, 5200, 6240, 7280, 8320, 9360, 10400 };
		refi = frq_refi_map[get_FRQ(tCK, 133) - 3];
	}

	return refi;
}

static u8 get_XSOffset(u32 tCK, u8 base_freq)
{
	u8 xsoffset;

	if (base_freq == 100) {
		/* Get XSOffset based on MCU frequency using the following rule:
		 * tXS-offset: tXS = tRFC+10ns.
		 *             _____________________________
		 * FRQ      : | 7  | 8  | 9  | 10 | 11 | 12 |
		 * XSOffset : | 7  | 8  | 9  | 10 | 11 | 12 |
		 */
		static const u8 frq_xs_map[] = { 7, 8, 9, 10, 11, 12 };
		xsoffset = frq_xs_map[get_FRQ(tCK, 100) - 7];
	} else {
		/* Get XSOffset based on MCU frequency using the following rule:
		 *             ___________________________________
		 * FRQ      : | 3 | 4 | 5 | 6 | 7  | 8  | 9  | 10 |
		 * XSOffset : | 4 | 6 | 7 | 8 | 10 | 11 | 12 | 14 |
		 */
		static const u8 frq_xs_map[] = { 4, 6, 7, 8, 10, 11, 12, 14 };
		xsoffset = frq_xs_map[get_FRQ(tCK, 133) - 3];
	}

	return xsoffset;
}

static u8 get_MOD(u32 tCK, u8 base_freq)
{
	u8 mod;

	if (base_freq == 100) {
		/* Get MOD based on MCU frequency using the following rule:
		 *        _____________________________
		 * FRQ : | 7  | 8  | 9  | 10 | 11 | 12 |
		 * MOD : | 12 | 12 | 14 | 15 | 17 | 18 |
		 */

		static const u8 frq_mod_map[] = { 12, 12, 14, 15, 17, 18 };
		mod = frq_mod_map[get_FRQ(tCK, 100) - 7];
	} else {
		/* Get MOD based on MCU frequency using the following rule:
		 *        _______________________________________
		 * FRQ : | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 |
		 * MOD : | 12 | 12 | 12 | 12 | 15 | 16 | 18 | 20 |
		 */

		static const u8 frq_mod_map[] = { 12, 12, 12, 12, 15, 16, 18, 20 };
		mod = frq_mod_map[get_FRQ(tCK, 133) - 3];
	}
	return mod;
}

static u8 get_WLO(u32 tCK, u8 base_freq)
{
	u8 wlo;

	if (base_freq == 100) {
		/* Get WLO based on MCU frequency using the following rule:
		 * Write leveling output delay
		 *        _____________________________
		 * FRQ : | 7  | 8  | 9  | 10 | 11 | 12 |
		 * MOD : | 6  | 6  | 7  | 8  | 9  | 9  |
		 */

		static const u8 frq_wlo_map[] = { 6, 6, 7, 8, 9, 9 };
		wlo = frq_wlo_map[get_FRQ(tCK, 100) - 7];
	} else {
		/* Get WLO based on MCU frequency using the following rule:
		 * Write leveling output delay
		 *        ________________________________
		 * FRQ : | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 |
		 * WLO : | 4 | 5 | 6 | 6 | 8 | 8 | 9 | 10 |
		 */
		static const u8 frq_wlo_map[] = { 4, 5, 6, 6, 8, 8, 9, 10 };
		wlo = frq_wlo_map[get_FRQ(tCK, 133) - 3];
	}

	return wlo;
}

static u8 get_CKE(u32 tCK, u8 base_freq)
{
	u8 cke;

	if (base_freq == 100) {
		/* Get CKE based on MCU frequency using the following rule:
		 *        _____________________________
		 * FRQ : | 7  | 8  | 9  | 10 | 11 | 12 |
		 * MOD : | 4  | 4  | 5  | 5  | 6  | 6  |
		 */

		static const u8 frq_cke_map[] = { 4, 4, 5, 5, 6, 6 };
		cke = frq_cke_map[get_FRQ(tCK, 100) - 7];
	} else {
		/* Get CKE based on MCU frequency using the following rule:
		 *        ________________________________
		 * FRQ : | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 |
		 * WLO : | 3 | 3 | 4 | 4 | 5 | 6 | 6 | 7  |
		 */
		static const u8 frq_cke_map[] = { 3, 3, 4, 4, 5, 6, 6, 7 };
		cke = frq_cke_map[get_FRQ(tCK, 133) - 3];
	}

	return cke;
}

static u8 get_XPDLL(u32 tCK, u8 base_freq)
{
	u8 xpdll;

	if (base_freq == 100) {
		/* Get XPDLL based on MCU frequency using the following rule:
		 *          _____________________________
		 * FRQ   : | 7  | 8  | 9  | 10 | 11 | 12 |
		 * XPDLL : | 17 | 20 | 22 | 24 | 27 | 32 |
		 */

		static const u8 frq_xpdll_map[] = { 17, 20, 22, 24, 27, 32 };
		xpdll = frq_xpdll_map[get_FRQ(tCK, 100) - 7];
	} else {
		/* Get XPDLL based on MCU frequency using the following rule:
		 *          _______________________________________
		 * FRQ   : | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 |
		 * XPDLL : | 10 | 13 | 16 | 20 | 23 | 26 | 29 | 32 |
		 */
		static const u8 frq_xpdll_map[] = { 10, 13, 16, 20, 23, 26, 29, 32 };
		xpdll = frq_xpdll_map[get_FRQ(tCK, 133) - 3];
	}

	return xpdll;
}

static u8 get_XP(u32 tCK, u8 base_freq)
{
	u8 xp;

	if (base_freq == 100) {
		/* Get XP based on MCU frequency using the following rule:
		 *        _____________________________
		 * FRQ : | 7  | 8  | 9  | 10 | 11 | 12 |
		 * XP  : | 5  | 5  | 6  | 6  | 7  | 8  |
		 */

		static const u8 frq_xp_map[] = { 5, 5, 6, 6, 7, 8 };
		xp = frq_xp_map[get_FRQ(tCK, 100) - 7];
	} else {
		/* Get XP based on MCU frequency using the following rule:
		 *        _______________________________________
		 * FRQ : | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 |
		 * XP  : | 3  | 4  | 4  | 5  | 6  | 7  | 8  | 8  |
		 */
		static const u8 frq_xp_map[] = { 3, 4, 4, 5, 6, 7 , 8, 8 };
		xp = frq_xp_map[get_FRQ(tCK, 133) - 3];
	}

	return xp;
}

static u8 get_AONPD(u32 tCK, u8 base_freq)
{
	u8 aonpd;

	if (base_freq == 100) {
		/* Get AONPD based on MCU frequency using the following rule:
		 *          _____________________________
		 * FRQ   : | 7  | 8  | 9  | 10 | 11 | 12 |
		 * AONPD : | 6  | 8  | 8  | 9  | 10 | 11 |
		 */

		static const u8 frq_aonpd_map[] = { 6, 8, 8, 9, 10, 11 };
		aonpd = frq_aonpd_map[get_FRQ(tCK, 100) - 7];
	} else {
		/* Get AONPD based on MCU frequency using the following rule:
		 *          _______________________________________
		 * FRQ   : | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 |
		 * AONPD : | 4  | 5  | 6  | 8  | 8  | 10 | 11 | 12 |
		 */
		static const u8 frq_aonpd_map[] = { 4, 5, 6, 8, 8, 10, 11, 12 };
		aonpd = frq_aonpd_map[get_FRQ(tCK, 133) - 3];
	}

	return aonpd;
}

static u32 get_COMP2(u32 tCK, u8 base_freq)
{
	u32 comp2;

	if (base_freq == 100) {
		/* Get COMP2 based on MCU frequency using the following rule:
		 *          ______________________________________________________________
		 * FRQ  : | 7        | 8        | 9        | 10       | 11      | 12      |
		 * COMP : | CA8C264  | C6671E4  | C6671E4  | C446964  | C235924 | C235924 |
		 */

		static const u32 frq_comp2_map[] = { 0xCA8C264, 0xC6671E4, 0xC6671E4, 0xC446964, 0xC235924, 0xC235924 };
		comp2 = frq_comp2_map[get_FRQ(tCK, 100) - 7];
	} else {
		/* Get COMP2 based on MCU frequency using the following rule:
		 *        ________________________________________________________________________________
		 * FRQ  : | 3       | 4       | 5       | 6       | 7       | 8       | 9       | 10      |
		 * COMP : | D6FF5E4 | CEBDB64 | CA8C264 | C6671E4 | C446964 | C235924 | C235924 | C235924 |
		 */
		static const u32 frq_comp2_map[] = { 0xD6FF5E4, 0xCEBDB64, 0xCA8C264,
			0xC6671E4, 0xC446964, 0xC235924, 0xC235924, 0xC235924
		};
		comp2 = frq_comp2_map[get_FRQ(tCK, 133) - 3];
	}

	return comp2;
}

static void ivb_normalize_tclk(ramctr_timing *ctrl,
			bool ref_100mhz_support)
{
	if (ctrl->tCK <= TCK_1200MHZ) {
		ctrl->tCK = TCK_1200MHZ;
		ctrl->base_freq = 100;
	} else if (ctrl->tCK <= TCK_1100MHZ) {
		ctrl->tCK = TCK_1100MHZ;
		ctrl->base_freq = 100;
	} else if (ctrl->tCK <= TCK_1066MHZ) {
		ctrl->tCK = TCK_1066MHZ;
		ctrl->base_freq = 133;
	} else if (ctrl->tCK <= TCK_1000MHZ) {
		ctrl->tCK = TCK_1000MHZ;
		ctrl->base_freq = 100;
	} else if (ctrl->tCK <= TCK_933MHZ) {
		ctrl->tCK = TCK_933MHZ;
		ctrl->base_freq = 133;
	} else if (ctrl->tCK <= TCK_900MHZ) {
		ctrl->tCK = TCK_900MHZ;
		ctrl->base_freq = 100;
	} else if (ctrl->tCK <= TCK_800MHZ) {
		ctrl->tCK = TCK_800MHZ;
		ctrl->base_freq = 133;
	} else if (ctrl->tCK <= TCK_700MHZ) {
		ctrl->tCK = TCK_700MHZ;
		ctrl->base_freq = 100;
	} else if (ctrl->tCK <= TCK_666MHZ) {
		ctrl->tCK = TCK_666MHZ;
		ctrl->base_freq = 133;
	} else if (ctrl->tCK <= TCK_533MHZ) {
		ctrl->tCK = TCK_533MHZ;
		ctrl->base_freq = 133;
	} else if (ctrl->tCK <= TCK_400MHZ) {
		ctrl->tCK = TCK_400MHZ;
		ctrl->base_freq = 133;
	} else {
		ctrl->tCK = 0;
		return;
	}

	if (!ref_100mhz_support && ctrl->base_freq == 100) {
		/* Skip unsupported frequency. */
		ctrl->tCK++;
		ivb_normalize_tclk(ctrl, ref_100mhz_support);
	}
}

static void find_cas_tck(ramctr_timing *ctrl)
{
	u8 val;
	u32 val32;
	u32 reg32;
	u8 ref_100mhz_support;

	/* 100 Mhz reference clock supported */
	reg32 = pci_read_config32(PCI_DEV(0, 0, 0), CAPID0_B);
	ref_100mhz_support = !!((reg32 >> 21) & 0x7);
	printk(BIOS_DEBUG, "100MHz reference clock support: %s\n",
		   ref_100mhz_support ? "yes" : "no");

	/* Find CAS latency */
	while (1) {
		/* Normalising tCK before computing clock could potentially
		 * results in lower selected CAS, which is desired.
		 */
		ivb_normalize_tclk(ctrl, ref_100mhz_support);
		if (!(ctrl->tCK))
			die("Couldn't find compatible clock / CAS settings\n");
		val = DIV_ROUND_UP(ctrl->tAA, ctrl->tCK);
		printk(BIOS_DEBUG, "Trying CAS %u, tCK %u.\n", val, ctrl->tCK);
		for (; val <= MAX_CAS; val++)
			if ((ctrl->cas_supported >> (val - MIN_CAS)) & 1)
				break;
		if (val == (MAX_CAS + 1)) {
			ctrl->tCK++;
			continue;
		} else {
			printk(BIOS_DEBUG, "Found compatible clock, CAS pair.\n");
			break;
		}
	}

	val32 = NS2MHZ_DIV256 / ctrl->tCK;
	printk(BIOS_DEBUG, "Selected DRAM frequency: %u MHz\n", val32);

	printk(BIOS_DEBUG, "Selected CAS latency   : %uT\n", val);
	ctrl->CAS = val;
}


static void dram_timing(ramctr_timing *ctrl)
{
	/* Maximum supported DDR3 frequency is 1400MHz (DDR3 2800).
	 * We cap it at 1200Mhz (DDR3 2400).
	 * Then, align it to the closest JEDEC standard frequency */
	if (ctrl->tCK == TCK_1200MHZ) {
		ctrl->edge_offset[0] = 18; //XXX: guessed
		ctrl->edge_offset[1] = 8;
		ctrl->edge_offset[2] = 8;
		ctrl->timC_offset[0] = 20; //XXX: guessed
		ctrl->timC_offset[1] = 8;
		ctrl->timC_offset[2] = 8;
		ctrl->reg_320c_range_threshold = 10;
	} else if (ctrl->tCK == TCK_1100MHZ) {
		ctrl->edge_offset[0] = 17; //XXX: guessed
		ctrl->edge_offset[1] = 7;
		ctrl->edge_offset[2] = 7;
		ctrl->timC_offset[0] = 19; //XXX: guessed
		ctrl->timC_offset[1] = 7;
		ctrl->timC_offset[2] = 7;
		ctrl->reg_320c_range_threshold = 13;
	} else if (ctrl->tCK == TCK_1066MHZ) {
		ctrl->edge_offset[0] = 16;
		ctrl->edge_offset[1] = 7;
		ctrl->edge_offset[2] = 7;
		ctrl->timC_offset[0] = 18;
		ctrl->timC_offset[1] = 7;
		ctrl->timC_offset[2] = 7;
		ctrl->reg_320c_range_threshold = 13;
	} else if (ctrl->tCK == TCK_1000MHZ) {
		ctrl->edge_offset[0] = 15; //XXX: guessed
		ctrl->edge_offset[1] = 6;
		ctrl->edge_offset[2] = 6;
		ctrl->timC_offset[0] = 17; //XXX: guessed
		ctrl->timC_offset[1] = 6;
		ctrl->timC_offset[2] = 6;
		ctrl->reg_320c_range_threshold = 13;
	} else if (ctrl->tCK == TCK_933MHZ) {
		ctrl->edge_offset[0] = 14;
		ctrl->edge_offset[1] = 6;
		ctrl->edge_offset[2] = 6;
		ctrl->timC_offset[0] = 15;
		ctrl->timC_offset[1] = 6;
		ctrl->timC_offset[2] = 6;
		ctrl->reg_320c_range_threshold = 15;
	} else if (ctrl->tCK == TCK_900MHZ) {
		ctrl->edge_offset[0] = 14; //XXX: guessed
		ctrl->edge_offset[1] = 6;
		ctrl->edge_offset[2] = 6;
		ctrl->timC_offset[0] = 15; //XXX: guessed
		ctrl->timC_offset[1] = 6;
		ctrl->timC_offset[2] = 6;
		ctrl->reg_320c_range_threshold = 12;
	} else if (ctrl->tCK == TCK_800MHZ) {
		ctrl->edge_offset[0] = 13;
		ctrl->edge_offset[1] = 5;
		ctrl->edge_offset[2] = 5;
		ctrl->timC_offset[0] = 14;
		ctrl->timC_offset[1] = 5;
		ctrl->timC_offset[2] = 5;
		ctrl->reg_320c_range_threshold = 15;
	} else if (ctrl->tCK == TCK_700MHZ) {
		ctrl->edge_offset[0] = 13; //XXX: guessed
		ctrl->edge_offset[1] = 5;
		ctrl->edge_offset[2] = 5;
		ctrl->timC_offset[0] = 14; //XXX: guessed
		ctrl->timC_offset[1] = 5;
		ctrl->timC_offset[2] = 5;
		ctrl->reg_320c_range_threshold = 16;
	} else if (ctrl->tCK == TCK_666MHZ) {
		ctrl->edge_offset[0] = 10;
		ctrl->edge_offset[1] = 4;
		ctrl->edge_offset[2] = 4;
		ctrl->timC_offset[0] = 11;
		ctrl->timC_offset[1] = 4;
		ctrl->timC_offset[2] = 4;
		ctrl->reg_320c_range_threshold = 16;
	} else if (ctrl->tCK == TCK_533MHZ) {
		ctrl->edge_offset[0] = 8;
		ctrl->edge_offset[1] = 3;
		ctrl->edge_offset[2] = 3;
		ctrl->timC_offset[0] = 9;
		ctrl->timC_offset[1] = 3;
		ctrl->timC_offset[2] = 3;
		ctrl->reg_320c_range_threshold = 17;
	} else  { /* TCK_400MHZ */
		ctrl->edge_offset[0] = 6;
		ctrl->edge_offset[1] = 2;
		ctrl->edge_offset[2] = 2;
		ctrl->timC_offset[0] = 6;
		ctrl->timC_offset[1] = 2;
		ctrl->timC_offset[2] = 2;
		ctrl->reg_320c_range_threshold = 17;
	}

	/* Initial phase between CLK/CMD pins */
	ctrl->reg_c14_offset = (256000 / ctrl->tCK) / 66;

	/* DLL_CONFIG_MDLL_W_TIMER */
	ctrl->reg_5064b0 = (128000 / ctrl->tCK) + 3;

	if (ctrl->tCWL)
		ctrl->CWL = DIV_ROUND_UP(ctrl->tCWL, ctrl->tCK);
	else
		ctrl->CWL = get_CWL(ctrl->tCK);
	printk(BIOS_DEBUG, "Selected CWL latency   : %uT\n", ctrl->CWL);

	/* Find tRCD */
	ctrl->tRCD = DIV_ROUND_UP(ctrl->tRCD, ctrl->tCK);
	printk(BIOS_DEBUG, "Selected tRCD          : %uT\n", ctrl->tRCD);

	ctrl->tRP = DIV_ROUND_UP(ctrl->tRP, ctrl->tCK);
	printk(BIOS_DEBUG, "Selected tRP           : %uT\n", ctrl->tRP);

	/* Find tRAS */
	ctrl->tRAS = DIV_ROUND_UP(ctrl->tRAS, ctrl->tCK);
	printk(BIOS_DEBUG, "Selected tRAS          : %uT\n", ctrl->tRAS);

	/* Find tWR */
	ctrl->tWR = DIV_ROUND_UP(ctrl->tWR, ctrl->tCK);
	printk(BIOS_DEBUG, "Selected tWR           : %uT\n", ctrl->tWR);

	/* Find tFAW */
	ctrl->tFAW = DIV_ROUND_UP(ctrl->tFAW, ctrl->tCK);
	printk(BIOS_DEBUG, "Selected tFAW          : %uT\n", ctrl->tFAW);

	/* Find tRRD */
	ctrl->tRRD = DIV_ROUND_UP(ctrl->tRRD, ctrl->tCK);
	printk(BIOS_DEBUG, "Selected tRRD          : %uT\n", ctrl->tRRD);

	/* Find tRTP */
	ctrl->tRTP = DIV_ROUND_UP(ctrl->tRTP, ctrl->tCK);
	printk(BIOS_DEBUG, "Selected tRTP          : %uT\n", ctrl->tRTP);

	/* Find tWTR */
	ctrl->tWTR = DIV_ROUND_UP(ctrl->tWTR, ctrl->tCK);
	printk(BIOS_DEBUG, "Selected tWTR          : %uT\n", ctrl->tWTR);

	/* Refresh-to-Active or Refresh-to-Refresh (tRFC) */
	ctrl->tRFC = DIV_ROUND_UP(ctrl->tRFC, ctrl->tCK);
	printk(BIOS_DEBUG, "Selected tRFC          : %uT\n", ctrl->tRFC);

	ctrl->tREFI = get_REFI(ctrl->tCK, ctrl->base_freq);
	ctrl->tMOD = get_MOD(ctrl->tCK, ctrl->base_freq);
	ctrl->tXSOffset = get_XSOffset(ctrl->tCK, ctrl->base_freq);
	ctrl->tWLO = get_WLO(ctrl->tCK, ctrl->base_freq);
	ctrl->tCKE = get_CKE(ctrl->tCK, ctrl->base_freq);
	ctrl->tXPDLL = get_XPDLL(ctrl->tCK, ctrl->base_freq);
	ctrl->tXP = get_XP(ctrl->tCK, ctrl->base_freq);
	ctrl->tAONPD = get_AONPD(ctrl->tCK, ctrl->base_freq);
}

static void dram_freq(ramctr_timing * ctrl)
{
	if (ctrl->tCK > TCK_400MHZ) {
		printk (BIOS_ERR, "DRAM frequency is under lowest supported "
				"frequency (400 MHz). Increasing to 400 MHz as last resort");
		ctrl->tCK = TCK_400MHZ;
	}

	while (1) {
		u8 val2;
		u32 reg1 = 0;

		/* Step 1 - Set target PCU frequency */
		find_cas_tck(ctrl);

		/* Frequency multiplier.  */
		u32 FRQ = get_FRQ(ctrl->tCK, ctrl->base_freq);

		/* The PLL will never lock if the required frequency is
		 * already set. Exit early to prevent a system hang.
		 */
		reg1 = MCHBAR32(MC_BIOS_DATA);
		val2 = (u8) reg1;
		if (val2)
			return;

		/* Step 2 - Select frequency in the MCU */
		reg1 = FRQ;
		if (ctrl->base_freq == 100)
			reg1 |= 0x100; /* Enable 100Mhz REF clock */
		reg1 |= 0x80000000;	// set running bit
		MCHBAR32(MC_BIOS_REQ) = reg1;
		int i=0;
		printk(BIOS_DEBUG, "PLL busy... ");
		while (reg1 & 0x80000000) {
			udelay(10);
			i++;
			reg1 = MCHBAR32(MC_BIOS_REQ);
		}
		printk(BIOS_DEBUG, "done in %d us\n", i * 10);

		/* Step 3 - Verify lock frequency */
		reg1 = MCHBAR32(MC_BIOS_DATA);
		val2 = (u8) reg1;
		if (val2 >= FRQ) {
			printk(BIOS_DEBUG, "MCU frequency is set at : %d MHz\n",
			       (1000 << 8) / ctrl->tCK);
			return;
		}
		printk(BIOS_DEBUG, "PLL didn't lock. Retrying at lower frequency\n");
		ctrl->tCK++;
	}
}

static void dram_ioregs(ramctr_timing * ctrl)
{
	u32 reg, comp2;

	int channel;

	// IO clock
	FOR_ALL_CHANNELS {
		MCHBAR32(0xc00 + 0x100 * channel) = ctrl->rankmap[channel];
	}

	// IO command
	FOR_ALL_CHANNELS {
		MCHBAR32(0x3200 + 0x100 * channel) = ctrl->rankmap[channel];
	}

	// IO control
	FOR_ALL_POPULATED_CHANNELS {
		program_timings(ctrl, channel);
	}

	// Rcomp
	printram("RCOMP...");
	reg = 0;
	while (reg == 0) {
		reg = MCHBAR32(0x5084) & 0x10000;
	}
	printram("done\n");

	// Set comp2
	comp2 = get_COMP2(ctrl->tCK, ctrl->base_freq);
	MCHBAR32(0x3714) = comp2;
	printram("COMP2 done\n");

	// Set comp1
	FOR_ALL_POPULATED_CHANNELS {
		reg = MCHBAR32(0x1810 + channel * 0x100);	//ch0
		reg = (reg & ~0xe00) | (1 << 9);	//odt
		reg = (reg & ~0xe00000) | (1 << 21);	//clk drive up
		reg = (reg & ~0x38000000) | (1 << 27);	//ctl drive up
		MCHBAR32(0x1810 + channel * 0x100) = reg;
	}
	printram("COMP1 done\n");

	printram("FORCE RCOMP and wait 20us...");
	MCHBAR32(0x5f08) |= 0x100;
	udelay(20);
	printram("done\n");
}

int try_init_dram_ddr3_ivy(ramctr_timing *ctrl, int fast_boot,
		int s3_resume, int me_uma_size)
{
	int err;

	printk(BIOS_DEBUG, "Starting Ivybridge RAM training (%d).\n",
		   fast_boot);

	if (!fast_boot) {
		/* Find fastest common supported parameters */
		dram_find_common_params(ctrl);

		dram_dimm_mapping(ctrl);
	}

	/* Set MCU frequency */
	dram_freq(ctrl);

	if (!fast_boot) {
		/* Calculate timings */
		dram_timing(ctrl);
	}

	/* Set version register */
	MCHBAR32(0x5034) = 0xC04EB002;

	/* Enable crossover */
	dram_xover(ctrl);

	/* Set timing and refresh registers */
	dram_timing_regs(ctrl);

	/* Power mode preset */
	MCHBAR32(0x4e80) = 0x5500;

	/* Set scheduler parameters */
	MCHBAR32(0x4c20) = 0x10100005;

	/* Set CPU specific register */
	set_4f8c();

	/* Clear IO reset bit */
	MCHBAR32(0x5030) &= ~0x20;

	/* Set MAD-DIMM registers */
	dram_dimm_set_mapping(ctrl);
	printk(BIOS_DEBUG, "Done dimm mapping\n");

	/* Zone config */
	dram_zones(ctrl, 1);

	/* Set memory map */
	dram_memorymap(ctrl, me_uma_size);
	printk(BIOS_DEBUG, "Done memory map\n");

	/* Set IO registers */
	dram_ioregs(ctrl);
	printk(BIOS_DEBUG, "Done io registers\n");

	udelay(1);

	if (fast_boot) {
		restore_timings(ctrl);
	} else {
		/* Do jedec ddr3 reset sequence */
		dram_jedecreset(ctrl);
		printk(BIOS_DEBUG, "Done jedec reset\n");

		/* MRS commands */
		dram_mrscommands(ctrl);
		printk(BIOS_DEBUG, "Done MRS commands\n");

		/* Prepare for memory training */
		prepare_training(ctrl);

		err = read_training(ctrl);
		if (err)
			return err;

		err = write_training(ctrl);
		if (err)
			return err;

		printram("CP5a\n");

		err = discover_edges(ctrl);
		if (err)
			return err;

		printram("CP5b\n");

		err = command_training(ctrl);
		if (err)
			return err;

		printram("CP5c\n");

		err = discover_edges_write(ctrl);
		if (err)
			return err;

		err = discover_timC_write(ctrl);
		if (err)
			return err;

		normalize_training(ctrl);
	}

	set_4008c(ctrl);

	write_controller_mr(ctrl);

	if (!s3_resume) {
		err = channel_test(ctrl);
		if (err)
			return err;
	}

	return 0;
}
