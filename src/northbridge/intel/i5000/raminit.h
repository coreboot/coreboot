/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef NORTHBRIDGE_I5000_RAMINIT_H
#define NORTHBRIDGE_I5000_RAMINIT_H

#include <types.h>
#include <arch/io.h>

#define I5000_MAX_BRANCH 2
#define I5000_MAX_CHANNEL 2
#define I5000_MAX_DIMM_PER_CHANNEL 4
#define I5000_MAX_DIMMS (I5000_MAX_BRANCH * I5000_MAX_CHANNEL * I5000_MAX_DIMM_PER_CHANNEL)

#define I5000_FBDRST 0x53

#define I5000_SPD_BUSY (1 << 12)
#define I5000_SPD_SBE (1 << 13)
#define I5000_SPD_WOD (1 << 14)
#define I5000_SPD_RDO (1 << 15)

#define I5000_SPD0 0x74
#define I5000_SPD1 0x76

#define I5000_SPDCMD0 0x78
#define I5000_SPDCMD1 0x7c

#define I5000_FBDHPC 0x4f
#define I5000_FBDST  0x4b

#define I5000_FBDHPC_STATE_RESET 0x00
#define I5000_FBDHPC_STATE_INIT 0x10
#define I5000_FBDHPC_STATE_READY 0x20
#define I5000_FBDHPC_STATE_ACTIVE 0x30

#define I5000_FBDISTS0 0x58
#define I5000_FBDISTS1 0x5a

#define I5000_FBDLVL0 0x44
#define I5000_FBDLVL1 0x45

#define I5000_FBDICMD0 0x46
#define I5000_FBDICMD1 0x47

#define I5000_FBDICMD_IDLE 0x00
#define I5000_FBDICMD_TS0  0x80
#define I5000_FBDICMD_TS1  0x90
#define I5000_FBDICMD_TS2  0xa0
#define I5000_FBDICMD_TS3  0xb0
#define I5000_FBDICMD_TS2_MERGE 0xd0
#define I5000_FBDICMD_TS2_NOMERGE 0xe0
#define I5000_FBDICMD_ALL_ONES 0xf0

#define I5000_AMBPRESENT0 0x64
#define I5000_AMBPRESENT1 0x66

#define I5000_FBDSBTXCFG0 0xc0
#define I5000_FBDSBTXCFG1 0xc1

#define I5000_PROCENABLE 0xf0
#define I5000_FBD0IBPORTCTL 0x180
#define I5000_FBD0IBTXPAT2EN 0x1a8
#define I5000_FBD0IBRXPAT2EN 0x1ac

#define I5000_FBD0IBTXMSK 0x18c
#define I5000_FBD0IBRXMSK 0x190

#define I5000_FBDPLLCTRL 0x1c0

/* dev 16, function 1 registers */
#define I5000_MC 0x40
#define I5000_DRTA 0x48
#define I5000_DRTB 0x4c
#define I5000_ERRPERR 0x50
#define I5000_MCA 0x58
#define I5000_TOLM 0x6c
#define I5000_MIR0 0x80
#define I5000_MIR1 0x84
#define I5000_MIR2 0x88
#define I5000_AMIR0 0x8c
#define I5000_AMIR1 0x90
#define I5000_AMIR2 0x94

#define I5000_FERR_FAT_FBD 0x98
#define I5000_NERR_FAT_FBD 0x9c
#define I5000_FERR_NF_FBD 0xa0
#define I5000_NERR_NF_FBD 0xa4
#define I5000_EMASK_FBD 0xa8
#define I5000_ERR0_FBD 0xac
#define I5000_ERR1_FBD 0xb0
#define I5000_ERR2_FBD 0xb4
#define I5000_MCERR_FBD 0xb8
#define I5000_NRECMEMA 0xbe
#define I5000_NRECMEMB 0xc0
#define I5000_NRECFGLOG 0xc4
#define I5000_NRECMEMA 0xbe
#define I5000_NRECFBDA 0xc8
#define I5000_NRECFBDB 0xcc
#define I5000_NRECFBDC 0xd0
#define I5000_NRECFBDD 0xd4
#define I5000_NRECFBDE 0xd8

