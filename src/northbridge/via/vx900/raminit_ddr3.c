/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011-2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#include "early_vx900.h"
#include "raminit.h"
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <delay.h>
#include <string.h>

#define min(a,b) a<b?a:b
#define max(a,b) a>b?a:b

#define MCU PCI_DEV(0, 0, 3)

/* Map BA0 to A17, BA1 to A18 */
/* Map BA2 to A19, RA0/RA1 must not overlap BA[0:2] */
#define VX900_MRS_MA_MAP   0x4b33       /* MA Pin Mapping for MRS commands */
#define VX900_CALIB_MA_MAP 0x5911       /* MA Pin mapping for calibrations */


/* FIXME: Really Alex, is this all you can come up with? */
void udelay(unsigned usecs)
{
	int i;
	for(i = 0; i < usecs; i++)
		inb(0x80);
}

/* Registers 0x78 -> 0x7f contain calibration the settings for DRAM IO timing
 * The dataset in these registers is selected from 0x70.
 * Once the correct dataset is selected the delays can be altered.
 *   mode refers to TxDQS, TxDQ, RxDQS, or RxCR
 *   setting refers to either manual, average, upper bound, or lower bound
 */
#define VX900_CALIB_TxDQS		0
#define VX900_CALIB_TxDQ		1
#define VX900_CALIB_RxDQS		2
#define VX900_CALIB_RxDQ_CR		3

#define VX900_CALIB_AVERAGE		0
#define VX900_CALIB_LOWER		1
#define VX900_CALIB_UPPER		2
#define VX900_CALIB_MANUAL		4

static void vx900_delay_calib_mode_select(u8 delay_type, u8 bound)
{
	/* Which calibration setting */
	u8 reg8 = (delay_type & 0x03) << 2;
	/* Upper, lower, average, or manual setting */
	reg8 |= (bound & 0x03);
	pci_write_config8(MCU, 0x70, reg8);
}

static void vx900_read_0x78_0x7f(timing_dly dly)
{
	*((u32*) (&(dly[0]))) = pci_read_config32(MCU, 0x78);
	*((u32*) (&(dly[4]))) = pci_read_config32(MCU, 0x7c);
}

static void vx900_write_0x78_0x7f(const timing_dly dly)
{
	pci_write_config32(MCU, 0x78, *((u32*) (&(dly[0]))) );
	pci_write_config32(MCU, 0x7c, *((u32*) (&(dly[4]))) );
}

static void vx900_read_delay_range(delay_range *d_range, u8 mode)
{
	vx900_delay_calib_mode_select(mode, VX900_CALIB_LOWER);
	vx900_read_0x78_0x7f(d_range->low);
	vx900_delay_calib_mode_select(mode, VX900_CALIB_AVERAGE);
	vx900_read_0x78_0x7f(d_range->avg);
	vx900_delay_calib_mode_select(mode, VX900_CALIB_UPPER);
	vx900_read_0x78_0x7f(d_range->high);
}

static void dump_delay(const timing_dly dly)
{
	u8 i;
	for(i = 0; i < 8; i ++)
	{
		printram(" %.2x", dly[i]);
	}
	printram("\n");
}

static void dump_delay_range(const delay_range d_range)
{
	printram("Lower limit: ");
	dump_delay(d_range.low);
	printram("Average:     ");
	dump_delay(d_range.avg);
	printram("Upper limit: ");
	dump_delay(d_range.high);
}

/* These are some "safe" values that can be used for memory initialization.
 * Some will stay untouched, and others will be overwritten later on
 * YOU REALLY NEED THE DATASHEET TO UNDERSTAND THESE !!! */
static pci_reg8 mcu_init_config[] = {
	{0x40, 0x01}, /* Virtual rank 0 ending address = 64M - 1 */
	{0x41, 0x00}, {0x42, 0x00}, {0x43, 0x00}, /* Virtual Ranks ending */
	{0x48, 0x00}, /* Virtual rank 0 starting address = 0 */
	{0x49, 0x00}, {0x4a, 0x00}, {0x4b, 0x00}, /* Virtual Ranks beginning */
	{0x50, 0xd8}, /* Set ranks 0-3 to 11 col bits, 16 row bits */
	/* Disable all virtual ranks */
	{0x54, 0x00}, {0x55, 0x00}, {0x56, 0x00}, {0x57, 0x00},
	/* Disable rank interleaving in ranks 0-3 */
	{0x58, 0x00}, {0x59, 0x00}, {0x5a, 0x00}, {0x5b, 0x00},
	{0x6c, 0xA0}, /* Memory type: DDR3, VDIMM: 1.5V, 64-bit DRAM */
	{0xc4, 0x80}, /* Enable 8 memory banks */
	{0xc6, 0x80}, /* Minimum latency from self-refresh. Bit [7] must be 1 */
	/* FIXME: do it here or in Final config? */
	{0xc8, 0x80}, /* Enable automatic triggering of short ZQ calibration */
	{0x99, 0xf0}, /* Power Management and Bypass Reorder Queue */
	/* Enable differential DQS; MODT assertion values suggested in DS */
	{0x9e, 0xa1}, {0x9f, 0x51},
	/* DQ/DQM Duty Control - Do not put any extra delays*/
	{0xe9, 0x00}, {0xea, 0x00}, {0xeb, 0x00}, {0xec, 0x00},
	{0xed, 0x00}, {0xee, 0x00}, {0xef, 0x00},
	{0xfc, 0x00}, {0xfd, 0x00}, {0xfe, 0x00}, {0xff, 0x00},
	/* The following parameters we may or may not change */
	{0x61, 0x2e}, /* DRAMC Pipeline Control */
	{0x77, 0x10}, /* MDQS Output Control */

	/* The following are parameters we'll most likely never change again */
	{0x60, 0xf4}, /* DRAM Pipeline Turn-Around Setting */
	{0x65, 0x49}, /* DRAM Arbitration Bandwidth Timer - I */
	{0x66, 0x80}, /* DRAM Queue / Arbitration */
	{0x69, 0xc6}, /* Bank Control: 8 banks, high priority refresh */
	{0x6a, 0xfc}, /* DRAMC Request Reorder Control */
	{0x6e, 0x38}, /* Burst lenght: 8, burst-chop: enable */
	{0x73, 0x04}, /* Close All Pages Threshold */

	/* The following need to be dynamically asserted */
	/* See: check_special_registers.c */
	{0x74, 0xa0}, /* Yes, same 0x74; add one more T */
	{0x76, 0x60}, /* Write Data Phase Control */

};

