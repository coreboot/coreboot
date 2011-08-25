/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011  Alexandru Gagniuc <mr.nuke.me@gmail.com>
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

#include <arch/io.h>
#include <arch/romcc_io.h>

#include "early_vx900.h"
#include "raminit.h"

#include <string.h>
#include <console/console.h>
#include <device/pci_ids.h>

#define min(a,b) a<b?a:b
#define max(a,b) a>b?a:b

#define MCU PCI_DEV(0, 0, 3)

typedef struct pci_reg8_st {
	u8 addr;
	u8 val;
} pci_reg8;

/* These are some "safe" values that can be used for memory initialization.
 * Some will stay untouched, and others will be overwritten later on */
static pci_reg8 mcu_init_config[] = {
	{0x40, 0x01}, /* Virtual rank 0 ending address = 64M - 1 */
	{0x48, 0x00}, /* Virtual rank 0 starting address = 0 */
	{0x50, 0xd8}, /* Set ranks 0-3 to 11 col bits, 16 row bits */
	{0x52, 0x33}, /* Map BA0 to A15, BA1 to A18 */
	{0x53, 0x0b}, /* Map BA2 to A19 */
	/* Disable rank interkeaving in ranks 0-3 */
	{0x58, 0x00}, {0x59, 0x00}, {0x5a, 0x00}, {0x5b, 0x00},
	{0x6c, 0xA0}, /* Memory type: DDR3, VDIMM: 1.5V, 64-bit DRAM */
	{0x6e, 0x38}, /* Burst lenght: 8, burst-chop: enable */
	{0xc6, 0x80}, /* Minimum latency from self-refresh. Bit [7] must be 1 */
	{0xc8, 0x80}, /* Enable automatic triggering of short ZQ calibration */
	/* Enable differential DQS; MODT assertion values suggested in DS */
	{0x9e, 0xa1}, {0x9f, 0x50}
};

/* FIXME: DO NOT ACCEPT THIS PATCH IF YOU SEE THIS ARRAY
 * This is just a cheat sheet to get the config up and running, and is specific
 * to my hardware setup. */
static pci_reg8 mcu_cheat_sheet[] = {
	/* Number of row, col and bank bits */
	{0x50, 0xa0},
	/* Rank interleave address select */
	{0x52, 0x11}, {0x53, 0x59},
	/* DRAM Pipeline control */
	{0x60, 0xf4}, {0x61, 0x2e},
	/* DRAM arbitration */
	              {0x65, 0x49}, {0x66, 0x80},
	/* Bank interleave control; request reorder control */
	              {0x69, 0xe7}, {0x6a, 0xfc},

	/* very finetuned timing; do not touch; leave commented out */
	//                            {0x72, 0x0f}, {0x73, 0x04},
	//                            {0x76, 0x68}, {0x77, 0x10},

	/* Top mem; need to calculate this dynamically */
	{0x84, 0x01}, {0x85, 0x40},

	/* DRAM clocking control; mostly finetuned delays ; leave out */
	              /*{0x91, 0x08}, {0x92, 0x08}, {0x93, 0x16},
	{0x94, 0x00}, {0x95, 0x16}, {0x96, 0x00}, {0x97, 0xa4},
	{0x98, 0xba}, {0x99, 0xff}, {0x9a, 0x80},
	{0x9c, 0xe4},
	*/

};
/* FIXME: Cheat sheet number two
 * We need to set driving strength before trying to operate the controller
 * It's like putting it in first gear before pushing the accelerator */
static pci_reg8 memdrv_cheat_sheet[] = {
	/* Enable memory clock output */
	//{0x9b, 0x3f}, // on by default

	{0xd3, 0x01}, /* Enable controller ODT auto-compensation */

	/* Memory driving controls */
	/* ?? Internal ODT control ?? */
	//{0xd4, 0x80},
	/* ?? ODT Range selection ?? */
	              {0xd5, 0x04},
	/* ?? MCLK/MA/MCS driving selection ?? */
	                            //{0xd6, 0x20},
	/* Auto compensation mode for _everything_ */
	{0xe1, 0x7e},

};