#define I5000_REDMEMB 0x7c
#define I5000_RECMEMA 0xe2
#define I5000_RECMEMB 0xe4
#define I5000_RECFGLOG 0xe8
#define I5000_RECFBDA 0xec
#define I5000_RECFBDB 0xf0
#define I5000_RECFBDC 0xf4
#define I5000_RECFBDD 0xf8
#define I5000_RECFBDE 0xfc

#define I5000_FBDTOHOSTGRCFG0 0x160
#define I5000_FBDTOHOSTGRCFG1 0x164
#define I5000_HOSTTOFBDGRCFG 0x168
#define I5000_GRFBDLVLDCFG 0x16c
#define I5000_GRHOSTFULLCFG 0x16d
#define I5000_GRBUBBLECFG 0x16e
#define I5000_GRFBDTOHOSTDBLCFG 0x16f

/* dev 16, function 2 registers */
#define I5000_FERR_GLOBAL 0x40
#define I5000_NERR_GLOBAL 0x44

/* dev 21, function 0 registers */
#define I5000_MTR0 0x80
#define I5000_MTR1 0x84
#define I5000_MTR2 0x88
#define I5000_MTR3 0x8c
#define I5000_DMIR0 0x90
#define I5000_DMIR1 0x94
#define I5000_DMIR2 0x98
#define I5000_DMIR3 0x9c
#define I5000_DMIR4 0xa0

#define DEFAULT_AMBASE 0xfe000000

/* AMB function 1 registers */
#define AMB_FBDSBCFGNXT 0x54
#define AMB_FBDLOCKTO 0x68
#define AMB_EMASK 0x8c
#define AMB_FERR 0x90
#define AMB_NERR 0x94
#define AMB_CMD2DATANXT 0xe8

/* AMB function 3 registers */
#define AMB_DAREFTC 0x70
#define AMB_DSREFTC 0x74
#define AMB_DRT 0x78
#define AMB_DRC 0x7c

#define AMB_MBCSR 0x40
#define AMB_MBADDR 0x44
#define AMB_MBLFSRSED 0xa4

/* AMB function 4 registers */
#define AMB_DCALCSR  0x40
#define AMB_DCALADDR 0x44
#define AMB_DCALCSR_START (1 << 31)

#define AMB_DCALCSR_OPCODE_NOP                0x00
#define AMB_DCALCSR_OPCODE_REFRESH            0x01
#define AMB_DCALCSR_OPCODE_PRECHARGE          0x02
#define AMB_DCALCSR_OPCODE_MRS_EMRS           0x03
#define AMB_DCALCSR_OPCODE_DQS_DELAY_CAL      0x05
#define AMB_DCALCSR_OPCODE_RECV_ENABLE_CAL    0x0c
#define AMB_DCALCSR_OPCODE_SELF_REFRESH_ENTRY 0x0d

#define AMB_DDR2ODTC 0xfc

#define FBDIMM_SPD_SDRAM_ADDRESSING 0x04
#define FBDIMM_SPD_MODULE_ORGANIZATION 0x07
#define FBDIMM_SPD_FTB 0x08
#define FBDIMM_SPD_MTB_DIVIDEND 0x09
#define FBDIMM_SPD_MTB_DIVISOR 0x0a
#define FBDIMM_SPD_MIN_TCK 0x0b
#define FBDIMM_SPD_CAS_LATENCIES 0x0d
#define FBDIMM_SPD_CAS_MIN_LATENCY 0x0e
#define FBDIMM_SPD_T_WR 0x10
#define FBDIMM_SPD_T_RCD 0x13
#define FBDIMM_SPD_T_RRD 0x14
#define FBDIMM_SPD_T_RP 0x15
#define FBDIMM_SPD_T_RAS_RC_MSB 0x16
#define FBDIMM_SPD_T_RAS 0x17
#define FBDIMM_SPD_T_RC 0x18
#define FBDIMM_SPD_T_RFC 0x19
#define FBDIMM_SPD_T_WTR 0x1b
#define FBDIMM_SPD_T_RTP 0x1c
#define FBDIMM_SPD_BURST_LENGTHS_SUPPORTED 0x1d
#define FBDIMM_SPD_ODT 0x4f
#define FBDIMM_SPD_T_REFI 0x20
#define FBDIMM_SPD_T_BB 0x83
#define FBDIMM_SPD_CMD2DATA_800 0x54
#define FBDIMM_SPD_CMD2DATA_667 0x55
#define FBDIMM_SPD_CMD2DATA_533 0x56