/* This table keeps the driving strength control setting that we can safely use
 * doring initialization. */
static pci_reg8 mcu_drv_ctrl_config[] = {
	{0xd3, 0x03}, /* Enable auto-compensation circuit for ODT strength */
	{0xd4, 0x80}, /* Set internal ODT to dynamically turn on or off */
	{0xd6, 0x20}, /* Enable strong driving for MA and DRAM commands*/
	{0xd0, 0x88}, /* (ODT) Strength ?has effect? */
	{0xe0, 0x88}, /* DRAM Driving – Group DQS (MDQS) */
	{0xe1, 0x00}, /* Disable offset mode for driving strength control */
	{0xe2, 0x88}, /* DRAM Driving – Group DQ (MD, MDQM) */
	{0xe4, 0xcc}, /* DRAM Driving – Group CSA (MCS, MCKE, MODT) */
	{0xe8, 0x88}, /* DRAM Driving – Group MA (MA, MBA, MSRAS, MSCAS, MSWE)*/
	{0xe6, 0xff}, /* DRAM Driving – Group DCLK0 (DCLK[2:0] for DIMM0) */
	{0xe7, 0xff}, /* DRAM Driving – Group DCLK1 (DCLK[5:3] for DIMM1) */
	{0xe4, 0xcc}, /* DRAM Driving – Group CSA (MCS, MCKE, MODT)*/
	{0x91, 0x08}, /* MCLKO Output Phase Delay - I */
	{0x92, 0x08}, /* MCLKO Output Phase Delay - II */
	{0x93, 0x16}, /* CS/CKE Output Phase Delay */
	{0x95, 0x16}, /* SCMD/MA Output Phase Delay */
	{0x9b, 0x3f}, /* Memory Clock Output Enable */
};

static void vx900_dram_set_ma_map(u16 map)
{
	pci_write_config16(MCU, 0x52, map);
}

static void vx900_dram_write_init_config(void)
{
	size_t i;
	for(i = 0; i < (sizeof(mcu_init_config)/sizeof(pci_reg8)); i++)
	{
		pci_write_config8(MCU, mcu_init_config[i].addr,
				  mcu_init_config[i].val);
	}
	vx900_dram_set_ma_map(VX900_CALIB_MA_MAP);
}

static void dram_find_spds_ddr3(const dimm_layout *addr, dimm_info *dimm)
{
	size_t i = 0;
	int dimms = 0;
	do {
		spd_raw_data spd;
		spd_read(addr->spd_addr[i], spd);
		spd_decode_ddr3(&dimm->dimm[i], spd);
		if(dimm->dimm[i].dram_type != DRAM_TYPE_DDR3) continue;
		dimms++;
		dram_print_spd_ddr3(&dimm->dimm[i]);
	} while(addr->spd_addr[++i] != SPD_END_LIST
		&& i < VX900_MAX_DIMM_SLOTS);

	if(!dimms)
		die("No DIMMs were found");
}

static void dram_find_common_params(const dimm_info *dimms, ramctr_timing *ctrl)
{
	size_t  i, valid_dimms;
	memset(ctrl, 0, sizeof(ramctr_timing));
	ctrl->cas_supported = 0xff;
	valid_dimms = 0;
	for(i = 0; i < VX900_MAX_DIMM_SLOTS; i++)
	{
		const dimm_attr *dimm = &dimms->dimm[i];
		if(dimm->dram_type == DRAM_TYPE_UNDEFINED) continue;
		valid_dimms++;

		if(valid_dimms == 1) {
			/* First DIMM defines the type of DIMM */
			ctrl->dram_type = dimm->dram_type;
		} else {
			/* Check if we have mismatched DIMMs */
			if(ctrl->dram_type != dimm->dram_type)
				die("Mismatched DIMM Types");
		}
		/* Find all possible CAS combinations */
		ctrl->cas_supported &= dimm->cas_supported;

		/* Find the smallest common latencies supported by all DIMMs */
		ctrl->tCK  = max(ctrl->tCK,  dimm->tCK );
		ctrl->tAA  = max(ctrl->tAA,  dimm->tAA );
		ctrl->tWR  = max(ctrl->tWR,  dimm->tWR );
		ctrl->tRCD = max(ctrl->tRCD, dimm->tRCD);
		ctrl->tRRD = max(ctrl->tRRD, dimm->tRRD);
		ctrl->tRP  = max(ctrl->tRP,  dimm->tRP );
		ctrl->tRAS = max(ctrl->tRAS, dimm->tRAS);
		ctrl->tRC  = max(ctrl->tRC,  dimm->tRC );
		ctrl->tRFC = max(ctrl->tRFC, dimm->tRFC);
		ctrl->tWTR = max(ctrl->tWTR, dimm->tWTR);
		ctrl->tRTP = max(ctrl->tRTP, dimm->tRTP);
		ctrl->tFAW = max(ctrl->tFAW, dimm->tFAW);

	}

	if(!ctrl->cas_supported) die("Unsupported DIMM combination. "
		"DIMMS do not support common CAS latency");
	if(!valid_dimms) die("No valid DIMMs found");
}

static void vx900_dram_phys_bank_range(const dimm_info *dimms,
				       rank_layout *ranks)
{
	size_t i;
	for(i = 0; i < VX900_MAX_DIMM_SLOTS; i ++)
	{
		if(dimms->dimm[i].dram_type == DRAM_TYPE_UNDEFINED)
			continue;
		u8 nranks = dimms->dimm[i].ranks;
		if(nranks > 2)
			die("Found DIMM with more than two ranks, which is not "
			"supported by this chipset");
		u32 size = dimms->dimm[i].size;
		if(nranks == 2) {
			/* Each rank holds half the capacity of the DIMM */
			size >>= 1;
			ranks->phys_rank_size[i<<1] = size;
			ranks->phys_rank_size[(i<<1) | 1] = size;
		} else {
			/* Otherwise, everything is held in the first bank */
			ranks->phys_rank_size[i<<1] = size;
			ranks->phys_rank_size[(i<<1) | 1] = 0;;
		}
	}
}

