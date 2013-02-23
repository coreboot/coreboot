/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#include "mct_d.h"

static void setSyncOnUnEccEn_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA);
static u8 isDramECCEn_D(struct DCTStatStruc *pDCTstat);

/* Initialize ECC modes of Integrated Dram+Memory Controllers of a network of
 * Hammer processors.  Use Dram background scrubber to fast initialize ECC bits
 * of all dram.
 *
 * Notes:
 *
 * Order that items are set:
 *  1. eccen bit in NB
 *  2. Scrub Base
 *  3. Temp Node Base
 *  4. Temp Node Limit
 *  5. Redir bit in NB
 *  6. Scrub CTL
 *
 * Conditions for setting background scrubber.
 *  1. node is present
 *  2. node has dram functioning (WE=RE=1)
 *  3. all eccdimms (or bit 17 of offset 90,fn 2)
 *  4. no chip-select gap exists
 *
 * The dram background scrubber is used under very controlled circumstances to
 * initialize all the ECC bits on the DIMMs of the entire dram address map
 * (including hidden or lost dram and dram above 4GB). We will turn the scrub
 * rate up to maximum, which should clear 4GB of dram in about 2.7 seconds.
 * We will activate the scrubbers of all nodes with ecc dram and let them run in
 * parallel, thereby reducing even further the time required to condition dram.
 * Finally, we will go through each node and either disable background scrubber,
 *  or set the scrub rate to the user setup specified rate.
 *
 * To allow the NB to scrub, we need to wait a time period long enough to
 * guarantee that the NB scrubs the entire dram on its node. Do do this, we
 * simply sample the scrub ADDR once, for an initial value, then we sample and poll until the polled value of scrub ADDR
 * has wrapped around at least once: Scrub ADDRi+1 < Scrub ADDRi. Since we let all
 * Nodes run in parallel, we need to guarantee that all nodes have wrapped. To do
 * this efficiently, we need only to sample one of the nodes, the node with the
 * largest ammount of dram populated is the one which will take the longest amount
 * of time (the scrub rate is set to max, the same rate, on all nodes).  So,
 * during setup of scrub Base, we determine how much memory and which node has
 * the largest memory installed.
 *
 * Scrubbing should not ordinarily be enabled on a Node with a chip-select gap
 * (aka SW memhole, cs hoisting, etc..).To init ECC memory on this node, the
 * scrubber is used in two steps.  First, the Dram Limit for the node is adjusted
 * down to the bottom of the gap, and that ECC dram is initialized.  Second, the
 * orignal Limit is restored, the Scrub base is set to 4GB, and scrubber is
 * allowed to run until the Scrub Addr wraps around to zero.
 */