static void vx900_dram_write_init_config(void)
{
	size_t i;
	for(i = 0; i < (sizeof(mcu_init_config)/sizeof(pci_reg8)); i++)
		pci_write_config8(MCU, mcu_init_config[i].addr,
				  mcu_init_config[i].val);
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
	ctrl->cas = 0xff;
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
		ctrl->cas &= dimm->cas;

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

	if(!ctrl->cas) die("Unsupported DIMM combination. "
		"DIMMS do not support common CAS latency");
	if(!valid_dimms) die("No valid DIMMs found");
}

static void vx900_dram_phys_bank_range(const dimm_info *dimms,
				       rank_layout *ranks)
{
	size_t i;
	u8 reg8;
	for(i = 0; i < VX900_MAX_DIMM_SLOTS; i ++)
	{
		if(dimms->dimm[i].dram_type == DRAM_TYPE_UNDEFINED)
			continue;
		u8 nranks = dimms->dimm[i].ranks;
		if(nranks > 2)
			die("Found DIMM with more than two ranks, which is not"
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
	/* There's a tiny thing we need to do:
	 * set the column and row address bits
	 * FIXME: Really bad programming here */
	reg8 = 0;// ((( (1<<2) | (dimms->dimm[0].col_bits - 9) ) & 0x7) << 2);
	reg8 |= ((( (1<<2) | (dimms->dimm[1].col_bits - 9) ) & 0x7) << 5);
	pci_write_config8(PCI_DEV(0,0,3), 0x50, reg8);
	
}

static void vx900_dram_driving_ctrl(void)
{
	/* My memory drive strength cheat sheet */
	size_t i;
	for(i = 0; i < (sizeof(memdrv_cheat_sheet)/sizeof(pci_reg8)); i++)
		pci_write_config8(MCU, memdrv_cheat_sheet[i].addr,
				  memdrv_cheat_sheet[i].val);
}

static void vx900_dram_range(ramctr_timing *ctrl, rank_layout *ranks)
{
	size_t i, vrank = 0;
	u8 reg8;
	u32 reg32 = 0, ramsize = 0;
	for(i = 0; i < VX900_MAX_MEM_RANKS; i++)
	{
		u32 rank_size = ranks->phys_rank_size[i];
		if(!rank_size) continue;
		ranks->virt[vrank].start_addr = ramsize;
		ramsize += rank_size;
		ranks->virt[vrank].end_addr = ramsize;

		/* Physical to virtual rank mapping */
		/* The position of the bit that enables physical rank [i] */
		const char rank_en_bit[] = {3, 7, 11, 15};
		/* The position of the LSB of the VR mapping of rank [i] */
		const char rank_map_pos[] = {0, 4, 8, 12};
		/* Enable the physical rank */
		reg32 |= ( 1 << rank_en_bit[i]);
		/* Map the physical rank to the first unused virtual rank */
		reg32 |= (vrank << rank_map_pos[i]);
		/* Rank memory range */
		reg8 = (ranks->virt[vrank].start_addr >> 2);
		pci_write_config8(MCU, 0x48 + vrank, reg8);
		reg8 = (ranks->virt[vrank].end_addr >> 2);
		pci_write_config8(MCU, 0x40 + vrank, reg8);

		printram("Mapped Physical rank %u, to virtual rank %u\n"
				 "    Start address: 0x%.10x000000\n"
				 "    End   address: 0x%.10x000000\n",
				(int) i, (int) vrank,
				ranks->virt[vrank].start_addr,
				ranks->virt[vrank].end_addr);

		/* Move on to next virtual rank */
		vrank++;
	}

	/* Finally, write the mapping configuration to the MCU */
	pci_write_config32(MCU, 0x54, reg32);

	printram("Initialized %u virtual ranks, with a total size of %u MB\n",
			 (int) vrank, ramsize << 4);
}

static void vx900_dram_freq_latency(ramctr_timing *ctrl)
{
	u8 reg8, val;
	u32 val32;

	/* Maximum supported DDR3 frequency is 533MHz (DDR3 1066)
	 * so make sure we cap it if we have faster DIMMs */
	if(ctrl->tCK < TCK_533MHZ) ctrl->tCK = TCK_533MHZ;
	val32 = (1000 << 8) / ctrl->tCK;
	printram("Selected DRAM frequency: %u MHz\n", val32);

	/* To change the DRAM frequency, it is required to set this bits to
	 * 1111b to reset the PLL first and set to the target value then.
	 * Minimum assertion time for the PLL Reset is 10 ns.
	 * Datasheet also states that the DRAM frequency must be set first */
	/* Reset the PLL */
	pci_write_config8(MCU, 0x90, 0x0f);
	/* Wait at least 10 ns; in this case, we will wait about 1 us */
	inb(0x80);
	/* Now find the right DRAM frequency setting,
	 * and bring it to the closest JEDEC standard frequency */
	if(ctrl->tCK <= TCK_533MHZ)      {val = 0x07; ctrl->tCK = TCK_533MHZ;}
	else if(ctrl->tCK <= TCK_400MHZ) {val = 0x06; ctrl->tCK = TCK_400MHZ;}
	else if(ctrl->tCK <= TCK_333MHZ) {val = 0x05; ctrl->tCK = TCK_333MHZ;}
	else if(ctrl->tCK <= TCK_266MHZ) {val = 0x04; ctrl->tCK = TCK_266MHZ;}

	reg8 = val & 0x0f;
	/* Restart the PLL with the correct frequency */
	pci_write_config8(MCU, 0x90, reg8);

	/* If we have registered DIMMs, we need to set bit[0] */
	if(dimm_is_registered(ctrl->dram_type)){
		printram("Enabling RDIMM support in memory controller\n");
		reg8 = pci_read_config8(MCU, 0x6c);
		reg8 |= 1;
		pci_write_config8(MCU, 0x6c, reg8);
	}

	/* Here we are calculating latencies, and writing them to the appropiate
	 * registers. Some registers do not take latencies from 0T, for example:
	 * CAS: 000 = 4T, 001 = 5T, 010 = 6T, etc
	 * In this example we subtract 4T from the result for CAS: (val - 4)
	 * The & 0x07 after (val - T0) just makes sure that, no matter what
	 * crazy thing may happen, we do not write outside the bits allocated
	 * in the register */
	
	/* Find CAS latency */
	val = (ctrl->tAA + ctrl->tCK -1) / ctrl->tCK;
	printram("Minimum  CAS latency   : %uT\n", val);
	/* Find lowest supported CAS latency that satisfies the minimum value */
	while( !((ctrl->cas >> (val-4))&1) && (ctrl->cas >> (val-4))) val++;
	/* Is CAS supported */
	if(!(ctrl->cas & (1 << (val-4))) ) printram("CAS not supported\n");
	printram("Selected CAS latency   : %uT\n", val);
	/* Write CAS latency */
	val -= 4; /* 000 means 4T */
	reg8 = ((val &0x07) << 4 ) | (val & 0x07);
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
	val = (ctrl->tWR + ctrl->tCK -1) / ctrl->tCK;
	printram("Selected tWR           : %uT\n", val);
	reg8 |= ((val-4) & 0x7);
	pci_write_config8(MCU, 0xc2, reg8);

	/* Find tFAW */
	val = (ctrl->tFAW + ctrl->tCK -1) / ctrl->tCK;
	printram("Selected tFAW          : %uT\n", val);
	reg8 = ((val-0) & 0x7) << 4;
	/* Find tRRD */
	val = (ctrl->tRRD + ctrl->tCK -1) / ctrl->tCK;
	printram("Selected tRRD          : %uT\n", val);
	reg8 |= ((val-2) & 0x7);
	pci_write_config8(MCU, 0xc3, reg8);

	/* This register needs a little more attention, as bit 7 controls some
	 * other stuff:
	 * [7] 8-Bank Device Timing Constraint (See datasheet)
	 *     Since all DDR3 modules are minimum 8-banks, there's no need to
	 *     worry about this bit, and we can just set it;
	 * [6] Reserved
	 * [5:4] tRTP with 00 = 3T for DDR3
	 * [3] Reserved
	 * [2-0] tWTR with 000 = 2T
	 */
	reg8 = 0x80;
	/* Find tRTP */
	val = (ctrl->tRTP + ctrl->tCK -1) / ctrl->tCK;
	printram("Selected tRTP          : %uT\n", val);
	reg8 |= ((val & 0x3) << 4);
	/* Find tWTR */
	val = (ctrl->tWTR + ctrl->tCK -1) / ctrl->tCK;
	printram("Selected tWTR          : %uT\n", val);
	reg8 |= ((val - 2) & 0x7);
	pci_write_config8(MCU, 0xc4, reg8);

	/* DRAM Timing for All Ranks - VI
	 * [7:6] CKE Assertion Minimum Pulse Width
	 *     We probably don't want to mess with this just yet.
	 * [5:0] Refresh-to-Active or Refresh-to-Refresh (tRFC)
	 *   RxC4[7] = 0:
	 *     tRFC = (10 + [5:0])T
	 *   RxC4[7] = 1:
	 *     tRFC = (30 + 2 * [5:0])T
	 *     Since we previously set RxC4[7], we're going to use this formula
	 */	
	reg8 = pci_read_config8(MCU, 0xc5);
	val = (ctrl->tRFC + ctrl->tCK -1) / ctrl->tCK;
	printram("Minimum  tRFC          : %uT\n", val);
	if(ctrl->tRFC < 30) {
		val = 0;
	} else {
		val = (val -30 + 1 ) / 2;
	}
	;
	printram("Selected tRFC          : %uT\n", 30 + 2 * val);
	reg8 |= (val & 0x1f);
	pci_write_config8(MCU, 0xc5, reg8);

	/* Where does this go??? */
	val = (ctrl->tRC + ctrl->tCK -1) / ctrl->tCK;
	printram("Required tRC           : %uT\n", val);
}

/* The VX900 can send the MRS commands directly through hardware */
static void vx900_dram_ddr3_do_hw_mrs(u8 ma_swap, u8 rtt_nom,
				      u8 ods, u8 rtt_wr, u8 srt, u8 asr)
{
	u8 reg8 = 0;
	if(asr) reg8 |= (1 << 0);
	if(srt) reg8 |= (1 << 1);
	reg8 |= ((rtt_wr & 0x03) << 4);
	pci_write_config8(MCU, 0xcd, reg8);
	reg8 = 1;
	if(ma_swap) reg8 |= (1 << 1);
	reg8 |= ((ods & 0x03) << 2);
	reg8 |= ((rtt_nom & 0x7) << 4);
	pci_write_config8(MCU, 0xcc, reg8);
	while(pci_read_config8(MCU, 0xcc) & 1);
}
#include "forgotten.h"
#include "forgotten.c"
static void vx900_dump_0x78_0x7f(void)
{
	u8 i;
	for(i = 0x78; i < 0x80; i ++)
	{
		printram(" %.2x", pci_read_config8(MCU, i));
	}
	printram("\n");
}

static void vx900_dump_calib(const u8 what)
{
	u8 reg8;
	/* Dump lower bound */
	reg8 = ((what & 0x3) << 2) | 0x1;
	pci_write_config8(MCU, 0x70, reg8);
	printram("Lower bound : ");
	vx900_dump_0x78_0x7f();
	
	/* Dump upper bound */
	reg8 = ((what & 0x3) << 2) | 0x2;
	pci_write_config8(MCU, 0x70, reg8);
	printram("Upper bound : ");
	vx900_dump_0x78_0x7f();
	
	/* Dump average values */
	reg8 = ((what & 0x3) << 2);
	pci_write_config8(MCU, 0x70, reg8);
	printram("Average     : ");
	vx900_dump_0x78_0x7f();
}

static void vx900_dram_send_soft_mrs(u8 type, u16 cmd)
{
	u8 reg8;
	u32 addr;
	/* Set Fun3_RX6B[2:0] to 011b (MSR Enable). */
	reg8 = pci_read_config8(MCU, 0x6b);
	reg8 &= ~(0x03);
	reg8 |= (3<<0);
	pci_write_config8(MCU, 0x6b, reg8);
	/* Find the address corresponding to the MRS */
	addr = vx900_get_mrs_addr(type, cmd);
	/* Execute the MRS */
	volatile_read(addr);
	/* Set Fun3_Rx6B[2:0] to 000b (Normal SDRAM Mode). */
	reg8 = pci_read_config8(MCU, 0x6b);
	reg8 &= ~(7<<0);
	pci_write_config8(MCU, 0x6b, reg8);
}
static void vx900_rx_capture_range_calib(u8 pattern);
static void vx900_rx_dqs_delay_calib(u8 pattern);
static void vx900_tx_dq_delay_calib(void);
static void vx900_tx_dqs_delay_calib(const u8  rank);

static void vx900_delay_calib_rank(const u8 rank)
{
	u8 reg8, val;
	u16 reg16;
	printram("Starting delay calibration for rank %u\n", rank);
	
	reg16 = 0xbbbb;
	reg16 &= ~(0xf << (4 * rank));
	reg16 |= (0x8 << (4 * rank));
	pci_write_config16(MCU, 0x54, reg16);

	/* MD Input Data Push Timing Control;
	 *     use values recommended in datasheet */
	reg8 = pci_read_config8(MCU, 0x74);
	val = (TCK_533MHZ <= TCK_400MHZ)?2:1; /* FIXME*/
	reg8 &= ~(0x03 << 1);
	reg8 |= ((val & 0x03) << 1);
	pci_write_config8(MCU, 0x74, reg8);
	
	/* Disable additional delays */
	pci_write_config8(MCU, 0xec, 0x00);
	pci_write_config8(MCU, 0xed, 0x00);
	pci_write_config8(MCU, 0xee, 0x00);

	pci_write_config8(MCU, 0x77, 0x10);/*FIXME*/
	const u8 pattern = 0x40; /*FIXME*/
	//const u8 pattern = 0x00; /*FIXME*/
	/* Run calibrations */
	vx900_rx_capture_range_calib(pattern);
	vx900_rx_dqs_delay_calib(pattern);
	vx900_tx_dq_delay_calib();
	vx900_tx_dqs_delay_calib(rank);
}

static void vx900_rx_capture_range_calib(u8 pattern)
{
	u8 reg8;
	u16 cmd;
	/*
	 * Calibration of RX Capture Range can be triggered by setting
	 * Rx71[5] to 1.
	 * The process started by sending a series of read commands on the DRAM
	 * bus with different setting for the RX Capture Range.
	 * 
	 * After a MRS (Mode Register Set) command to put DDR3 in a mode called
	 * “Read Leveling Mode”,
	 */
	/* Put DRAM in read leveling mode */
	cmd = ddr3_get_mr3(1);
	vx900_dram_send_soft_mrs(3, cmd);

	/* DDR3 returned the read command with a predefined
	 * data pattern either “00h-01h-00h-01h” or “00h-FFh-00h-FFh”. There is
	 * no specific definition in the current DDR3 specification for this
	 * pattern, so, the working software might need to do the calibration
	 * twice. It could triggered the scanning with
	 * Rx71[6] set to 1 first.
	 * If nothing can be working right, the software trigger the scanning
	 * again with
	 * Rx71[6] set to 0.
	 * The controller thus can check those pattern to verify if the data
	 * from DDR3 DIMM is correct or not. The calibration will work from the
	 * lowest limit of the setting for the RX Capture Range, it marked the
	 * setting for its lower bound when checking is correct. It increased
	 * the setting and issued read cycle again and again until checking of
	 * the data is incorrect. It then marked the last working setting as
	 * the upper bound.
	 * Controller also calculated the average of the lower bound and upper
	 * bound to have the center of the setting, which presumably the most
	 * stable one. Besides this capture range setting, there is another set
	 * of delay control for internal MD paths at RxEF[5:4].
	 * However, that one is independent of the calibration mechanism here.*/
	/* Data pattern must be 0x00 for this calibration */
	pci_write_config8(MCU, 0x8e, 0x00);
	/* Trigger calibration */
	reg8 = 0xa0 | (pattern & 0x40);
	pci_write_config8(MCU, 0x71, reg8);

	printram("RX capture range calibration results:\n");
	vx900_dump_calib(3);

	/* Disable read leveling, and put dram in normal operation mode */
	cmd = ddr3_get_mr3(0);
	vx900_dram_send_soft_mrs(3, cmd);
}

static void vx900_rx_dqs_delay_calib(u8 pattern)
{
	u8 reg8;
	/* The same process is done by setting
	 * Rx71[1] to 1
	 * to start the calibration for RX DQS Delay. Besides the manual
	 * setting, there is also a setting which is calculated by using
	 * internal DCLK DLL to have a 1/4T delay for DQS.
	 * Noted that when this option is set
	 * (Rx71[2] =1)
	 * the DQS of all the bytes (MDQS[7:0]P/N) will use this self-calculated
	 * 1/4 delay setting. */

	/* Data pattern must be 0x00 for this calibration */
	pci_write_config8(MCU, 0x8e, 0x00);
	/* Trigger calibration */
	reg8 = 0x82 | (pattern & 0x40);
	pci_write_config8(MCU, 0x71, reg8);

	printram("RX DQS delay calibration results:\n");
	vx900_dump_calib(2);
}
static void vx900_tx_dq_delay_calib()
{
	/* The calibration for TX DQ Delay is started once
	 * Rx75[1] is set to 1.
	 * That process is to issue a write with
	 * data defined by Rx8E[7:0]
	 * and to issue a read command right after it to do the checking.
	 * Before issuing the write command, the calibration controller will set
	 * the setting of TX DQ Delay. It started with the smallest setting, and
	 * it will mark the one as lower bound when the corresponding checking
	 * is right. It continue with the write + read + checking processes and
	 * see if it started to fail. The last working setting is the upper
	 * bound. This process will not stop until all the upper bound are found
	 * for all the bytes. */
	/* Data pattern for calibration */
	pci_write_config8(MCU, 0x8e, 0x5a);
	/* Trigger calibration */
	pci_write_config8(MCU, 0x75, 0x02);

	printram("TX DQ delay calibration results:\n");
	vx900_dump_calib(1);
}
static void vx900_tx_dqs_delay_calib(const u8  rank)
{
	u8 reg8;
	u16 cmd;
	/* Noted that there are two types of calibration for the TX DQS Delay.
	 * Setting Rx75[5] to 1,
	 * the calibration process will go as that of TX DQ Delay. Only that the
	 * adjustment made is on the TX DQS output path not the TX DQ output
	 * path. There is another way of calibration worked with DDR3 DRAM
	 * called “Write Leveling”. The procedure would be
	 * 1) Set Rx9E[3:2] to select which Rank to do write leveling.
	 * 2) Issue MRS commands to each Rank of DRAM to let them know which one
	 *     is enabled for Write leveling.
	 * 3) Set Rx75[2] to 1 to trigger the operation.
	 * After step 3, the controller will adjust the TX DQS delay by one step
	 * and send a pulse of DQS out to DRAM. The bit0 of MD of a byte
	 * (MD0, MD8, MD16, .. MD57) will return the status (“high” or “low”) of
	 * DCLK the selected Rank sensed. The controller will continue adjusting
	 * the delay and sending a pulse of MDQS out. This scheme is to detect
	 * the MDQS delay against the DCLK’s rising and falling edge. Thus we
	 * can know where the lower bound of the TX DQS setting is when the
	 * falling edge of DCLK is found, and where the upper bound of the
	 * TX DQS setting is when the rising edge of DCLK is found by the DRAM.
	 * With the upper bound and lower bound, we can choose the middle point
	 * as the setting for the calibration (scan) result. */
	
	reg8 = pci_read_config8(MCU, 0x9e);
	reg8 &= ~(3 << 2);
	reg8 |= ((rank & 0x3) << 2);
	pci_write_config8(MCU, 0x9e, reg8);
	cmd = ddr3_get_mr1(0, 0, 0, 1, 0, 0, 0);
	vx900_dram_send_soft_mrs(1, cmd);
	/* Data pattern for calibration */
	pci_write_config8(MCU, 0x8e, 0x5a);
	/* Trigger calibration */
	pci_write_config8(MCU, 0x75, 0x04);
	printram("TX DQS delay calibration results:\n");
	vx900_dump_calib(0);
	cmd = ddr3_get_mr1(0, 0, 0, 0, 0, 0, 0);
	vx900_dram_send_soft_mrs(1, cmd);

}

static void vx900_dram_ddr3_dimm_init(const ramctr_timing *ctrl,
				      const rank_layout *ranks)
{
	u8 reg8;
	size_t i;

	/* Step 01 - Set Fun3_Rx6E[5] to 1b to support burst length. */
	/* This is set at init time
	reg8 = pci_read_config8(MCU, 0x6e);
	reg8 |= (1<<5);
	pci_write_config8(MCU, 0x6e, reg8);*/
	/* Step 02 - Set Fun3_RX69[0] to 0b (Disable Multiple Page Mode). */
	reg8 = pci_read_config8(MCU, 0x69);
	reg8 &= ~(1<<0);
	pci_write_config8(MCU, 0x69, reg8);
	
	/* Step 03 - Set the target physical rank to virtual rank0 and other
	 * ranks to virtual rank3. If physical rank0 init is desired,
	 * set Fun3_Rx54 to 08Bh and Fun3_Rx55=0BBh. */
	pci_write_config16(MCU, 0x54, 0xbbb8);
	
	/* Step 04 - Set Fun3_Rx50 to D8h. */
	pci_write_config8(MCU, 0x50, 0xd8);
	/* Step 05 - Set Fun3_RX6B[5] to 1b to de-assert RESET# and wait for at
	 * least 500 us. */
	reg8 = pci_read_config8(MCU, 0x6b);
	reg8 |= (1<<5);
	pci_write_config8(MCU, 0x6b, reg8);
	for(i = 0; i < 500; i++) inb(0x80);

	for(i = 0; i < VX900_MAX_MEM_RANKS; i++)
	{
		u16 reg16;
		if(ranks->phys_rank_size[i] == 0) continue;
		printram("Initializing rank %lu\n", i);

		const u8 rtt_nom = 2;
		const u8 ods = 0;
		const u8 rtt_wr = 1;

		reg16 = 0xbbbb;
		reg16 &= ~(0xf << (4 * i));
		reg16 |= (0x8 << (4 * i));
		pci_write_config16(MCU, 0x54, reg16);
		vx900_dram_ddr3_do_hw_mrs(0, rtt_nom, ods, rtt_wr, 0, 0);
		vx900_delay_calib_rank(i);
	}
	/* Step 15 – Set the next target physical rank to virtual rank 0 and
	 * other ranks to virtual rank 3. Repeat Step 6 to 14, then jump to
	 * Step 16. */

	/* Step 16 – Set Fun3_Rx6B[2:0] to 000b (Normal SDRAM Mode). */
	reg8 = pci_read_config8(MCU, 0x6b);
	reg8 &= ~(7<<0);
	pci_write_config8(MCU, 0x6b, reg8);
	
	/* Step 17 – Set Fun3_Rx69[0] to 1b (Enable Multiple Page Mode). */
	reg8 = pci_read_config8(MCU, 0x69);
	reg8 |= (1<<0);
	pci_write_config8(MCU, 0x69, reg8);

	printram("DRAM initialization sequence complete\n");
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
	
	for (i = 0; i <= 255; i++) {
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

static void vx900_dram_write_final_config(ramctr_timing *ctrl)
{
	u8 reg8;
	/* Set DRAM refresh counter
	 * Based on a refresh counter of 0x61 at 400MHz */
	reg8 = (TCK_400MHZ * 0x61) / ctrl->tCK;
	pci_write_config8(MCU, 0xc7, reg8);

	/* My cheat sheet */
	size_t i;
	for(i = 0; i < (sizeof(mcu_cheat_sheet)/sizeof(pci_reg8)); i++)
		pci_write_config8(MCU, mcu_cheat_sheet[i].addr,
				  mcu_cheat_sheet[i].val);
}

void vx900_init_dram_ddr3(const dimm_layout *dimm_addr)
{
	dimm_info dimm_prop;
	ramctr_timing ctrl_prop;
	rank_layout ranks;
	device_t mcu;
	/* Locate the Memory controller */
	mcu = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				       PCI_DEVICE_ID_VIA_VX900_DRAM_CTR), 0);

	if (mcu == PCI_DEV_INVALID) {
		die("Memory Controller not found\n");
	}
	
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
	vx900_dram_driving_ctrl();	
	/* 6) Set DRAM frequency and latencies */
	vx900_dram_freq_latency(&ctrl_prop);
	/* 7) Initialize the modules themselves */
	vx900_dram_ddr3_dimm_init(&ctrl_prop, &ranks);
	/* 8) Enable Physical to Virtual Rank mapping */
	vx900_dram_range(&ctrl_prop, &ranks);
	/* 99) Some final adjustments */
	vx900_dram_write_final_config(&ctrl_prop);
	/* Take a dump */
	dump_pci_device(mcu);

}