static void vx900_dram_driving_ctrl(const dimm_info *dimm)
{
	size_t i, ndimms;
	u8 val;

	/* For ODT range selection, datasheet recommends
	 * when 1 DIMM  present:  60 Ohm
	 * when 2 DIMMs present: 120 Ohm  */
	ndimms = 0;
	for(i = 0; i < VX900_MAX_DIMM_SLOTS; i++) {
		if(dimm->dimm[i].dram_type == DRAM_TYPE_DDR3) ndimms++;
	}
	val = (ndimms > 1) ? 0x0 : 0x1;
	pci_write_config8(MCU, 0xd5, val << 2);


	/* FIXME: Assert dynamically based on dimm config */
	/* DRAM ODT Lookup Table*/
	pci_write_config8(MCU, 0x9c, 0xe4);

	for(i = 0; i < (sizeof(mcu_drv_ctrl_config)/sizeof(pci_reg8)); i++)
	{
		pci_write_config8(MCU, mcu_drv_ctrl_config[i].addr,
				  mcu_drv_ctrl_config[i].val);
	}
}

static void vx900_clear_vr_map(void)
{
	/* Disable all ranks */
	pci_write_config16(MCU, 0x54, 0x0000);
}

static void vx900_pr_map_all_vr3(void)
{
	/* Enable all ranks and set them to VR3 */
	pci_write_config16(MCU, 0x54, 0xbbbb);
}
/* Map physical rank pr to virtual rank vr */
static void vx900_map_pr_vr(u8 pr, u8 vr)
{
	pr &= 0x3; vr &= 0x3;
	/* Enable rank (bit [3], and set the VR number bits [1:0] */
	u16 val = 0x8 | vr;
	/* Now move the value to the appropriate PR */
	val <<= (pr * 4);
	pci_mod_config16(MCU, 0x54, 0xf << (pr * 4), val);
	printram("Mapping PR %u to VR %u\n", pr, vr);
}

static u8 vx900_get_CWL(u8 CAS)
{
	/* Get CWL based on CAS using the following rule:
	 *       _________________________________________
	 * CAS: | 4T | 5T | 6T | 7T | 8T | 9T | 10T | 11T |
	 * CWL: | 5T | 5T | 5T | 6T | 6T | 7T |  7T |  8T |
	 */
	static const u8 cas_cwl_map[] = {5, 5, 5, 6, 6, 7, 7, 8};
	if(CAS > 11) return 8;
	return cas_cwl_map[CAS - 4];
}

static void vx900_dram_timing(ramctr_timing *ctrl)
{
	/* Here we are calculating latencies, and writing them to the appropiate
	 * registers. Some registers do not take latencies from 0T, for example:
	 * CAS: 000 = 4T, 001 = 5T, 010 = 6T, etc
	 * In this example we subtract 4T from the result for CAS: (val - 4)
	 * The & 0x07 after (val - T0) just makes sure that, no matter what
	 * crazy thing may happen, we do not write outside the bits allocated
	 * in the register */
	u8 reg8, val, tFAW, tRRD;
	u32 val32;

	/* Maximum supported DDR3 frequency is 533MHz (DDR3 1066)
	 * so make sure we cap it if we have faster DIMMs */
	if(ctrl->tCK < TCK_533MHZ) ctrl->tCK = TCK_533MHZ;
	val32 = (1000 << 8) / ctrl->tCK;
	printram("Selected DRAM frequency: %u MHz\n", val32);

	/* Now find the right DRAM frequency setting,
	 * and align it to the closest JEDEC standard frequency */
	if(ctrl->tCK <= TCK_533MHZ)      {val = 0x07; ctrl->tCK = TCK_533MHZ;}
	else if(ctrl->tCK <= TCK_400MHZ) {val = 0x06; ctrl->tCK = TCK_400MHZ;}
	else if(ctrl->tCK <= TCK_333MHZ) {val = 0x05; ctrl->tCK = TCK_333MHZ;}
	else if(ctrl->tCK <= TCK_266MHZ) {val = 0x04; ctrl->tCK = TCK_266MHZ;}

	/* Find CAS and CWL latencies */
	val = (ctrl->tAA + ctrl->tCK -1) / ctrl->tCK;
	printram("Minimum  CAS latency   : %uT\n", val);
	/* Find lowest supported CAS latency that satisfies the minimum value */
	while( !((ctrl->cas_supported >> (val-4))&1)
		&& (ctrl->cas_supported >> (val-4))) {
		val++;
	}
	/* Is CAS supported */
	if(!(ctrl->cas_supported & (1 << (val-4))) )
		printram("CAS not supported\n");
	printram("Selected CAS latency   : %uT\n", val);
	ctrl->CAS = val;
	ctrl->CWL = vx900_get_CWL(ctrl->CAS);
	printram("Selected CWL latency   : %uT\n", ctrl->CWL);
	/* Write CAS and CWL */
	reg8 = ( ((ctrl->CWL - 4) &0x07) << 4 ) | ((ctrl->CAS - 4) & 0x07);
	pci_write_config8(MCU, 0xc0, reg8);

	/* Find tRCD */
	val = (ctrl->tRCD + ctrl->tCK -1) / ctrl->tCK;
	printram("Selected tRCD          : %uT\n", val);
	reg8 = ((val-4) & 0x7) << 4;
	/* Find tRP */
	val = (ctrl->tRP + ctrl->tCK -1) / ctrl->tCK;
	printram("Selected tRP           : %uT\n", val);
	reg8 |= ((val-4) & 0x7);
	pci_write_config8(MCU, 0xc1, reg8);

	/* Find tRAS */
	val = (ctrl->tRAS + ctrl->tCK -1) / ctrl->tCK;
	printram("Selected tRAS          : %uT\n", val);
	reg8 = ((val-15) & 0x7) << 4;
	/* Find tWR */
	ctrl->WR = (ctrl->tWR + ctrl->tCK -1) / ctrl->tCK;
	printram("Selected tWR           : %uT\n", ctrl->WR);
	reg8 |= ((ctrl->WR-4) & 0x7);
	pci_write_config8(MCU, 0xc2, reg8);

	/* Find tFAW */
	tFAW = (ctrl->tFAW + ctrl->tCK -1) / ctrl->tCK;
	printram("Selected tFAW          : %uT\n", tFAW);
	/* Find tRRD */
	tRRD = (ctrl->tRRD + ctrl->tCK -1) / ctrl->tCK;
	printram("Selected tRRD          : %uT\n", tRRD);
	val = tFAW - 4*tRRD;	/* number of cycles above 4*tRRD */
	reg8 = ((val-0) & 0x7) << 4;
	reg8 |= ((tRRD-2) & 0x7);
	pci_write_config8(MCU, 0xc3, reg8);

	/* Find tRTP */
	val = (ctrl->tRTP + ctrl->tCK -1) / ctrl->tCK;
	printram("Selected tRTP          : %uT\n", val);
	reg8 = ((val & 0x3) << 4);
	/* Find tWTR */
	val = (ctrl->tWTR + ctrl->tCK -1) / ctrl->tCK;
	printram("Selected tWTR          : %uT\n", val);
	reg8 |= ((val - 2) & 0x7);
	pci_mod_config8(MCU, 0xc4, 0x3f, reg8);

	/* DRAM Timing for All Ranks - VI
	 * [7:6] CKE Assertion Minimum Pulse Width
	 *     We probably don't want to mess with this just yet.
	 * [5:0] Refresh-to-Active or Refresh-to-Refresh (tRFC)
	 *     tRFC = (30 + 2 * [5:0])T
	 *     Since we previously set RxC4[7]
	 */
	reg8 = pci_read_config8(MCU, 0xc5);
	val = (ctrl->tRFC + ctrl->tCK -1) / ctrl->tCK;
	printram("Minimum  tRFC          : %uT\n", val);
	if(val < 30) {
		val = 0;
	} else {
		val = (val -30 + 1 ) / 2;
	}
	;
	printram("Selected tRFC          : %uT\n", 30 + 2 * val);
	reg8 |= (val & 0x3f);
	pci_write_config8(MCU, 0xc5, reg8);

	/* Where does this go??? */
	val = (ctrl->tRC + ctrl->tCK -1) / ctrl->tCK;
	printram("Required tRC           : %uT\n", val);
}