void i5000_fbdimm_init(void);

#define I5000_BURST4 0x01
#define I5000_BURST8 0x02
#define I5000_BURST_CHOP 0x80

#define I5000_ODT_50 4
#define I5000_ODT_75 2
#define I5000_ODT_150 1

enum ddr_speeds {
	DDR_533MHZ,
	DDR_667MHZ,
	DDR_MAX,
};

struct i5000_fbdimm {
	struct i5000_fbd_branch *branch;
	struct i5000_fbd_channel *channel;
	struct i5000_fbd_setup *setup;
	enum ddr_speeds speed;
	int num;
	int present:1;
	u32 ambase;

	/* SPD data */
	u8 amb_personality_bytes[14];
	u8 banks;
	u8 rows;
	u8 columns;
	u8 ranks;
	u8 odt;
	u8 sdram_width;
	u8 mtb_divisor;
	u8 mtb_dividend;
	u8 t_ck_min;
	u8 min_cas_latency;
	u8 t_rrd;
	u16 t_rfc;
	u8 t_wtr;
	u8 t_refi;
	u8 cmd2datanxt[DDR_MAX];

	u16 vendor;
	u16 device;

	/* memory rank size in MB */
	int ranksize;
};

struct i5000_fbd_channel {
	struct i5000_fbdimm dimm[I5000_MAX_DIMM_PER_CHANNEL];
	struct i5000_fbd_branch *branch;
	struct i5000_fbd_setup *setup;
	int num;
	int used;
	int highest_amb;
	int columns;
	int rows;
	int ranks;
	int banks;
	int width;
	/* memory size in MB on this channel */
	int totalmem;
};

struct i5000_fbd_branch {
	struct i5000_fbd_channel channel[I5000_MAX_CHANNEL];
	struct i5000_fbd_setup *setup;
	device_t branchdev;
	int num;
	int used;
	/* memory size in MB on this branch */
	int totalmem;
};

enum odt {
	ODT_150OHM=1,
	ODT_50OHM=4,
	ODT_75OHM=2,
};

enum bl {
	BL_BL4=1,
	BL_BL8=2,
};

struct i5000_fbd_setup {
	struct i5000_fbd_branch branch[I5000_MAX_BRANCH];
	struct i5000_fbdimm *dimms[I5000_MAX_DIMMS];
	enum bl bl;
	enum ddr_speeds ddr_speed;

	int single_channel:1;
	u32 tolm;

	/* global SDRAM timing parameters */
	u8 t_al;
	u8 t_cl;
	u8 t_ras;
	u8 t_wrc;
	u8 t_rc;
	u8 t_rfc;
	u8 t_rrd;
	u8 t_ref;
	u8 t_w2rdr;
	u8 t_r2w;
	u8 t_w2r;
	u8 t_r2r;
	u8 t_w2w;
	u8 t_wtr;
	u8 t_rcd;
	u8 t_rp;
	u8 t_wr;
	u8 t_rtp;
	/* memory size in MB */
	int totalmem;
};

int mainboard_set_fbd_clock(int);
#define AMB_ADDR(base, fn, reg)	(base | ((fn & 7) << 8) | ((reg & 0xff)))
#endif