u8 ECCInit_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA)
{
	u8 Node;
	u8 AllECC;
	u16 OB_NBECC;
	u32 curBase;
	u16 OB_ECCRedir;
	u32 LDramECC;
	u32 OF_ScrubCTL;
	u16 OB_ChipKill;
	u8 MemClrECC;

	u32 dev;
	u32 reg;
	u32 val;
	u16 nvbits;

	mctHookBeforeECC();

	/* Construct these booleans, based on setup options, for easy handling
	later in this procedure */
	OB_NBECC = mctGet_NVbits(NV_NBECC);	/* MCA ECC (MCE) enable bit */

	OB_ECCRedir =  mctGet_NVbits(NV_ECCRedir);	/* ECC Redirection */

	OB_ChipKill = mctGet_NVbits(NV_ChipKill); 	/* ECC Chip-kill mode */

	OF_ScrubCTL = 0;		/* Scrub CTL for Dcache, L2, and dram */
	nvbits = mctGet_NVbits(NV_DCBKScrub);
	/* mct_AdjustScrub_D(pDCTstatA, &nvbits); */ /* Need not adjust */
	OF_ScrubCTL |= (u32) nvbits << 16;

	nvbits = mctGet_NVbits(NV_L2BKScrub);
	OF_ScrubCTL |= (u32) nvbits << 8;

	nvbits = mctGet_NVbits(NV_DramBKScrub);
	OF_ScrubCTL |= nvbits;

	AllECC = 1;
	MemClrECC = 0;
	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;
		LDramECC = 0;
		if (NodePresent_D(Node)) {	/*If Node is present */
			dev = pDCTstat->dev_map;
			reg = 0x40+(Node << 3);	/* Dram Base Node 0 + index */
			val = Get_NB32(dev, reg);

			/* WE/RE is checked */
			if((val & 3)==3) {	/* Node has dram populated */
				/* Negate 'all nodes/dimms ECC' flag if non ecc
				   memory populated */
				if( pDCTstat->Status & (1<<SB_ECCDIMMs)) {
					LDramECC = isDramECCEn_D(pDCTstat);
					if(pDCTstat->ErrCode != SC_RunningOK) {
						pDCTstat->Status &=  ~(1 << SB_ECCDIMMs);
						if (!OB_NBECC) {
							pDCTstat->ErrStatus |= (1 << SB_DramECCDis);
						}
						AllECC = 0;
						LDramECC =0;
					}
				} else {
					AllECC = 0;
				}
				if(LDramECC) {	/* if ECC is enabled on this dram */
					if (OB_NBECC) {
						mct_EnableDatIntlv_D(pMCTstat, pDCTstat);
						dev = pDCTstat->dev_nbmisc;
						reg =0x44;	/* MCA NB Configuration */
						val = Get_NB32(dev, reg);
						val |= 1 << 22;	/* EccEn */
						Set_NB32(dev, reg, val);
						DCTMemClr_Init_D(pMCTstat, pDCTstat);
						MemClrECC = 1;
						printk(BIOS_DEBUG, "  ECC enabled on node: %02x\n", Node);
					}
				}	/* this node has ECC enabled dram */
			} else {
				LDramECC = 0;
			}	/* Node has Dram */

			if (MemClrECC) {
				MCTMemClrSync_D(pMCTstat, pDCTstatA);
			}
		}	/* if Node present */
	}

	if(AllECC)
		pMCTstat->GStatus |= 1<<GSB_ECCDIMMs;
	else
		pMCTstat->GStatus &= ~(1<<GSB_ECCDIMMs);

	/* Program the Dram BKScrub CTL to the proper (user selected) value.*/
	/* Reset MC4_STS. */
	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;
		LDramECC = 0;
		if (NodePresent_D(Node)) {	/* If Node is present */
			reg = 0x40+(Node<<3);	/* Dram Base Node 0 + index */
			val = Get_NB32(pDCTstat->dev_map, reg);
			curBase = val & 0xffff0000;
			/*WE/RE is checked because memory config may have been */
			if((val & 3)==3) {	/* Node has dram populated */
				if (isDramECCEn_D(pDCTstat)) {	/* if ECC is enabled on this dram */
					dev = pDCTstat->dev_nbmisc;
					val = curBase << 8;
					if(OB_ECCRedir) {
						val |= (1<<0); /* enable redirection */
					}
					Set_NB32(dev, 0x5C, val); /* Dram Scrub Addr Low */
					val = curBase>>24;
					Set_NB32(dev, 0x60, val); /* Dram Scrub Addr High */
					Set_NB32(dev, 0x58, OF_ScrubCTL);	/*Scrub Control */

					/* Divisor should not be set deeper than
					 * divide by 16 when Dcache scrubber or
					 * L2 scrubber is enabled.
					 */
					if ((OF_ScrubCTL & (0x1F << 16)) || (OF_ScrubCTL & (0x1F << 8))) {
						val = Get_NB32(dev, 0x84);
						if ((val & 0xE0000000) > 0x80000000) {	/* Get F3x84h[31:29]ClkDivisor for C1 */
							val &= 0x1FFFFFFF;	/* If ClkDivisor is deeper than divide-by-16 */
							val |= 0x80000000;	/* set it to divide-by-16 */
							Set_NB32(dev, 0x84, val);
						}
					}
				}	/* this node has ECC enabled dram */
			}	/*Node has Dram */
		}	/*if Node present */
	}

	if(mctGet_NVbits(NV_SyncOnUnEccEn))
		setSyncOnUnEccEn_D(pMCTstat, pDCTstatA);

	mctHookAfterECC();
	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;
		if (NodePresent_D(Node)) {
			printk(BIOS_DEBUG, "ECCInit: Node %02x\n", Node);
			printk(BIOS_DEBUG, "ECCInit: Status %x\n", pDCTstat->Status);
			printk(BIOS_DEBUG, "ECCInit: ErrStatus %x\n", pDCTstat->ErrStatus);
			printk(BIOS_DEBUG, "ECCInit: ErrCode %x\n", pDCTstat->ErrCode);
			printk(BIOS_DEBUG, "ECCInit: Done\n");
		}
	}
	return MemClrECC;
}

static void setSyncOnUnEccEn_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{
	u32 Node;
	u32 reg;
	u32 dev;
	u32 val;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;
		if (NodePresent_D(Node)) {	/* If Node is present*/
			reg = 0x40+(Node<<3);	/* Dram Base Node 0 + index*/
			val = Get_NB32(pDCTstat->dev_map, reg);
			/*WE/RE is checked because memory config may have been*/
			if((val & 3)==3) {	/* Node has dram populated*/
				if( isDramECCEn_D(pDCTstat)) {
					/*if ECC is enabled on this dram*/
					dev = pDCTstat->dev_nbmisc;
					reg = 0x44;	/* MCA NB Configuration*/
					val = Get_NB32(dev, reg);
					val |= (1<<SyncOnUcEccEn);
					Set_NB32(dev, reg, val);
				}
			}	/* Node has Dram*/
		}	/* if Node present*/
	}
}

static u8 isDramECCEn_D(struct DCTStatStruc *pDCTstat)
{
	u32 reg;
	u32 val;
	u8 i;
	u32 dev = pDCTstat->dev_dct;
	u8 ch_end;
	u8 isDimmECCEn = 0;

	if(pDCTstat->GangedMode) {
		ch_end = 1;
	} else {
		ch_end = 2;
	}
	for(i=0; i<ch_end; i++) {
		if(pDCTstat->DIMMValidDCT[i] > 0){
			reg = 0x90 + i * 0x100;		/* Dram Config Low */
			val = Get_NB32(dev, reg);
			if(val & (1<<DimmEcEn)) {
				/* set local flag 'dram ecc capable' */
				isDimmECCEn = 1;
				break;
			}
		}
	}
	return isDimmECCEn;
}