static void vx900_dram_freq(ramctr_timing *ctrl)
{
	u8 val;

	/* Program the DRAM frequency */

	/* Step 1 - Reset the PLL */
	pci_mod_config8(MCU, 0x90, 0x00, 0x0f);
	/* Wait at least 10 ns; VIA code delays by 640us */
	udelay(640);

	/* Step 2 - Set target frequency */
	if(ctrl->tCK <= TCK_533MHZ)      {val = 0x07; ctrl->tCK = TCK_533MHZ;}
	else if(ctrl->tCK <= TCK_400MHZ) {val = 0x06; ctrl->tCK = TCK_400MHZ;}
	else if(ctrl->tCK <= TCK_333MHZ) {val = 0x05; ctrl->tCK = TCK_333MHZ;}
	else /*ctrl->tCK <= TCK_266MHZ*/ {val = 0x04; ctrl->tCK = TCK_266MHZ;}
	/* Restart the PLL with the desired frequency */
	pci_mod_config8(MCU, 0x90, 0x0f, val);

	/* Step 3 - Wait for PLL to stabilize */
	udelay(2000);

	/* Step 4 - Reset the DLL - Clear [7,4]*/
	pci_mod_config8(MCU, 0x6b, 0x90, 0x00);
	udelay(2000);

	/* Step 5 - Enable the DLL - Set bits [7,4] to 01b*/
	pci_mod_config8(MCU, 0x6b, 0x00, 0x10);
	udelay(2000);

	/* Step 6 - Start DLL Calibration - Set bit [7] */
	pci_mod_config8(MCU, 0x6b, 0x00, 0x80);
	udelay(5);

	/* Step 7 - Finish DLL Calibration - Clear bit [7]*/
	pci_mod_config8(MCU, 0x6b, 0x80, 0x00);

	/* Step 8 - If we have registered DIMMs, we need to set bit[0] */
	if(dimm_is_registered(ctrl->dram_type)){
		printram("Enabling RDIMM support in memory controller\n");
		pci_mod_config8(MCU, 0x6c, 0x00, 0x01);
	}
}

/* The VX900 can send the MRS commands directly through hardware */
void vx900_dram_ddr3_do_hw_mrs(u8 ma_swap, u8 rtt_nom,
				      u8 ods, u8 rtt_wr, u8 srt, u8 asr);
/*static*/ void vx900_dram_ddr3_do_hw_mrs(u8 ma_swap, u8 rtt_nom,
				      u8 ods, u8 rtt_wr, u8 srt, u8 asr)
{
	u8 reg8 = 0;
	u32 reg32;
	if(asr) reg8 |= (1 << 0);
	if(srt) reg8 |= (1 << 1);
	reg8 |= ((rtt_wr & 0x03) << 4);
	pci_write_config8(MCU, 0xcd, reg8);
	reg8 = 1;
	if(ma_swap) reg8 |= (1 << 1);
	reg8 |= ((ods & 0x03) << 2);
	reg8 |= ((rtt_nom & 0x7) << 4);
	pci_write_config8(MCU, 0xcc, reg8);
	reg32=0;
	while(pci_read_config8(MCU, 0xcc) & 1) reg32++;
	printram(" Waited %u PCI cycles for HW MRS\n", reg32);
}
#include "forgotten.h"
#include "forgotten.c"

static void vx900_dram_send_soft_mrs(u8 type, u16 cmd)
{
	u32 addr;
	/* Set Fun3_RX6B[2:0] to 011b (MSR Enable). */
	pci_mod_config8(MCU, 0x6b, 0x07, (3<<0));
	/* Find the address corresponding to the MRS */
	addr = vx900_get_mrs_addr(type, cmd);
	/* Execute the MRS */
	volatile_read(addr);
	/* Set Fun3_Rx6B[2:0] to 000b (Normal SDRAM Mode). */
	pci_mod_config8(MCU, 0x6b, 0x07, 0x00);
}

