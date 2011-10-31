/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Digital Design Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
 * e7505.h: PCI configuration space for the Intel E7501 memory controller
 */

/************  D0:F0 ************/
// Register offsets
#define MAYBE_SMRBASE	0x14	/* System Memory RCOMP Base Address Register, 32 bit? (if similar to 855PM) */
#define MCHCFGNS		0x52	/* MCH (scrubber) configuration register, 16 bit */
#define DRB_ROW_0		0x60	/* DRAM Row Boundary register, 8 bit */
#define DRB_ROW_1		0x61
#define DRB_ROW_2		0x62
#define DRB_ROW_3		0x63
#define DRB_ROW_4		0x64
#define DRB_ROW_5		0x65
#define DRB_ROW_6		0x66
#define DRB_ROW_7		0x67

#define DRA				0x70	/* DRAM Row Attributes registers, 4 x 8 bit */
#define DRT				0x78	/* DRAM Timing register, 32 bit */
#define DRC				0x7C	/* DRAM Controller Mode register, 32 bit */
#define MAYBE_DRDCTL	0x80	/* DRAM Read Timing Control register, 16 bit? (if similar to 855PM) */
#define CKDIS			0x8C	/* Clock disable register, 8 bit */
#define TOLM			0xC4	/* Top of Low Memory register, 16 bit */
#define REMAPBASE		0xC6	/* Remap Base Address register, 16 bit */
#define REMAPLIMIT		0xC8	/* Remap Limit Address register, 16 bit */
#define SKPD			0xDE	/* Scratchpad register, 16 bit */
#define MAYBE_MCHTST	0xF4	/* MCH Test Register, 32 bit? (if similar to 855PM) */

// CAS# Latency bits in the DRAM Timing (DRT) register
#define DRT_CAS_2_5		(0<<4)
#define DRT_CAS_2_0		(1<<4)
#define DRT_CAS_MASK	(3<<4)

// Mode Select (SMS) bits in the DRAM Controller Mode (DRC) register
#define RAM_COMMAND_NOP			(1<<4)
#define RAM_COMMAND_PRECHARGE	(2<<4)
#define RAM_COMMAND_MRS			(3<<4)
#define RAM_COMMAND_EMRS		(4<<4)
#define RAM_COMMAND_CBR			(6<<4)
#define RAM_COMMAND_NORMAL		(7<<4)


// RCOMP Memory Map offsets
// Conjecture based on apparent similarity between E7501 and 855PM
// Intel doc. 252613-003 describes these for 855PM

#define MAYBE_SMRCTL		0x20	/* System Memory RCOMP Control Register? */
#define MAYBE_DQCMDSTR		0x30	/* Strength control for DQ and CMD signal groups? */
#define MAYBE_CKESTR		0x31	/* Strength control for CKE signal group? */
#define MAYBE_CSBSTR		0x32	/* Strength control for CS# signal group? */
#define MAYBE_CKSTR			0x33	/* Strength control for CK signal group? */
#define MAYBE_RCVENSTR		0x34	/* Strength control for RCVEnOut# signal group? */

/************  D0:F1 ************/
// Register offsets
#define FERR_GLOBAL			0x40	/* First global error register, 32 bits */
#define NERR_GLOBAL			0x44	/* Next global error register, 32 bits */
#define DRAM_FERR			0x80	/* DRAM first error register, 8 bits */
#define DRAM_NERR			0x82	/* DRAM next error register, 8 bits */