static void vx900_dram_ddr3_dimm_init(const ramctr_timing *ctrl,
				      const rank_layout *ranks)
{
	size_t i;

	/* Set BA[0/1/2] to [A17/18/19] */
	vx900_dram_set_ma_map(VX900_MRS_MA_MAP);

	/* Step 01 - Set Fun3_Rx6E[5] to 1b to support burst length. */
	pci_mod_config8(MCU, 0x6e, 0, 1<<5);
	/* Step 02 - Set Fun3_RX69[0] to 0b (Disable Multiple Page Mode). */
	pci_mod_config8(MCU, 0x69, (1<<0), 0x00);
	/* And set [7:6] to 10b ?*/
	pci_write_config8(MCU, 0x69, 0x87);

	/* Step 03 - Set the target physical rank to virtual rank0 and other
	 * ranks to virtual rank3. */
	vx900_pr_map_all_vr3();

	/* Step 04 - Set Fun3_Rx50 to D8h. */
	pci_write_config8(MCU, 0x50, 0xd8);
	/* Step 05 - Set Fun3_RX6B[5] to 1b to de-assert RESET# and wait for at
	 * least 500 us. */
	pci_mod_config8(MCU, 0x6b, 0x00, (1<<5) );
	udelay(500);

	/* Step 6 -> 15 - Set the target physical rank to virtual rank 0 and
	 * other ranks to virtual rank 3.
	 * Repeat Step 6 to 14 for every rank present, then jump to Step 16. */
	for(i = 0; i < VX900_MAX_MEM_RANKS; i++)
	{
		if(ranks->phys_rank_size[i] == 0) continue;
		printram("Initializing rank %lu\n", i);

		/* Set target physical rank to virtual rank 0
		 * other ranks to virtual rank 3*/
		vx900_map_pr_vr(i, 0);

#define USE_HW_INIT_SEQUENCE 1
#if USE_HW_INIT_SEQUENCE

		/* FIXME: Is this needed on HW init? */
		pci_mod_config8(MCU, 0x6b, 0x07, 0x01);	/* Enable NOP */
		volatile_read(0x0);			/* Do NOP */
		pci_mod_config8(MCU, 0x6b, 0x07, 0x03);	/* MSR Enable */

		/* FIXME: Values dependent on DIMM setup
		 * This works for 1 DIMM
		 * See init_dram_by_rank.c and get_basic_information.c
		 * in the VIA provided code */
		const u8 rtt_nom = DDR3_MR1_RTT_NOM_RZQ2;
		const u8 ods = DDR3_MR1_ODS_RZQ6;
		const u8 rtt_wr = DDR3_MR2_RTT_WR_OFF;

		printram("Using Hardware method\n");
		/* FIXME: MA swap dependent on module */
		vx900_dram_ddr3_do_hw_mrs(0, rtt_nom, ods, rtt_wr, 0, 0);

		/* Normal SDRAM Mode */
		pci_mod_config8(MCU, 0x6b, 0x07, 0x00);

#else  /* USE_HW_INIT_SEQUENCE */
		printram("Using software method\n");
		vx900_dram_ddr3_init_rank(MCU, ctrl, i);
#endif /* USE_HW_INIT_SEQUENCE */
		/* Step 15, set the rank to virtual rank 3*/
		vx900_map_pr_vr(i, 3);
	}

	/* Step 16 – Set Fun3_Rx6B[2:0] to 000b (Normal SDRAM Mode). */
	pci_mod_config8(MCU, 0x6b, 0x07, 0x00);

	/* Set BA[0/1/2] to [A13/14/15] */
	vx900_dram_set_ma_map(VX900_CALIB_MA_MAP);

	/* Step 17 – Set Fun3_Rx69[0] to 1b (Enable Multiple Page Mode). */
	pci_mod_config8(MCU, 0x69, 0x00, (1<<0) );

	printram("DIMM initialization sequence complete\n");
}

static void vx900_dram_enter_read_leveling(void)
{
	/* Precharge all before issuing read leveling MRS to DRAM */
	pci_mod_config8(MCU, 0x06b, 0x07, 0x02);
	volatile_read(0x0);
	udelay(1000);

	/* Enable read leveling: Set D0F3Rx71[7]=1 */
	pci_mod_config8(MCU, 0x71, 0x40, 0x80);

	/* Put DRAM in read leveling mode */
	u16 cmd = ddr3_get_mr3(1);
	vx900_dram_send_soft_mrs(3, cmd);
}

static void vx900_dram_exit_read_leveling(void)
{
	/* Disable read leveling, and put dram in normal operation mode */
	u16 cmd = ddr3_get_mr3(0);
	vx900_dram_send_soft_mrs(3, cmd);

	/* Disable read leveling: Set D0F3Rx71[7]=0 */
	pci_mod_config8(MCU, 0x71, 1<<7, 0);
}

static void vx900_rx_capture_range_calib(void)
{
	u8 reg8;
	const u32 cal_addr = 0x20;

	/* Set IO calibration address */
	pci_mod_config16(MCU, 0x8c , 0xfff0, cal_addr&(0xfff0));
	/* Data pattern must be 0x00 for this calibration
	 * See paragraph describing Rx8e */
	pci_write_config8(MCU, 0x8e, 0x00);

	/* Need to put DRAM and MCU in read leveling */
	vx900_dram_enter_read_leveling();

	/* Data pattern must be 0x00 for this calibration
	 * See paragraph describing Rx8e */
	pci_write_config8(MCU, 0x8e, 0x00);
	/* Trigger calibration */
	reg8 = 0xa0;
	pci_write_config8(MCU, 0x71, reg8);

	/* Wait for it */
	while(pci_read_config8(MCU, 0x71) & 0x10);
	vx900_dram_exit_read_leveling();
}

static void vx900_rx_dqs_delay_calib(void)
{
	const u32 cal_addr = 0x30;

	/* We need to disable refresh commands so that they don't interfere */
	const u8 ref_cnt = pci_read_config8(MCU, 0xc7);
	pci_write_config8(MCU, 0xc7, 0);
	/* Set IO calibration address */
	pci_mod_config16(MCU, 0x8c , 0xfff0, cal_addr&(0xfff0));
	/* Data pattern must be 0x00 for this calibration
	 * See paragraph describing Rx8e */
	pci_write_config8(MCU, 0x8e, 0x00);

	/* Need to put DRAM and MCU in read leveling */
	vx900_dram_enter_read_leveling();

	/* From VIA code; Undocumented
	 * In theory this enables MODT[3:0] to be asserted */
	pci_mod_config8(MCU, 0x9e, 0, 0x80);

	/* Trigger calibration: Set D0F3Rx71[1:0]=10b */
	pci_mod_config8(MCU, 0x71, 0x03, 0x02);

	/* Wait for calibration to complete */
	while( pci_read_config8(MCU, 0x71) & 0x02 );
	vx900_dram_exit_read_leveling();

	/* Restore the refresh counter*/
	if(1)pci_write_config8(MCU, 0xc7, ref_cnt);

	/* FIXME: should we save it before, or should we just set it as is */
	vx900_dram_set_ma_map(VX900_CALIB_MA_MAP);
}

static void vx900_tx_dqs_trigger_calib(u8 pattern)
{
	u32 i;
	/* Data pattern for calibration */
	pci_write_config8(MCU, 0x8e, pattern);
	/* Trigger calibration */
	pci_mod_config8(MCU, 0x75, 0, 0x20);
	/* Wait for calibration */
	i = 0;
	while(pci_read_config8(MCU, 0x75) & 0x20) i++;
	printram(" Tx DQS calib took %u PCI cycles\n", i);
}
static void vx900_tx_dqs_delay_calib(void)
{
	const u32 cal_addr = 0x00;
	/* Set IO calibration address */
	pci_mod_config16(MCU, 0x8c , 0xfff0, cal_addr&(0xfff0));
	/* Set circuit to use calibration results - Clear Rx75[0]*/
	pci_mod_config8(MCU, 0x75, 0x01, 0);
	/* Run calibration with first data pattern*/
	vx900_tx_dqs_trigger_calib(0x5a);
	/* Run again with different pattern */
	vx900_tx_dqs_trigger_calib(0xa5);
}

static void vx900_tx_dq_delay_calib(void)
{
	int i = 0;
	/* Data pattern for calibration */
	pci_write_config8(MCU, 0x8e, 0x5a);
	/* Trigger calibration */
	pci_mod_config8(MCU, 0x75, 0, 0x02);
	/* Wait for calibration */
	while(pci_read_config8(MCU, 0x75) & 0x02) i++;
	printram("TX DQ calibration took %u PCI cycles\n", i);
}

static void vx900_rxdqs_adjust(delay_range *dly)
{
	/* Adjust Rx DQS delay after calibration has been run. This is
	 * recommended by VIA, but no explanation was provided as to why */
	size_t i;
	for(i = 0; i < 8; i++)
	{
		if(dly->low[i] < 3)
		{
			if(i == 2 || i== 4) dly->low[i] += 4;
			else dly->avg[i] += 3;

		}

		if(dly->high[i] > 0x38) dly->avg[i] -= 6;
		else if(dly->high[i] > 0x30) dly->avg[i] -= 4;

		if(dly->avg[i] > 0x20) dly->avg[i] = 0x20;
	}

	/* Put Rx DQS delay into manual mode (Set Rx[2,0] to 01) */
	pci_mod_config8(MCU, 0x71, 0x05, 0x01);
	/* Now write the new settings */
	vx900_delay_calib_mode_select(VX900_CALIB_RxDQS, VX900_CALIB_MANUAL);
	vx900_write_0x78_0x7f(dly->avg);
}

static void vx900_dram_calibrate_delays(const ramctr_timing *ctrl,
				   const rank_layout *ranks)
{
	timing_dly dly;
	size_t i;
	u8 val;
	vx900_delay_calib delay_cal;
	memset(&delay_cal, 0, sizeof(delay_cal));
	printram("Starting delay calibration\n");

	/**** Read delay control ****/
	/* MD Input Data Push Timing Control;
	 *     use values recommended in datasheet
	 * Setting this too low causes the Rx window to move below the range we
	 * need it so we can capture it with Rx_78_7f
	 * This causes Rx calibrations to be too close to 0, and Tx
	 * calibrations will fail.
	 * Setting this too high causes the window to move above the range.
	 */
	if      (ctrl->tCK <= TCK_533MHZ) val = 2;
	else if (ctrl->tCK <= TCK_333MHZ) val = 1;
	else	 val = 0;
	val ++; /* FIXME: vendor BIOS sets this to 3 */
	pci_mod_config8(MCU, 0x74, (0x03 << 1), ((val & 0x03) << 1) );

	/* FIXME: The vendor BIOS increases the MD input delay - WHY ? */
	pci_mod_config8(MCU, 0xef, (3<<4), 3<<4);


	/**** Write delay control ****/
	/* FIXME: The vendor BIOS does this, but WHY?
	 * Early DQ/DQS for write cycles */
	pci_mod_config8(MCU, 0x76, (3<<2), 2<<2);
	/* FIXME: The vendor BIOS does this - Output preamble ?*/
	pci_write_config8(MCU, 0x77, 0x10);

	/* FIXME: Vendor BIOS goes in with
	 * 8 page registers
	 * multiple page mode
	 * High Priority Refresh request
	 * -- WHY?*/
	pci_write_config8(MCU, 0x69, 0xc7);

	/* Set BA[0/1/2] to [A17/18/19] */
	vx900_dram_set_ma_map(VX900_MRS_MA_MAP);
	/* Disable Multiple Page Mode - Set Rx69[0] to 0 */
	pci_mod_config8(MCU, 0x69, (1<<0), 0x00);

	/* It's very important that we keep all ranks which are not calibrated
	 * mapped to VR3. Even if we disable them, if they are mapped to VR0
	 * (the rank we use for calibrations), the calibrations may fail in
	 * unexpected ways. */
	vx900_pr_map_all_vr3();

	for(i = 0; i < VX900_MAX_DIMM_SLOTS; i++)
	{
		/* Do we have a valid DIMM? */
		if(ranks->phys_rank_size[i] + ranks->phys_rank_size[i+1] == 0 )
			continue;

		/* Map the first rank of the DIMM to VR0 */
		vx900_map_pr_vr(2*i, 0);

		/* Run calibrations */if(1){
		vx900_rx_capture_range_calib();
		vx900_read_delay_range(&(delay_cal.rx_dq_cr),
				       VX900_CALIB_RxDQ_CR);
		dump_delay_range(delay_cal.rx_dq_cr);}

		/*FIXME: Cheating with Rx CR setting
		 * We need to either use Rx CR calibration
		 * or set up a table for the calibration */
		dly[0] = 0x28; dly[1] = 0x1c; dly[2] = 0x28; dly[3] = 0x28;
		dly[4] = 0x2c; dly[5] = 0x30; dly[6] = 0x30; dly[7] = 0x34;
		printram("Bypassing RxCR 78-7f calibration with:\n");
		dump_delay(dly);
		/* We need to put the setting on manual mode */
		pci_mod_config8(MCU, 0x71, 0, 0x10);
		vx900_delay_calib_mode_select(VX900_CALIB_RxDQ_CR, VX900_CALIB_MANUAL);
		vx900_write_0x78_0x7f(dly);

		/************* RxDQS *************/
		vx900_rx_dqs_delay_calib();
		vx900_read_delay_range(&(delay_cal.rx_dqs), VX900_CALIB_RxDQS);
		printram("RX DQS calibration results\n");
		dump_delay_range(delay_cal.rx_dqs);

		vx900_rxdqs_adjust(&(delay_cal.rx_dqs));

		vx900_read_delay_range(&(delay_cal.rx_dqs), VX900_CALIB_RxDQS);
		printram("RX DQS calibration results after adjustment\n");
		dump_delay_range(delay_cal.rx_dqs);

		/* Enable multiple page mode */
		pci_mod_config8(MCU, 0x69, 0, 1<<1);

		/* FIXME: this is done by vendor BIOS, and recommended by VIA
		 * However, datasheet says that bit[7] is reserved, and
		 * calibration works just as well if we don't set this to 1b .
		 * Should we really do this, or can we drop it ? */
		if(ctrl->tCK <= TCK_533MHZ){
		for( i = 0; i< 8; i++) dly[i] = 0x80;
		pci_mod_config8(MCU, 0x75, 0x00, 0x01); /* manual Tx DQ DQS */
		vx900_delay_calib_mode_select(VX900_CALIB_TxDQ, VX900_CALIB_MANUAL);
		vx900_write_0x78_0x7f(dly);
		vx900_delay_calib_mode_select(VX900_CALIB_TxDQS, VX900_CALIB_MANUAL);
		vx900_write_0x78_0x7f(dly);
		}

		/************* TxDQS *************/
		vx900_tx_dqs_delay_calib();

		vx900_read_delay_range(&(delay_cal.tx_dqs), VX900_CALIB_TxDQS);
		printram("Tx DQS calibration results\n");
		dump_delay_range(delay_cal.tx_dqs);
		/************* TxDQ  *************/
		/* FIXME: not sure if multiple page mode should be enabled here
		 * Vendor BIOS does it */
		pci_mod_config8(MCU, 0x69, 0 , 0x01);

		vx900_tx_dq_delay_calib();
		vx900_read_delay_range(&(delay_cal.tx_dq), VX900_CALIB_TxDQ);
		printram("TX DQ delay calibration results:\n");
		dump_delay_range(delay_cal.tx_dq);

		/* write manual settings */
		pci_mod_config8(MCU, 0x75, 0, 0x01);
		vx900_delay_calib_mode_select(VX900_CALIB_TxDQS, VX900_CALIB_MANUAL);
		vx900_write_0x78_0x7f(delay_cal.tx_dqs.avg);
		vx900_delay_calib_mode_select(VX900_CALIB_TxDQ, VX900_CALIB_MANUAL);
		vx900_write_0x78_0x7f(delay_cal.tx_dq.avg);
	}
}

static void vx900_dram_set_refresh_counter(ramctr_timing *ctrl)
{
	u8 reg8;
	/* Set DRAM refresh counter
	 * Based on a refresh counter of 0x61 at 400MHz */
	reg8 = (TCK_400MHZ * 0x61) / ctrl->tCK;
	pci_write_config8(MCU, 0xc7, reg8);
}

static void vx900_dram_range(ramctr_timing *ctrl, rank_layout *ranks)
{
	size_t i, vrank = 0;
	u8 reg8;
	u32 ramsize = 0;
	vx900_clear_vr_map();
	/* All unused physical ranks go to VR3. Otherwise, the MCU might be
	 * trying to read or write from unused ranks, or even worse, write some
	 * bits to the rank we want, and some to the unused ranks, even though
	 * they are disabled. Since VR3 is the last virtual rank to be used, we
	 * eliminate any ambiguities that the MCU may face. */
	vx900_pr_map_all_vr3();
	for(i = 0; i < VX900_MAX_MEM_RANKS; i++)
	{
		u32 rank_size = ranks->phys_rank_size[i];
		if(!rank_size) continue;
		ranks->virt[vrank].start_addr = ramsize;
		ramsize += rank_size;
		ranks->virt[vrank].end_addr = ramsize;

		/* Rank memory range */
		reg8 = (ranks->virt[vrank].start_addr >> 2);
		pci_write_config8(MCU, 0x48 + vrank, reg8);
		reg8 = (ranks->virt[vrank].end_addr >> 2);
		pci_write_config8(MCU, 0x40 + vrank, reg8);

		vx900_map_pr_vr(i, vrank);

		printram("Mapped Physical rank %u, to virtual rank %u\n"
		"    Start address: 0x%.8x000000\n"
		"    End   address: 0x%.8x000000\n",
	   (int) i, (int) vrank,
			 ranks->virt[vrank].start_addr,
	   ranks->virt[vrank].end_addr);

		/* Move on to next virtual rank */
		vrank++;
	}

	printram("Initialized %u virtual ranks, with a total size of %u MB\n",
		 (int) vrank, ramsize << 4);
}

static void vx900_dram_write_final_config(ramctr_timing *ctrl)
{

	/* FIXME: These are quick cheats */
	pci_write_config8(MCU, 0x50, 0xa0); /* DRAM MA map */
	vx900_dram_set_ma_map(VX900_CALIB_MA_MAP); /* Rank interleave */

	pci_write_config8(MCU, 0x69, 0xe7);
	//pci_write_config8(MCU, 0x72, 0x0f);

	//pci_write_config8(MCU, 0x97, 0xa4); /* self-refresh */
	//pci_write_config8(MCU, 0x98, 0xba); /* self-refresh II */
	//pci_write_config8(MCU, 0x9a, 0x80); /* self-refresh III */

	/* Enable automatic triggering of short ZQ calibration */
	pci_write_config8(MCU, 0xc8, 0x80);

	/* Disable shitty 8086 legacy shadows */
	pci_write_config8(MCU, 0x80, 0xff); /* ROM 768k - 832k */
	pci_write_config8(MCU, 0x81, 0xff); /* ROM 832k - 896k */
	pci_write_config8(MCU, 0x82, 0xff); /* ROM 896k - 960k */
	/* ROM 960k - 1M * SMRAM: 640k - 768k */
	pci_write_config8(MCU, 0x83, 0x31);

}

static void print_debug_pci_dev(device_t dev)
{
	print_debug("PCI: ");
	print_debug_hex8((dev >> 20) & 0xff);
	print_debug_char(':');
	print_debug_hex8((dev >> 15) & 0x1f);
	print_debug_char('.');
	print_debug_hex8((dev >> 12) & 7);
}

static void dump_pci_device(device_t dev)
{
	int i;
	print_debug_pci_dev(dev);
	print_debug("\n");

	for (i = 0; i <= 0xff; i++) {
		unsigned char val;
		if ((i & 0x0f) == 0) {
			print_debug_hex8(i);
			print_debug_char(':');
		}

		if ((i & 0x0f) == 0x08) {
			print_debug(" |");
		}

		val = pci_read_config8(dev, i);
		print_debug_char(' ');
		print_debug_hex8(val);

		if ((i & 0x0f) == 0x0f) {
			print_debug("\n");
		}
	}
}

static void vx900_gfx_write_ioctl(u8 where, u8 what)
{
	outb(where, 0x3c4);
	outb(what, 0x3c5);
}
/*static void vx900_gfx_mod_ioctl(u8 where, u8 clr_bits, u8 set_bits)
{
	outb(where, 0x3c4);
	u8 what = inb(0x3c5);
	what &= ~clr_bits;
	what |= set_bits;
	outb(what, 0x3c5);
}
*/
static void vx900_dram_set_gfx_resources(void)
{
	/* FIXME: enable VGA or not? */
	/* u32 fbuff_size = vga_decide_framebuffer_size();
	 * if(fbuff_size == 0) {
	 *	Do not initialize the IGP
	 * 	return;
	 * } */
	/* Step 1 - Enable VGA controller */
	pci_mod_config8(MCU, 0xa1, 0, 0x80);
	/* FIXME: can we disable this? */
	pci_mod_config8(MCU, 0xa4, 0, 0x80); /* VGA memory hole */

	device_t d0f0 = PCI_DEV(0,0,0);
	/* Step 2 - Forward MDA to GFX */
	pci_mod_config8(d0f0, 0x4e, 0, 1<<1);

	device_t gfx = PCI_DEV(0,1,0);
	/* Step 3 - Turn on GFX I/O space */
	pci_mod_config8(gfx, 0x04, 0, 1<<0);

	/* Step 4 - Enable video subsystem */
	u8 io8 = inb(0x3c3);
	io8 |= 1<<0;
	outb(io8, 0x3c3);
	/* Step 5 - Unlock accessing of IO space */
	vx900_gfx_write_ioctl(0x10, 0x01);

	/* FIXME: decide what the framebuffer size is, don't assume it's 512M */
	/* Step 6 - Let MCU know the framebuffer size */
	pci_mod_config8(MCU, 0xa1, 7<<4, 7<<4);
	/* Step 7 - Let gfx know the framebuffer size (through PCI and IOCTL) */
	pci_write_config8(gfx, 0xb2, 0x00);
	vx900_gfx_write_ioctl(0x68, 0x00);

	/* FIXME: framebuffer base should be decided based on TOM and size */
	/* Let the MCU know of the frambuffer base */
	pci_mod_config16(MCU, 0xa0, 0xffe, 0x200);
	/* Step 8 - Enable memory base register on the GFX */
	vx900_gfx_write_ioctl(0x6d, 0x00); /* base 28:21 */
	vx900_gfx_write_ioctl(0x6e, 0x01); /* base 36:29 */
	vx900_gfx_write_ioctl(0x6f, 0x00); /* base what what in the butt ?? */

	/* Step 9 - Set SID/VID */
	// Set SVID high byte
	vx900_gfx_write_ioctl(0x36, 0x11);
	// Set SVID Low byte
	vx900_gfx_write_ioctl(0x35, 0x06);
	// Set SID high byte
	vx900_gfx_write_ioctl(0x38, 0x71);
	// Set SID Low byte
	vx900_gfx_write_ioctl(0x37, 0x22);
}

void vx900_init_dram_ddr3(const dimm_layout *dimm_addr)
{
	dimm_info dimm_prop;
	ramctr_timing ctrl_prop;
	rank_layout ranks;
	device_t mcu;

	/* Locate the Memory controller */
	mcu = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				       PCI_DEVICE_ID_VIA_VX900_MEMCTRL), 0);

	if (mcu == PCI_DEV_INVALID) {
		die("Memory Controller not found\n");
	}
	dump_pci_device(mcu);
	memset(&ranks, 0, sizeof(ranks));
	/* 1) Write some initial "safe" parameters */
	vx900_dram_write_init_config();
	/* 2) Get timing information from SPDs */
	dram_find_spds_ddr3(dimm_addr, &dimm_prop);
	/* 3) Find lowest common denominator for all modules */
	dram_find_common_params(&dimm_prop, &ctrl_prop);
	/* 4) Find the size of each memory rank */
	vx900_dram_phys_bank_range(&dimm_prop, &ranks);
	/* 5) Set DRAM driving strength */
	vx900_dram_driving_ctrl(&dimm_prop);
	/* 6) Set DRAM frequency and latencies */
	vx900_dram_timing(&ctrl_prop);
	vx900_dram_freq(&ctrl_prop);
	/* 7) Initialize the modules themselves */
	vx900_dram_ddr3_dimm_init(&ctrl_prop, &ranks);
	/* 8) Set refresh counter based on DRAM frequency */
	vx900_dram_set_refresh_counter(&ctrl_prop);
	/* 9) Calibrate receive and transmit delays */
	vx900_dram_calibrate_delays(&ctrl_prop, &ranks);
	/* 10) Enable Physical to Virtual Rank mapping */
	vx900_dram_range(&ctrl_prop, &ranks);
	/* 99) Some final adjustments */
	vx900_dram_write_final_config(&ctrl_prop);
	/* A0) What about our beloved GFX */
	vx900_dram_set_gfx_resources();
	/* Take a dump */
	dump_pci_device(mcu);

}
