/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <delay.h>
#include <spd.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <device/dram/common.h>
#include <static_devices.h>

#include "chip.h"
#include "registers.h"
#include "raminit.h"

struct mem_controller {
	struct dram_cfg cfg;
};

#define SPD_MOD_ATTRIB_RANK		5
#define  SPD_MOD_ATTRIB_RANK_NUM_MASK	0x07
#define SPD_PRI_WIDTH			13
#define SPD_CAS_LAT			18
#define  SPD_CAS_LATx(x)		(1 << (x))
#define SPD_CAS_LAT_MIN_X_1		23
#define SPD_CAS_LAT_MIN_X_2		25
#define SPD_TRP				27
#define SPD_TRRD			28
#define SPD_TRCD			29
#define SPD_TRAS			30
#define SPD_RANK_SIZE			31      /* Only one bit is set */
#define SPD_TWR				36
#define SPD_TWTR			37
#define SPD_TRTP			38
#define SPD_EX_TRC_TRFC			40
#define SPD_TRFC			42

#define RAM_COMMAND_NORMAL	0x0
#define RAM_COMMAND_NOP		0x1
#define RAM_COMMAND_PRECHARGE	0x2
#define RAM_COMMAND_MRS		0x3
#define RAM_COMMAND_CBR		0x4

#define HOSTCTRL		_sdev_host_if
#define MEMCTRL			_sdev_dram_ctrl
#define SCRATCH			_sdev_pm_ctrl

#define	SDRAM1X_RA_14		31
#define	SDRAM1X_RA_13		30
#define	SDRAM1X_RA_12		29
#define	SDRAM1X_RA_12_8bk	27
#define SDRAM1X_CA_12		16
#define SDRAM1X_CA_11		15
#define SDRAM1X_CA_09		12
#define SDRAM1X_CA_09_8bk	12
#define SDRAM1X_BA1		14
#define SDRAM1X_BA2_8bk		15
#define SDRAM1X_BA1_8bk		14

#define	MA_Column	0x06
#define	MA_Bank		0x08
#define	MA_Row		0x30
#define	MA_4_Bank	0x00
#define	MA_8_Bank	0x08
#define	MA_12_Row	0x00
#define	MA_13_Row	0x10
#define	MA_14_Row	0x20
#define	MA_15_Row	0x30
#define	MA_9_Column	0x00
#define	MA_10_Column	0x02
#define	MA_11_Column	0x04
#define	MA_12_Column	0x06

static u8 get_spd_data(const struct dram_cfg *cfg, unsigned int socket, u8 reg)
{
	return cfg->spd_addr[socket] ? smbus_read_byte(cfg->spd_addr[socket], reg) : 0x00;
}

#define	GET_SPD(i, val, tmp, reg)							\
	do {										\
		val = 0;								\
		tmp = 0;								\
		for (i = 0; i < DIMM_SOCKETS; i++)	{				\
			if (pci_read_config8(SCRATCH, SCRATCH_RANK_0 + (i << 1))) {	\
				tmp = get_spd_data(&ctrl->cfg, i, reg);			\
				if (tmp > val)						\
					val = tmp;					\
			}								\
		}									\
	} while (0)

#define REGISTERPRESET(bus, dev, fun, bdfspec) \
	{ u8 j, reg; \
		for (j = 0; j < ARRAY_SIZE(bdfspec); j++) { \
			printk(BIOS_SPEW, "Writing bus " #bus " dev " #dev " fun " #fun " register "); \
			printk(BIOS_SPEW, "%02x", (bdfspec)[j].reg); \
			printk(BIOS_SPEW, "\n"); \
			reg = pci_read_config8(PCI_DEV((bus), (dev), (fun)), (bdfspec)[j].reg); \
			reg &= (bdfspec)[j].mask; \
			reg |= (bdfspec)[j].val; \
			pci_write_config8(PCI_DEV((bus), (dev), (fun)), (bdfspec)[j].reg, reg); \
		} \
	}

static const u8 Reg_Val[] = {
/*	REG,  VALUE */
	0x70, 0x33,
	0x71, 0x11,
	0x72, 0x33,
	0x73, 0x11,
	0x74, 0x20,
	0x75, 0x2e,
	0x76, 0x64,
	0x77, 0x00,
	0x78, 0x44,
	0x79, 0xaa,
	0x7a, 0x33,
	0x7b, 0xaa,
	0x7c, 0x00,
	0x7e, 0x33,
	0x7f, 0x33,
	0x80, 0x44,
	0x81, 0x44,
	0x82, 0x44,
	0x83, 0x02,
	0x50, 0x88,
	0x51, 0x7b,
	0x52, 0x6f,
	0x53, 0x88,
	0x54, 0x0e,
	0x55, 0x00,
	0x56, 0x00,
	0x59, 0x00,
	0x5d, 0x72,
	0x5e, 0x88,
	0x5f, 0xc7,
	0x68, 0x01,
};

/* Host registers initial value */
static const u8 Host_Reg_Val[] = {
/*	REG,  VALUE */
	0x60, 0xff,
	0x61, 0xff,
	0x62, 0x0f,
	0x63, 0xff,
	0x64, 0xff,
	0x65, 0x0f,
	0x66, 0xff,
	0x67, 0x30,
};

static const u8 Mem_Reg_Init[] = {
/*	REG,  AND,  OR	*/
	0x50, 0x11, 0x66,
	0x51, 0x11, 0x66,
	0x52, 0x00, 0x11,
	0x53, 0x00, 0x0f,
	0x54, 0x00, 0x00,
	0x55, 0x00, 0x00,
	0x56, 0x00, 0x00,
	0x57, 0x00, 0x00,
	0x60, 0x00, 0x00,
	0x62, 0xf7, 0x08,
	0x65, 0x00, 0xd9,
	0x66, 0x00, 0x80,
	0x67, 0x00, 0x50,	/* OR 0x00 ?? */
	0x69, 0xf0, 0x00,
	0x6a, 0x00, 0x00,
	0x6d, 0xcf, 0xc0,
	0x6e, 0xff, 0x80,
	0x75, 0x0f, 0x40,
	0x77, 0x00, 0x00,
	0x80, 0x00, 0x00,
	0x81, 0x00, 0x00,
	0x82, 0x00, 0x00,
	0x83, 0x00, 0x00,
	0x84, 0x00, 0x00,
	0x85, 0x00, 0x00,
	0x86, 0xff, 0x2c,	/* OR 0x28 if we don't want enable top 1M SM memory */
	0x40, 0x00, 0x00,
	0x7c, 0x00, 0x00,
	0x7e, 0x00, 0x00,
	0xa4, 0xfe, 0x00,
	0xb0, 0x7f, 0x80,
	0xb1, 0x00, 0xaa,
	0xb4, 0xfd, 0x02,
	0xb8, 0xfe, 0x00,
};

static const u8 Dram_Driving_ODT_CTRL[] = {
/*	REG,  VALUE */
	0xd6, 0xa8,
	0xd4, 0x80,
	0xd0, 0x88,
	0xd3, 0x01,
	0xd8, 0x00,
	0xda, 0x80,
};

#define	Rank0_ODT	0x00
#define	Rank1_ODT	0x01
#define	Rank2_ODT	0x02
#define	Rank3_ODT	0x03
#define	NA_ODT		0x00
#define	NB_ODT_75ohm	0x00
#define	NB_ODT_150ohm	0x01
#define	DDR2_ODT_75ohm  0x20
#define	DDR2_ODT_150ohm 0x40

static const u8 ODT_TBL[] = {
/*	RankMap, ODT Control Bits,							DRAM & NB ODT setting	*/
	0x01,	 ((NA_ODT << 6)    | (NA_ODT << 4)    | (NA_ODT << 2)    | Rank0_ODT),	(DDR2_ODT_150ohm | NB_ODT_75ohm),
	0x03,	 ((NA_ODT << 6)    | (NA_ODT << 4)    | (Rank0_ODT << 2) | Rank1_ODT),	(DDR2_ODT_150ohm | NB_ODT_75ohm),
	0x04,	 ((NA_ODT << 6)    | (Rank2_ODT << 4) | (NA_ODT << 2)    | NA_ODT),	(DDR2_ODT_150ohm | NB_ODT_75ohm),
	0x05,	 ((NA_ODT << 6)    | (Rank0_ODT << 4) | (NA_ODT << 2)    | Rank2_ODT),	(DDR2_ODT_75ohm  | NB_ODT_150ohm),
	0x07,	 ((NA_ODT << 6)    | (Rank0_ODT << 4) | (Rank2_ODT << 2) | Rank2_ODT),	(DDR2_ODT_75ohm  | NB_ODT_150ohm),
	0x0c,	 ((Rank2_ODT << 6) | (Rank3_ODT << 4) | (NA_ODT << 2)    | NA_ODT),	(DDR2_ODT_150ohm | NB_ODT_75ohm),
	0x0d,	 ((Rank0_ODT << 6) | (Rank0_ODT << 4) | (NA_ODT << 2)    | Rank2_ODT),	(DDR2_ODT_75ohm  | NB_ODT_150ohm),
	0x0f,	 ((Rank0_ODT << 6) | (Rank0_ODT << 4) | (Rank2_ODT << 2) | Rank2_ODT),	(DDR2_ODT_75ohm  | NB_ODT_150ohm),
};

static const u8 DQS_DQ_TBL[] = {
/* RxE0: DRAM Timing DQS */
/* RxE2: DRAM Timing DQ */
/*	RxE0, RxE2 */
	0xee, 0xba,
	0xee, 0xba,
	0xcc, 0xba,
	0xcc, 0xba,
};

static const u8 Duty_Control_DDR2[] = {
/*	RxEC, RxED, RxEE, RXEF */
	/* DDRII533 1~2 rank, DDRII400 */
	0x84, 0x10, 0x00, 0x10,
	/* DDRII533 3~4 rank    */
	0x44, 0x10, 0x00, 0x10,
};

static const u8 ChA_Clk_Phase_DDR2_Table[] = {
/*	Rx91, Rx92, Rx93 */
	/* DDRII533 1 rank */
	0x04, 0x05, 0x06,
	/* DDRII533 2~4 rank */
	0x04, 0x05, 0x05,
	/* DDRII400 */
	0x02, 0x04, 0x04,
};

static const u8 DQ_DQS_Table[] = {
/*	REG,  VALUE */
/* DRAM DQ/DQS Output Delay Control */
	0xdc, 0x65,
	0xdd, 0x01,
	0xde, 0xc0,
/* DRAM DQ/DQS input Capture Control */
	0x78, 0x83,
	0x79, 0x83,
	0x7a, 0x00,
};

static const u8 DQSOChA_DDR2_Driving_Table[] = {
/*	Rx70, Rx71 */
	/* DDRII533 1~2 rank */
	0x00, 0x01,
	/* DDRII533 3~4 rank */
	0x03, 0x00,
	/* DDRII400 1~2 rank */
	0x00, 0x04,
	/* DDRII400 3~4 rank */
	0x00, 0x01,
};

/************************************************************************/
/* Chipset Performance UP and other setting after DRAM Sizing Registers	*/
/************************************************************************/
static const u8 Dram_Table[] = {
/*	REG,  AND,  OR	*/
	0x60, 0xff, 0x03,
	0x66, 0xcf, 0x80,
	0x68, 0x00, 0x00,
	0x69, 0xfd, 0x03,
	0x6e, 0xff, 0x01,
	0x95, 0xff, 0x40,
};

static const u8 Host_Table[] = {
/*	REG,	AND,	OR */
	0x51, 0x81, 0x7a,
	0x55, 0xff, 0x06,
	0x5e, 0x00, 0x88,
	0x5d, 0xff, 0xb2,
};

static const u8 Init_Rank_Reg_Table[] = {
	/* Rank Ending Address Registers */
	0x40, 0x41, 0x42, 0x43,
	/* Rank Beginning Address Registers */
	0x48, 0x49, 0x4a, 0x4b,
	/* Physical-to-Virtual Rank Mapping Registers */
	0x54, 0x55,
};

static const u16 DDR2_MRS_table[] = {
/* CL:	2,     3,     4,     5 */
	0x150, 0x1d0, 0x250, 0x2d0,	/* BL = 4; Use 1X-bandwidth MA table to init DRAM */
	0x158, 0x1d8, 0x258, 0x2d8,	/* BL = 8; Use 1X-bandwidth MA table to init DRAM */
};

#define	MRS_DDR2_TWR2	((0 << 15) | (0 << 20) | (1 << 12))
#define	MRS_DDR2_TWR3	((0 << 15) | (1 << 20) | (0 << 12))
#define	MRS_DDR2_TWR4	((0 << 15) | (1 << 20) | (1 << 12))
#define	MRS_DDR2_TWR5	((1 << 15) | (0 << 20) | (0 << 12))
static const u32 DDR2_Twr_table[] = {
	MRS_DDR2_TWR2,
	MRS_DDR2_TWR3,
	MRS_DDR2_TWR4,
	MRS_DDR2_TWR5,
};

static const u8 DQSI_Rate_Table[] = {
	8,			/* DDRII 200 */
	8,			/* DDRII 266 */
	8,			/* DDRII 333 */
	7,			/* DDRII 400 */
	8,			/* DDRII 533 */
	8,			/* DDRII 666 */
};

static const u8 REFC_Table[] = {
	0x65, 0x32,		/* DDRII 100 */
	0x86, 0x43,		/* DDRII 266 */
	0xa8, 0x54,		/* DDRII 333 */
	0xca, 0x65,		/* DDRII 400 */
	0xca, 0x86,		/* DDRII 533 */
	0xca, 0xa8,		/* DDRII 666 */
};

static void do_ram_command(const struct mem_controller *ctrl, u8 command)
{
	u8 reg;

	reg = pci_read_config8(MEMCTRL, 0x6b);
	reg &= 0xf8;		/* Clear bits 2-0. */
	reg |= command;
	pci_write_config8(MEMCTRL, 0x6b, reg);

	printram("    Sending RAM command 0x%02x\n", reg);
}

// TODO factor out to another file
static void c7_cpu_setup(const struct mem_controller *ctrl)
{
	for (size_t i = 0; i < ARRAY_SIZE(Reg_Val); i += 2)
		pci_write_config8(HOSTCTRL, Reg_Val[i], Reg_Val[i + 1]);
}

static void ddr_detect(const struct mem_controller *ctrl)
{
	/* FIXME: Only supports 2 ranks per DIMM */
	u8 val, rsize, dimm;
	u8 nrank = 0;
	u8 ndimm = 0;
	u8 rmap = 0;
	for (dimm = 0; dimm < DIMM_SOCKETS; dimm++) {
		val = get_spd_data(&ctrl->cfg, dimm, 0);
		if ((val != 0x80) && (val != 0xff))
			continue;

		printk(BIOS_DEBUG, "Found DIMM #%u\n", dimm);
		ndimm++;

		rsize = get_spd_data(&ctrl->cfg, dimm, SPD_RANK_SIZE);
		/* unit is 128M */
		rsize = (rsize << 3) | (rsize >> 5);

		val = get_spd_data(&ctrl->cfg, dimm, SPD_MOD_ATTRIB_RANK);
		switch (val & SPD_MOD_ATTRIB_RANK_NUM_MASK) {
		case 1:
			pci_write_config8(SCRATCH, SCRATCH_RANK_1 + (dimm << 1), rsize);
			rmap |= 1 << ((dimm << 1) + 1);
			nrank++;
			__fallthrough;
		case 0:
			pci_write_config8(SCRATCH, SCRATCH_RANK_0 + (dimm << 1), rsize);
			rmap |= 1 << (dimm << 1);
			nrank++;
		}
	}
	pci_write_config8(SCRATCH, SCRATCH_DIMM_NUM, ndimm);
	pci_write_config8(SCRATCH, SCRATCH_RANK_NUM, nrank);
	pci_write_config8(SCRATCH, SCRATCH_RANK_MAP, rmap);
}

static void sdram_set_safe_values(const struct mem_controller *ctrl)
{
	/* The purpose of this function is to set initial values for the dram
	 * size and timings. It will be replaced with the SPD based function
	 * once the RAM commands are working with these values.
	 */
	u8 regs, val, t, dimm;
	u32 spds, tmp;

	regs = pci_read_config8(MEMCTRL, 0x6c);
	if (regs & (1 << 6))
		printk(BIOS_DEBUG, "DDR2 Detected.\n");
	else
		die("ERROR: DDR1 memory detected but not supported by coreboot.\n");

	/* Enable DDR2 */
	regs |= (1 << 7);
	pci_write_config8(MEMCTRL, 0x6c, regs);

	/* SPD 5 # of ranks */
	pci_write_config8(MEMCTRL, 0x6d, 0xc0);

	/**********************************************/
	/*          Set DRAM Freq (DDR2 533)          */
	/**********************************************/
	/* SPD 9 SDRAM Cycle Time */
	GET_SPD(dimm, spds, regs, 9);

	printk(BIOS_DEBUG, "DDRII ");
	if (spds <= 0x3d) {
		printk(BIOS_DEBUG, "533\n");
		val = DDRII_533;
		t = 38;
	} else if (spds <= 0x50) {
		printk(BIOS_DEBUG, "400\n");
		val = DDRII_400;
		t = 50;
	} else if (spds <= 0x60) {
		printk(BIOS_DEBUG, "333\n");
		val = DDRII_333;
		t = 60;
	} else if (spds <= 0x75) {
		printk(BIOS_DEBUG, "266\n");
		val = DDRII_266;
		t = 75;
	} else {
		printk(BIOS_DEBUG, "200\n");
		val = DDRII_200;
		t = 100;
	}
	/* To store DDRII frequency */
	pci_write_config8(SCRATCH, SCRATCH_DRAM_FREQ, val);

	/* Manual reset and adjust DLL when DRAM change frequency
	 * This is a necessary sequence.
	 */
	udelay(2000);
	regs = pci_read_config8(MEMCTRL, 0x90);
	regs |= 0x7;
	pci_write_config8(MEMCTRL, 0x90, regs);
	udelay(2000);
	regs = pci_read_config8(MEMCTRL, 0x90);
	regs &= ~0x7;
	regs |= val;
	pci_write_config8(MEMCTRL, 0x90, regs);
	udelay(2000);
	regs = pci_read_config8(MEMCTRL, 0x6b);
	regs |= 0xc0;
	regs &= ~0x10;
	pci_write_config8(MEMCTRL, 0x6b, regs);
	udelay(1);
	regs |= 0x10;
	pci_write_config8(MEMCTRL, 0x6b, regs);
	udelay(1);
	regs &= ~0xc0;
	pci_write_config8(MEMCTRL, 0x6b, regs);
	regs = pci_read_config8(MEMCTRL, 0x6f);
	regs |= 0x1;
	pci_write_config8(MEMCTRL, 0x6f, regs);

	/**********************************************/
	/*      Set DRAM Timing Setting (DDR2 533)    */
	/**********************************************/
	/* SPD 9 18 23 25 CAS Latency NB3DRAM_REG62[2:0] */
	/* Read SPD byte 18 CAS Latency */
	GET_SPD(dimm, spds, regs, SPD_CAS_LAT);
	printk(BIOS_DEBUG, "CAS Supported ");
	if (spds & SPD_CAS_LATx(2))
		printk(BIOS_DEBUG, "2 ");
	if (spds & SPD_CAS_LATx(3))
		printk(BIOS_DEBUG, "3 ");
	if (spds & SPD_CAS_LATx(4))
		printk(BIOS_DEBUG, "4 ");
	if (spds & SPD_CAS_LATx(5))
		printk(BIOS_DEBUG, "5 ");
	if (spds & SPD_CAS_LATx(6))
		printk(BIOS_DEBUG, "6");

	/* We don't consider CAS = 6, because CX700 doesn't support it */
	printk(BIOS_DEBUG, "\n CAS:");
	if (spds & SPD_CAS_LATx(5)) {
		printk(BIOS_DEBUG, "Starting at CL5");
		val = 0x3;
		/* See whether we can improve it */
		GET_SPD(dimm, tmp, regs, SPD_CAS_LAT_MIN_X_1);
		if ((spds & SPD_CAS_LATx(4)) && (tmp < 0x50)) {
			printk(BIOS_DEBUG, "\n... going to CL4");
			val = 0x2;
		}
		GET_SPD(dimm, tmp, regs, SPD_CAS_LAT_MIN_X_2);
		if ((spds & SPD_CAS_LATx(3)) && (tmp < 0x50)) {
			printk(BIOS_DEBUG, "\n... going to CL3");
			val = 0x1;
		}
	} else {
		printk(BIOS_DEBUG, "Starting at CL4");
		val = 0x2;
		GET_SPD(dimm, tmp, regs, SPD_CAS_LAT_MIN_X_1);
		if ((spds & SPD_CAS_LATx(3)) && (tmp < 0x50)) {
			printk(BIOS_DEBUG, "\n... going to CL3");
			val = 0x1;
		}
		GET_SPD(dimm, tmp, regs, SPD_CAS_LAT_MIN_X_2);
		if ((spds & SPD_CAS_LATx(2)) && (tmp < 0x50)) {
			printk(BIOS_DEBUG, "\n... going to CL2");
			val = 0x0;
		}
	}
	regs = pci_read_config8(MEMCTRL, 0x62);
	regs &= ~0x7;
	regs |= val;
	pci_write_config8(MEMCTRL, 0x62, regs);
	printk(BIOS_DEBUG, "\n");

	/* SPD 27 Trp NB3DRAM_REG64[3:2] */
	GET_SPD(dimm, spds, regs, SPD_TRP);
	printk(BIOS_DEBUG, "Trp %d\n", spds);
	spds >>= 2;
	for (val = 2; val <= 5; val++) {
		if (spds <= (val * t / 10)) {
			val = val - 2;
			break;
		}
	}
	val <<= 2;
	regs = pci_read_config8(MEMCTRL, 0x64);
	regs &= ~0xc;
	regs |= val;
	pci_write_config8(MEMCTRL, 0x64, regs);

	/* SPD 29 Trcd NB3DRAM_REG64[7:6] */
	GET_SPD(dimm, spds, regs, SPD_TRCD);
	printk(BIOS_DEBUG, "Trcd %d\n", spds);
	spds >>= 2;
	for (val = 2; val <= 5; val++) {
		if (spds <= (val * t / 10)) {
			val = val - 2;
			break;
		}
	}
	val <<= 6;
	regs = pci_read_config8(MEMCTRL, 0x64);
	regs &= ~0xc0;
	regs |= val;
	pci_write_config8(MEMCTRL, 0x64, regs);

	/* SPD 30 Tras NB3DRAM_REG62[7:4] */
	GET_SPD(dimm, spds, regs, SPD_TRAS);
	printk(BIOS_DEBUG, "Tras %d\n", spds);
	for (val = 5; val <= 20; val++) {
		if (spds <= (val * t / 10)) {
			val = val - 5;
			break;
		}
	}
	val <<= 4;
	regs = pci_read_config8(MEMCTRL, 0x62);
	regs &= ~0xf0;
	regs |= val;
	pci_write_config8(MEMCTRL, 0x62, regs);

	/* SPD 42 SPD 40 Trfc NB3DRAM_REG61[5:0] */
	GET_SPD(dimm, spds, regs, SPD_TRFC);
	printk(BIOS_DEBUG, "Trfc %d\n", spds);
	tmp = spds;
	GET_SPD(dimm, spds, regs, SPD_EX_TRC_TRFC);
	if (spds & 0x1)
		tmp += 256;
	if (spds & 0xe)
		tmp++;
	for (val = 8; val <= 71; val++) {
		if (tmp <= (val * t / 10)) {
			val = val - 8;
			break;
		}
	}
	regs = pci_read_config8(MEMCTRL, 0x61);
	regs &= ~0x3f;
	regs |= val;
	pci_write_config8(MEMCTRL, 0x61, regs);

	/* SPD 28 Trrd NB3DRAM_REG63[7:6] */
	GET_SPD(dimm, spds, regs, SPD_TRRD);
	spds >>= 2;
	for (val = 2; val <= 5; val++) {
		if (spds <= (val * t / 10)) {
			val = val - 2;
			break;
		}
	}
	val <<= 6;
	printk(BIOS_DEBUG, "Trrd val = 0x%x\n", val);

	regs = pci_read_config8(MEMCTRL, 0x63);
	regs &= ~0xc0;
	regs |= val;
	pci_write_config8(MEMCTRL, 0x63, regs);

	/* SPD 36 Twr NB3DRAM_REG61[7:6] */
	GET_SPD(dimm, spds, regs, SPD_TWR);
	for (val = 2; val <= 5; val++) {
		if (spds <= (val * t / 10)) {
			val = val - 2;
			break;
		}
	}
	val <<= 6;
	printk(BIOS_DEBUG, "Twr val = 0x%x\n", val);

	regs = pci_read_config8(MEMCTRL, 0x61);
	regs &= ~0xc0;
	regs |= val;
	pci_write_config8(MEMCTRL, 0x61, regs);

	/* SPD 37 Twtr NB3DRAM_REG63[1] */
	GET_SPD(dimm, spds, regs, SPD_TWTR);
	spds >>= 2;
	printk(BIOS_DEBUG, "Twtr 0x%x\n", spds);
	if (spds <= (t * 2 / 10))
		val = 0;
	else
		val = 1;
	val <<= 1;
	printk(BIOS_DEBUG, "Twtr val = 0x%x\n", val);

	regs = pci_read_config8(MEMCTRL, 0x63);
	regs &= ~0x2;
	regs |= val;
	pci_write_config8(MEMCTRL, 0x63, regs);

	/* SPD 38 Trtp NB3DRAM_REG63[3] */
	GET_SPD(dimm, spds, regs, SPD_TRTP);
	spds >>= 2;
	printk(BIOS_DEBUG, "Trtp 0x%x\n", spds);
	if (spds <= (t * 2 / 10))
		val = 0;
	else
		val = 1;
	val <<= 3;
	printk(BIOS_DEBUG, "Trtp val = 0x%x\n", val);

	regs = pci_read_config8(MEMCTRL, 0x63);
	regs &= ~0x8;
	regs |= val;
	pci_write_config8(MEMCTRL, 0x63, regs);

	/**********************************************/
	/*           Set DRAM DRDY Setting            */
	/**********************************************/
	/* Write slowest value to register */
	for (val = 0; val < ARRAY_SIZE(Host_Reg_Val); val += 2)
		pci_write_config8(HOSTCTRL, Host_Reg_Val[val], Host_Reg_Val[val + 1]);

	/* F2_RX51[7]=0, disable DRDY timing */
	regs = pci_read_config8(HOSTCTRL, 0x51);
	regs &= ~0x80;
	pci_write_config8(HOSTCTRL, 0x51, regs);

	/**********************************************/
	/*           Set DRAM BurstLength             */
	/**********************************************/
	regs = pci_read_config8(MEMCTRL, 0x6c);
	for (dimm = 0; dimm < DIMM_SOCKETS; dimm++) {
		if (pci_read_config8(SCRATCH, SCRATCH_REG_BASE + (dimm << 1))) {
			spds = get_spd_data(&ctrl->cfg, dimm, 16);
			if (!(spds & 0x8))
				break;
		}
	}
	if (dimm == 2)
		regs |= 0x8;
	pci_write_config8(MEMCTRL, 0x6c, regs);
	val = pci_read_config8(HOSTCTRL, 0x54);
	val &= ~0x10;
	if (dimm == 2)
		val |= 0x10;
	pci_write_config8(HOSTCTRL, 0x54, val);

	/**********************************************/
	/*          Set DRAM Driving Setting          */
	/**********************************************/
	/* DRAM Timing ODT */
	for (val = 0; val < ARRAY_SIZE(Dram_Driving_ODT_CTRL); val += 2)
		pci_write_config8(MEMCTRL, Dram_Driving_ODT_CTRL[val],
				  Dram_Driving_ODT_CTRL[val + 1]);

	regs = pci_read_config8(SCRATCH, SCRATCH_RANK_NUM);
	val = pci_read_config8(MEMCTRL, 0xd5);
	val &= ~0xaa;
	switch (regs) {
	case 3:
	case 2:
		val |= 0xa0;
		break;
	default:
		val |= 0x80;
	}
	regs = pci_read_config8(SCRATCH, SCRATCH_DIMM_NUM);
	if (regs == 1)
		val |= 0xa;
	pci_write_config8(MEMCTRL, 0xd5, val);

	regs = pci_read_config8(SCRATCH, SCRATCH_DIMM_NUM);
	val = pci_read_config8(MEMCTRL, 0xd6);
	val &= ~0x2;
	if (regs == 1)
		val |= 0x2;
	pci_write_config8(MEMCTRL, 0xd6, val);

	regs = pci_read_config8(SCRATCH, SCRATCH_RANK_MAP);
	for (val = 0; val < ARRAY_SIZE(ODT_TBL); val += 3) {
		if (regs == ODT_TBL[val]) {
			pci_write_config8(MEMCTRL, 0xd8, ODT_TBL[val + 1]);
			/* Store DRAM & NB ODT setting in d0f4_Rxd8 */
			pci_write_config8(SCRATCH, SCRATCH_DRAM_NB_ODT, ODT_TBL[val + 2]);
			break;
		}
	}

	pci_write_config8(MEMCTRL, 0xd9, 0x0a);
	regs = pci_read_config8(SCRATCH, SCRATCH_RANK_NUM);
	regs--;
	regs = regs << 1;
	pci_write_config8(MEMCTRL, 0xe0, DQS_DQ_TBL[regs++]);
	pci_write_config8(MEMCTRL, 0xe2, DQS_DQ_TBL[regs]);

	/* DRAM Timing CS */
	pci_write_config8(MEMCTRL, 0xe4, 0x66);

	/* DRAM Timing MAA */
	val = 0;
	for (dimm = 0; dimm < DIMM_SOCKETS; dimm++) {
		if (pci_read_config8(SCRATCH, SCRATCH_REG_BASE + (dimm << 1))) {
			spds = get_spd_data(&ctrl->cfg, dimm, SPD_PRI_WIDTH);
			spds = 64 / spds;
			if (pci_read_config8(SCRATCH, SCRATCH_REG_BASE + (dimm << 1) + 1))
				spds = spds << 1;
			val += spds;
		}
	}
	printk(BIOS_DEBUG, "\nchip #%d\n", val);
	if (val > 18)
		regs = 0xdb;
	else
		regs = 0x86;
	pci_write_config8(MEMCTRL, 0xe8, regs);

	/* DRAM Timing MAB */
	pci_write_config8(MEMCTRL, 0xe9, 0x0);

	/* DRAM Timing DCLK VT8454C always 0x66 */
	pci_write_config8(MEMCTRL, 0xe6, 0xaa);

	/**********************************************/
	/*            Set DRAM Duty Control           */
	/**********************************************/
	regs = pci_read_config8(SCRATCH, SCRATCH_RANK_NUM);
	switch (regs) {
	case 1:
	case 2:		/* 1~2 rank */
		val = 0;
		break;
	case 3:
	case 4:		/* 3~4 rank */
		regs = pci_read_config8(SCRATCH, SCRATCH_DRAM_FREQ);
		if (regs == DDRII_533)
			val = 4;
		else		/* DDRII-400 */
			val = 0;
		break;
	}
	regs = 0xec;
	for (t = 0; t < 4; t++) {
		pci_write_config8(MEMCTRL, regs, Duty_Control_DDR2[val]);
		regs++;
		val++;
	}

	/**********************************************/
	/*            Set DRAM Clock Control          */
	/**********************************************/
	/* Write Data Phase */
	val = pci_read_config8(SCRATCH, SCRATCH_DRAM_FREQ);
	regs = pci_read_config8(MEMCTRL, 0x75);
	regs &= 0xf0;
	switch (val) {
	case DDRII_533:
		pci_write_config8(MEMCTRL, 0x74, 0x07);
		regs |= 0x7;
		break;
	case DDRII_400:
	default:
		pci_write_config8(MEMCTRL, 0x74, 0x05);
		regs |= 0x5;
		break;
	}
	pci_write_config8(MEMCTRL, 0x75, regs);
	pci_write_config8(MEMCTRL, 0x76, 0x80);

	/* Clock Phase Control for FeedBack Mode */
	regs = pci_read_config8(MEMCTRL, 0x90);
	pci_write_config8(MEMCTRL, 0x90, regs);

	regs = pci_read_config8(SCRATCH, SCRATCH_DRAM_FREQ);
	switch (regs) {
	case DDRII_533:
		regs = pci_read_config8(SCRATCH, SCRATCH_RANK_NUM);
		if (regs == 1)
			val = 0;
		else
			val = 3;
		break;
	case DDRII_400:
	default:
		val = 6;
		break;
	}
	regs = pci_read_config8(MEMCTRL, 0x91);
	regs &= ~0xc0;
	regs |= ctrl->cfg.mem_clocks << 6;
	pci_write_config8(MEMCTRL, 0x91, regs);
	regs = 0x91;
	for (t = 0; t < 3; t++) {
		dimm = pci_read_config8(MEMCTRL, regs);
		dimm &= ~0x7;
		dimm |= ChA_Clk_Phase_DDR2_Table[val];
		pci_write_config8(MEMCTRL, regs, dimm);
		regs++;
		val++;
	}

	pci_write_config8(MEMCTRL, 0x97, 0x12);
	pci_write_config8(MEMCTRL, 0x98, 0x33);

	regs = pci_read_config8(SCRATCH, SCRATCH_RANK_0);
	val = pci_read_config8(SCRATCH, SCRATCH_RANK_2);
	if (regs && val)
		pci_write_config8(MEMCTRL, 0x9d, 0x00);
	else
		pci_write_config8(MEMCTRL, 0x9d, 0x0f);

	for (val = 0; val < ARRAY_SIZE(DQ_DQS_Table); val += 2)
		pci_write_config8(MEMCTRL, DQ_DQS_Table[val], DQ_DQS_Table[val + 1]);
	regs = pci_read_config8(SCRATCH, SCRATCH_DRAM_FREQ);
	if (regs == DDRII_533)
		pci_write_config8(MEMCTRL, 0x7b, 0xa0);
	else
		pci_write_config8(MEMCTRL, 0x7b, 0x10);

	/***************************************************/
	/*  Set necessary register before DRAM initialize  */
	/***************************************************/
	for (val = 0; val < ARRAY_SIZE(Mem_Reg_Init); val += 3) {
		regs = pci_read_config8(MEMCTRL, Mem_Reg_Init[val]);
		regs &= Mem_Reg_Init[val + 1];
		regs |= Mem_Reg_Init[val + 2];
		pci_write_config8(MEMCTRL, Mem_Reg_Init[val], regs);
	}
	regs = pci_read_config8(HOSTCTRL, 0x51);
	regs &= 0xbf;		// Clear bit 6 Disable Read Around Write
	pci_write_config8(HOSTCTRL, 0x51, regs);

	regs = pci_read_config8(HOSTCTRL, 0x54);
	t = regs >> 5;
	val = pci_read_config8(HOSTCTRL, 0x57);
	dimm = val >> 5;
	if (t == dimm)
		t = 0x0;
	else
		t = 0x1;
	regs &= ~0x1;
	regs |= t;
	val &= ~0x1;
	val |= t;
	pci_write_config8(HOSTCTRL, 0x57, val);

	regs = pci_read_config8(HOSTCTRL, 0x51);
	regs |= t;
	pci_write_config8(HOSTCTRL, 0x51, regs);

	regs = pci_read_config8(MEMCTRL, 0x90);
	regs &= 0x7;
	val = 0;
	if (regs < 0x2)
		val = 0x80;
	regs = pci_read_config8(MEMCTRL, 0x76);
	regs &= 0x80;
	regs |= val;
	pci_write_config8(MEMCTRL, 0x76, regs);

	regs = pci_read_config8(MEMCTRL, 0x6f);
	regs |= 0x10;
	pci_write_config8(MEMCTRL, 0x6f, regs);

	/***************************************************/
	/*    Find suitable DQS value for ChA and ChB      */
	/***************************************************/
	// Set DQS output delay for Channel A
	regs = pci_read_config8(SCRATCH, SCRATCH_DRAM_FREQ);
	val = pci_read_config8(SCRATCH, SCRATCH_RANK_NUM);
	switch (regs) {
	case DDRII_533:
		if (val <= 2)
			val = 0;
		else
			val = 2;
		break;
	case DDRII_400:
	default:
		if (val <= 2)
			val = 4;
		else
			val = 6;
		break;
	}
	for (t = 0; t < 2; t++)
		pci_write_config8(MEMCTRL, (0x70 + t), DQSOChA_DDR2_Driving_Table[val + t]);
	// Set DQS output delay for Channel B
	pci_write_config8(MEMCTRL, 0x72, 0x0);

	regs = pci_read_config8(SCRATCH, SCRATCH_RANK_0);
	val = pci_read_config8(SCRATCH, SCRATCH_RANK_2);
	if (regs && val)
		pci_write_config8(MEMCTRL, 0x73, 0xfd);
	else
		pci_write_config8(MEMCTRL, 0x73, 0x01);
}

static void sdram_set_registers(const struct mem_controller *ctrl)
{
	c7_cpu_setup(ctrl);
	ddr_detect(ctrl);
	sdram_set_safe_values(ctrl);
}

static void step_20_21(const struct mem_controller *ctrl)
{
	u8 val;

	// Step 20
	udelay(200);

	val = pci_read_config8(SCRATCH, SCRATCH_DRAM_NB_ODT);
	if (val & DDR2_ODT_150ohm)
		read32p(0x102200);
	else
		read32p(0x102020);

	/* Step 21. Normal operation */
	printk(BIOS_SPEW, "RAM Enable 5: Normal operation\n");
	do_ram_command(ctrl, RAM_COMMAND_NORMAL);
	udelay(3);
}

static void step_2_19(const struct mem_controller *ctrl)
{
	u32 i;
	u8 val;

	//  Step 2
	val = pci_read_config8(MEMCTRL, 0x69);
	val &= ~0x03;
	pci_write_config8(MEMCTRL, 0x69, val);

	/* Step 3 Apply NOP. */
	printk(BIOS_SPEW, "RAM Enable 1: Apply NOP\n");
	do_ram_command(ctrl, RAM_COMMAND_NOP);

	udelay(15);

	// Step 4
	printk(BIOS_SPEW, "SEND: ");
	read32p(0);
	printk(BIOS_SPEW, "OK\n");

	// Step 5
	udelay(400);

	/* 6. Precharge all. Wait tRP. */
	printk(BIOS_SPEW, "RAM Enable 2: Precharge all\n");
	do_ram_command(ctrl, RAM_COMMAND_PRECHARGE);

	// Step 7
	printk(BIOS_SPEW, "SEND: ");
	read32p(0);
	printk(BIOS_SPEW, "OK\n");

	/* Step 8. Mode register set. */
	printk(BIOS_SPEW, "RAM Enable 4: Mode register set\n");
	do_ram_command(ctrl, RAM_COMMAND_MRS);	//enable dll

	// Step 9
	printk(BIOS_SPEW, "SEND: ");

	val = pci_read_config8(SCRATCH, SCRATCH_DRAM_NB_ODT);
	if (val & DDR2_ODT_150ohm)
		read32p(0x102200);	//DDR2_ODT_150ohm
	else
		read32p(0x102020);
	printk(BIOS_SPEW, "OK\n");

	// Step 10
	printk(BIOS_SPEW, "SEND: ");
	read32p(0x800);
	printk(BIOS_SPEW, "OK\n");

	/* Step 11. Precharge all. Wait tRP. */
	printk(BIOS_SPEW, "RAM Enable 2: Precharge all\n");
	do_ram_command(ctrl, RAM_COMMAND_PRECHARGE);

	// Step 12
	printk(BIOS_SPEW, "SEND: ");
	read32p(0);
	printk(BIOS_SPEW, "OK\n");

	/* Step 13. Perform 8 refresh cycles. Wait tRC each time. */
	printk(BIOS_SPEW, "RAM Enable 3: CBR\n");
	do_ram_command(ctrl, RAM_COMMAND_CBR);

	/* JEDEC says only twice, do 8 times for posterity */
	// Step 16: Repeat Step 14 and 15 another 7 times
	for (i = 0; i < 8; i++) {
		// Step 14
		read32p(0);
		printk(BIOS_SPEW, ".");

		// Step 15
		udelay(100);
	}

	/* Step 17. Mode register set. Wait 200us. */
	printk(BIOS_SPEW, "\nRAM Enable 4: Mode register set\n");

	//safe value for now, BL = 8, WR = 4, CAS = 4
	do_ram_command(ctrl, RAM_COMMAND_MRS);
	udelay(200);

	/* Use Single Chanel temporarily */
	val = pci_read_config8(MEMCTRL, 0x6c);
	if (val & 0x8) {	/* Burst Length = 8 */
		val = pci_read_config8(MEMCTRL, 0x62);
		val &= 0x7;
		i = DDR2_MRS_table[4 + val];
	} else {
		val = pci_read_config8(MEMCTRL, 0x62);
		val &= 0x7;
		i = DDR2_MRS_table[val];
	}

	// Step 18
	val = pci_read_config8(MEMCTRL, 0x61);
	val = val >> 6;
	i |= DDR2_Twr_table[val];
	read32p(i);

	printk(BIOS_DEBUG, "MRS = %08x\n", i);

	udelay(15);

	// Step 19
	val = pci_read_config8(SCRATCH, SCRATCH_DRAM_NB_ODT);
	if (val & DDR2_ODT_150ohm)
		read32p(0x103e00);	//EMRS OCD Default
	else
		read32p(0x103c20);
}

static void sdram_set_vr(const struct mem_controller *ctrl, u8 num)
{
	u8 reg, val;
	val = 0x54 + (num >> 1);
	reg = pci_read_config8(MEMCTRL, val);
	reg &= (0xf << (4 * (num & 0x1)));
	reg |= (((0x8 | num) << 4) >> (4 * (num & 0x1)));
	pci_write_config8(MEMCTRL, val, reg);
}
static void sdram_ending_addr(const struct mem_controller *ctrl, u8 num)
{
	u8 reg, val;
	/* Set Ending Address */
	val = 0x40 + num;
	reg = pci_read_config8(MEMCTRL, val);
	reg += 0x10;
	pci_write_config8(MEMCTRL, val, reg);
	/* Set Beginning Address */
	val = 0x48 + num;
	pci_write_config8(MEMCTRL, val, 0x0);
}

static void sdram_clear_vr_addr(const struct mem_controller *ctrl, u8 num)
{
	u8 reg, val;
	val = 0x54 + (num >> 1);
	reg = pci_read_config8(MEMCTRL, val);
	reg &= ~(0x80 >> (4 * (num & 0x1)));
	pci_write_config8(MEMCTRL, val, reg);
	val = 0x40 + num;
	reg = pci_read_config8(MEMCTRL, val);
	reg -= 0x10;
	pci_write_config8(MEMCTRL, val, reg);
	val = 0x48 + num;
	pci_write_config8(MEMCTRL, val, 0x0);
}

/* Perform sizing DRAM by dynamic method */
static void sdram_calc_size(const struct mem_controller *ctrl, u8 num)
{
	u8 ca, ra, ba, reg;
	ba = pci_read_config8(SCRATCH, SCRATCH_FLAGS);
	if (ba == 8) {
		write8p(0, 0x0d);
		ra = read8p(0);
		write8p(1 << SDRAM1X_RA_12_8bk, 0x0c);
		ra = read8p(0);

		write8p(0, 0x0a);
		ca = read8p(0);
		write8p(1 << SDRAM1X_CA_09_8bk, 0x0c);
		ca = read8p(0);

		write8p(0, 0x03);
		ba = read8p(0);
		write8p(1 << SDRAM1X_BA2_8bk, 0x02);
		ba = read8p(0);
		write8p(1 << SDRAM1X_BA1_8bk, 0x01);
		ba = read8p(0);
	} else {
		write8p(0, 0x0f);
		ra = read8p(0);
		write8p(1 << SDRAM1X_RA_14, 0x0e);
		ra = read8p(0);
		write8p(1 << SDRAM1X_RA_13, 0x0d);
		ra = read8p(0);
		write8p(1 << SDRAM1X_RA_12, 0x0c);
		ra = read8p(0);

		write8p(0, 0x0c);
		ca = read8p(0);
		write8p(1 << SDRAM1X_CA_12, 0x0b);
		ca = read8p(0);
		write8p(1 << SDRAM1X_CA_11, 0x0a);
		ca = read8p(0);
		write8p(1 << SDRAM1X_CA_09, 0x09);
		ca = read8p(0);

		write8p(0, 0x02);
		ba = read8p(0);
		write8p(1 << SDRAM1X_BA1, 0x01);
		ba = read8p(0);
	}

	if (ra < 10 || ra > 15)
		die("bad RA %u\n", ra);
	if (ca < 8 || ca > 12)
		die("bad CA %u\n", ca);
	if (ba < 1 || ba > 3)
		die("bad BA %u\n", ba);

	/* Calculate MA type save to scratch register */
	reg = 0;

	switch (ra) {
	case 12:
		reg |= MA_12_Row;
		break;
	case 13:
		reg |= MA_13_Row;
		break;
	case 14:
		reg |= MA_14_Row;
		break;
	default:
		reg |= MA_15_Row;
	}

	switch (ca) {
	case 9:
		reg |= MA_9_Column;
		break;
	case 10:
		reg |= MA_10_Column;
		break;
	case 11:
		reg |= MA_11_Column;
		break;
	default:
		reg |= MA_12_Column;
	}

	switch (ba) {
	case 3:
		reg |= MA_8_Bank;
		break;
	default:
		reg |= MA_4_Bank;
	}

	pci_write_config8(SCRATCH, SCRATCH_RANK0_MA_REG + num, reg);

	if (ra >= 13)
		pci_write_config8(SCRATCH, SCRATCH_DRAM_256M_BIT, 1);

	/* Calculate rank size save to scratch register */
	ra = ra + ca + ba + 3 - 26;	/* 1 unit = 64M */
	ra = 1 << ra;
	pci_write_config8(SCRATCH, SCRATCH_RANK0_SIZE_REG + num, ra);
}

static void enable_shadow_ram(const struct mem_controller *ctrl)
{
	u8 shadowreg;

	pci_write_config8(MEMCTRL, 0x80, 0x2a);

	/* 0xf0000-0xfffff - ACPI tables */
	shadowreg = pci_read_config8(MEMCTRL, 0x83);
	shadowreg |= 0x30;
	pci_write_config8(MEMCTRL, 0x83, shadowreg);
}

void sdram_enable(const struct dram_cfg *cfg)
{
	struct mem_controller ctrl = { *cfg, };
	u8 reg8;
	u8 val, i;
	u8 dl, dh;
	u32 quot;

	sdram_set_registers(&ctrl);
	enable_shadow_ram(&ctrl);

	/* Init Present Bank */
	for (i = 0; i < ARRAY_SIZE(Init_Rank_Reg_Table); i++)
		pci_write_config8(MEMCTRL, Init_Rank_Reg_Table[i], 0x0);

	/* Init other banks */
	for (i = 0; i < 4; i++) {
		reg8 = pci_read_config8(SCRATCH, SCRATCH_RANK_0 + i);
		if (reg8) {
			sdram_set_vr(&ctrl, i);
			sdram_ending_addr(&ctrl, i);
			step_2_19(&ctrl);
			step_20_21(&ctrl);
			sdram_clear_vr_addr(&ctrl, i);
		}
	}

	/****************************************************************/
	/* Find the DQSI Low/High bound and save it to Scratch register */
	/****************************************************************/
	for (dl = 0; dl < 0x3f; dl += 2) {
		reg8 = dl & 0x3f;
		reg8 |= 0x80;	/* Set Manual Mode */
		pci_write_config8(MEMCTRL, 0x77, reg8);
		for (i = 0; i < 4; i++) {
			reg8 = pci_read_config8(SCRATCH, SCRATCH_RANK_0 + i);
			if (reg8) {
				sdram_set_vr(&ctrl, i);
				sdram_ending_addr(&ctrl, i);
				write32p(0, 0x55555555);
				write32p(4, 0x55555555);
				udelay(15);
				if (read32p(0) != 0x55555555)
					break;
				if (read32p(4) != 0x55555555)
					break;
				write32p(0, 0xaaaaaaaa);
				write32p(4, 0xaaaaaaaa);
				udelay(15);
				if (read32p(0) != 0xaaaaaaaa)
					break;
				if (read32p(4) != 0xaaaaaaaa)
					break;
				sdram_clear_vr_addr(&ctrl, i);
			}
		}
		if (i == 4)
			break;
		else
			sdram_clear_vr_addr(&ctrl, i);
	}
	printk(BIOS_DEBUG, "DQSI Low 0x%02x\n", dl);
	for (dh = dl; dh < 0x3f; dh += 2) {
		reg8 = dh & 0x3f;
		reg8 |= 0x80;	/* Set Manual Mode */
		pci_write_config8(MEMCTRL, 0x77, reg8);
		for (i = 0; i < 4; i++) {
			reg8 = pci_read_config8(SCRATCH, SCRATCH_RANK_0 + i);
			if (reg8) {
				sdram_set_vr(&ctrl, i);
				sdram_ending_addr(&ctrl, i);

				write32p(0, 0x55555555);
				write32p(4, 0x55555555);
				udelay(15);
				if (read32p(0) != 0x55555555)
					break;
				if (read32p(4) != 0x55555555)
					break;
				write32p(0, 0xaaaaaaaa);
				write32p(4, 0xaaaaaaaa);
				udelay(15);
				if (read32p(0) != 0xaaaaaaaa)
					break;
				if (read32p(4) != 0xaaaaaaaa)
					break;
				sdram_clear_vr_addr(&ctrl, i);
			}
		}
		if (i != 4) {
			sdram_clear_vr_addr(&ctrl, i);
			break;
		}
	}
	printk(BIOS_DEBUG, "DQSI High 0x%02x\n", dh);
	pci_write_config8(SCRATCH, SCRATCH_CHA_DQSI_LOW_REG, dl);
	pci_write_config8(SCRATCH, SCRATCH_CHA_DQSI_HIGH_REG, dh);
	reg8 = pci_read_config8(MEMCTRL, 0X90) & 0X7;
	val = DQSI_Rate_Table[reg8];
	quot = dh - dl;
	quot = quot * val;
	quot >>= 4;
	val = quot + dl;
	pci_write_config8(SCRATCH, SCRATCH_ChA_DQSI_REG, val);
	reg8 = val & 0x3f;
	reg8 |= 0x80;
	pci_write_config8(MEMCTRL, 0x77, reg8);

	/****************************************************************/
	/*     Find out the lowest Bank Interleave and Set Register     */
	/****************************************************************/
	dl = 2;
	for (i = 0; i < 4; i++) {
		reg8 = pci_read_config8(SCRATCH, SCRATCH_RANK_0 + i);
		if (reg8) {
			reg8 = get_spd_data(&ctrl.cfg, (i >> 1), 17);
			sdram_set_vr(&ctrl, i);
			sdram_ending_addr(&ctrl, i);
			if (reg8 == 4) {
				write8p(0, 0x02);
				val = read8p(0);
				write8p(1 << SDRAM1X_BA1, 0x01);
				val = read8p(0);
			} else {
				write8p(0, 0x03);
				val = read8p(0);
				write8p(1 << SDRAM1X_BA2_8bk, 0x02);
				val = read8p(0);
				write8p(1 << SDRAM1X_BA1_8bk, 0x01);
				val = read8p(0);
			}
			if (val < dl)
				dl = val;
			sdram_clear_vr_addr(&ctrl, i);
		}
	}
	dl <<= 6;
	reg8 = pci_read_config8(MEMCTRL, 0x69);
	reg8 &= ~0xc0;
	reg8 |= dl;
	pci_write_config8(MEMCTRL, 0x69, reg8);

	/****************************************************************/
	/*               DRAM Sizing and Fill MA type                   */
	/****************************************************************/
	for (i = 0; i < 4; i++) {
		val = pci_read_config8(SCRATCH, SCRATCH_RANK_0 + i);
		if (val) {
			reg8 = get_spd_data(&ctrl.cfg, (i >> 1), 17);
			pci_write_config8(SCRATCH, SCRATCH_FLAGS, reg8);
			if (reg8 == 4) {
				/* Use MA Type 3 for DRAM sizing */
				reg8 = pci_read_config8(MEMCTRL, 0x50);
				reg8 &= 0x11;
				reg8 |= 0x66;
				pci_write_config8(MEMCTRL, 0x50, reg8);
				pci_write_config8(MEMCTRL, 0x51, reg8);
			} else {
				/* Use MA Type 5 for DRAM sizing */
				reg8 = pci_read_config8(MEMCTRL, 0x50);
				reg8 &= 0x11;
				reg8 |= 0xaa;
				pci_write_config8(MEMCTRL, 0x50, reg8);
				pci_write_config8(MEMCTRL, 0x51, reg8);
				reg8 = pci_read_config8(MEMCTRL, 0x53);
				reg8 &= 0x0f;
				reg8 |= 0x90;
				pci_write_config8(MEMCTRL, 0x53, reg8);
			}
			sdram_set_vr(&ctrl, i);
			val = 0x40 + i;
			reg8 = pci_read_config8(MEMCTRL, val);
			/* max size 3G for new MA table */
			reg8 += 0x30;
			pci_write_config8(MEMCTRL, val, reg8);
			/* Set Beginning Address */
			val = 0x48 + i;
			pci_write_config8(MEMCTRL, val, 0x0);

			sdram_calc_size(&ctrl, i);

			/* Clear */
			val = 0x54 + (i >> 1);
			reg8 = pci_read_config8(MEMCTRL, val);
			reg8 = ~(0x80 >> (4 * (i & 0x1)));
			pci_write_config8(MEMCTRL, val, reg8);
			val = 0x40 + i;
			reg8 = pci_read_config8(MEMCTRL, val);
			reg8 -= 0x30;
			pci_write_config8(MEMCTRL, val, reg8);
			val = 0x48 + i;
			pci_write_config8(MEMCTRL, val, 0x0);

		}
	}
	/* Clear MA Type */
	reg8 = pci_read_config8(MEMCTRL, 0x50);
	reg8 &= 0x11;
	pci_write_config8(MEMCTRL, 0x50, reg8);
	pci_write_config8(MEMCTRL, 0x51, reg8);
	reg8 = pci_read_config8(MEMCTRL, 0x6b);
	reg8 &= ~0x08;
	pci_write_config8(MEMCTRL, 0x6b, reg8);

	/****************************************************************/
	/*             DRAM re-initialize for burst length              */
	/****************************************************************/
	for (i = 0; i < 4; i++) {
		reg8 = pci_read_config8(SCRATCH, SCRATCH_RANK_0 + i);
		if (reg8) {
			sdram_set_vr(&ctrl, i);
			sdram_ending_addr(&ctrl, i);
			step_2_19(&ctrl);
			step_20_21(&ctrl);
			sdram_clear_vr_addr(&ctrl, i);
		}
	}

	/****************************************************************/
	/*                    Set the MA Type                           */
	/****************************************************************/
	reg8 = pci_read_config8(MEMCTRL, 0x50);
	reg8 &= 0x11;
	pci_write_config8(MEMCTRL, 0x50, reg8);

	reg8 = pci_read_config8(MEMCTRL, 0x51);
	reg8 &= 0x11;
	pci_write_config8(MEMCTRL, 0x51, reg8);

	reg8 = pci_read_config8(MEMCTRL, 0x6b);
	reg8 &= ~0x08;
	pci_write_config8(MEMCTRL, 0x6b, reg8);

	for (i = 0; i < 4; i += 2) {
		reg8 = pci_read_config8(SCRATCH, SCRATCH_RANK_0 + i);
		if (reg8) {
			reg8 = pci_read_config8(SCRATCH, SCRATCH_RANK0_MA_REG + i);
			reg8 &= (MA_Bank + MA_Column);
			val = pci_read_config8(MEMCTRL, 0x50);
			if (i == 0) {
				reg8 <<= 4;
				val &= 0x1f;
			} else
				val &= 0xf1;
			val |= reg8;
			pci_write_config8(MEMCTRL, 0x50, val);
		}
	}

	/****************************************************************/
	/*                 Set Start and Ending Address                 */
	/****************************************************************/
	dl = 0;			/* Begin Address */
	dh = 0;			/* Ending Address */
	for (i = 0; i < 4; i++) {
		reg8 = pci_read_config8(SCRATCH, SCRATCH_RANK_0 + i);
		if (reg8) {
			reg8 = pci_read_config8(SCRATCH, SCRATCH_RANK0_SIZE_REG + i);
			if (reg8 == 0)
				continue;
			dh += reg8;
			pci_write_config8(MEMCTRL, (0x40 + i), dh);
			pci_write_config8(MEMCTRL, (0x48 + i), dl);
			dl = dh;
		}
	}
	dh <<= 2;
	// F7_Rx57 Ending address mirror register
	pci_write_config8(_sdev_north_end, 0x57, dh);
	pci_write_config8(_sdev_south_end, 0x57, dh);
	// LOW TOP Address
	pci_write_config8(MEMCTRL, 0x88, dh);
	pci_write_config8(MEMCTRL, 0x85, dh);
	// also program vlink mirror
	pci_write_config8(_sdev_north_end, 0xe5, dh);

	/****************************************************************/
	/*            Set Physical to Virtual Rank mapping              */
	/****************************************************************/
	pci_write_config32(MEMCTRL, 0x54, 0x0);
	for (i = 0; i < 4; i++) {
		reg8 = pci_read_config8(SCRATCH, SCRATCH_RANK_0 + i);
		if (reg8) {
			reg8 = pci_read_config8(MEMCTRL, (0x54 + (i >> 1)));
			if (i & 0x1) {	/* Odd Rank */
				reg8 &= 0xf0;
				reg8 |= (0x8 | i);
			} else {	/* Even Rank */

				reg8 &= 0x0f;
				reg8 |= ((0x8 | i) << 4);
			}
			pci_write_config8(MEMCTRL, (0x54 + (i >> 1)), reg8);
		}
	}

	/****************************************************************/
	/*                   Set DRAM Refresh Counter                   */
	/****************************************************************/
	val = pci_read_config8(MEMCTRL, 0X90) & 0X7;
	val <<= 1;
	reg8 = pci_read_config8(SCRATCH, SCRATCH_DRAM_256M_BIT);
	if (reg8)
		val++;
	pci_write_config8(MEMCTRL, 0x6a, REFC_Table[val]);

	/****************************************************************/
	/*  Chipset Performance UP and other setting after DRAM Sizing  */
	/****************************************************************/
	/* Dram Registers */
	for (i = 0; i < ARRAY_SIZE(Dram_Table); i += 3) {
		reg8 = pci_read_config8(MEMCTRL, Dram_Table[i]);
		reg8 &= Dram_Table[i + 1];
		reg8 |= Dram_Table[i + 2];
		pci_write_config8(MEMCTRL, Dram_Table[i], reg8);
	}

	/* Host Registers */
	for (i = 0; i < ARRAY_SIZE(Host_Table); i += 3) {
		reg8 = pci_read_config8(HOSTCTRL, Host_Table[i]);
		reg8 &= Host_Table[i + 1];
		reg8 |= Host_Table[i + 2];
		pci_write_config8(HOSTCTRL, Host_Table[i], reg8);
	}

	pci_write_config8(HOSTCTRL, 0x5d, 0xb2);

	/****************************************************************/
	/*              UMA registers for N-series projects             */
	/****************************************************************/

	/* Manual setting frame buffer bank */
	for (i = 0; i < 4; i++) {
		reg8 = pci_read_config8(SCRATCH, SCRATCH_RANK_0 + i);
		if (reg8)
			val = i;
	}
	pci_write_config8(MEMCTRL, 0xb0, val);
	reg8 = 0x40;		// Frame buffer size 64M
	reg8 |= 0x80;		// VGA Enable
	reg8 |= 0x0a;		// A[31:28] = 1010b
	pci_write_config8(MEMCTRL, 0xa1, reg8);

	static const struct regmask {
		u8 reg;
		u8 mask;
		u8 val;
	} b0d1f0[] = {
		{ 0x40, 0x00, 0x8b},
		{ 0x41, 0x80, 0x43},
		{ 0x42, 0x00, 0x62},
		{ 0x43, 0x00, 0x44},
		{ 0x44, 0x00, 0x34},
		{ 0x45, 0x00, 0x72}
	}, b0d0f3[] = {
		{ 0x53, 0xf0, 0x0f},
		{ 0x60, 0x00, 0x03},
		{ 0x65, 0x00, 0xd9},
		{ 0x66, 0x00, 0x80},
		{ 0x67, 0x00, 0x00},
		{ 0x68, 0x00, 0x01},
		{ 0x69, 0xe0, 0x03},
		{ 0x6b, 0x00, 0x10},
		{ 0x6c, 0xc1, 0x08},
		{ 0x6e, 0x00, 0x89},
		{ 0x6f, 0x00, 0x51},
		{ 0x75, ~0x40, 0x40},
		{ 0x76, 0x8f, 0x00},
		{ 0x7b, 0x00, 0xa0},
		{ 0x86, 0x01, 0x24},
		{ 0x86, 0x04, 0x29},
		{ 0x8c, 0x00, 0x00},
		{ 0x8d, 0x00, 0x00},
		{ 0x95, ~0x40, 0x00},
		{ 0xa2, 0x00, 0x44},
		{ 0xb1, 0x00, 0xaa}
	}, b0d0f0[] = {
		{ 0x4d, 0x00, 0x24},
		{ 0x4f, 0x00, 0x01},
		{ 0xbc, 0x00, 0x21},
		{ 0xbe, 0x00, 0x00},
		{ 0xbf, 0x7f, 0x80}
	}, b0d17f0[] = {
		{ 0x40, ~0x01, 0x01},		// enable timer/counter shadow registers
		{ 0x67, ~0x03, 0x01},
		{ 0x5b, ~0x01, 0x00},
		{ 0x8d, ~0x02, 0x02},
		{ 0x97, 0x7f, 0x00},
		{ 0xd2, ~0x18, 0x00},
		{ 0xe2, ~0x36, 0x06},
		{ 0xe4, 0x7f, 0x00},
		{ 0xe5, 0x00, 0x40},
		{ 0xe6, 0x00, 0x20},
		{ 0xe7, 0x2f, 0xc0},
		{ 0xec, ~0x08, 0x00}
	}, b0d17f7[] = {
		{ 0x4e, 0x7f, 0x80},
		{ 0x4f, ~(1 << 6), 1 << 6 },	/* PG_CX700: 14.1.1 enable P2P Bridge Header for External PCI Bus */
		{ 0x74, ~0x00, 0x04},		/* PG_CX700: 14.1.2 APIC FSB directly up to snmic, not on pci */
		{ 0x7c, ~0x00, 0x02},		/* PG_CX700: 14.1.1 APIC FSB directly up to snmic, not on pci */
		{ 0xe6, 0x0, 0x04}		// MSI post
	}, b0d19f0[] = {	/* P2PE */
		{ 0x42, ~0x08, 0x08},		// Disable HD Audio,
		{ 0x40, 0x3f, 0x80}		// 14.1.3.1.1 of the PG: extended cfg mode for pcie. enable capability, but don't activate
	}, b0d0f2[] = {
		{ 0x50, ~0x40, 0x88},
		{ 0x51, 0x80, 0x7b},
		{ 0x52, 0x90, 0x6f},
		{ 0x53, 0x00, 0x88},
		{ 0x54, 0xe4, 0x16},
		{ 0x55, 0xf2, 0x04},
		{ 0x56, 0x0f, 0x00},
		{ 0x57, ~0x04, 0x00},
		{ 0x5d, 0x00, 0xb2},
		{ 0x5e, 0x00, 0x88},
		{ 0x5f, 0x00, 0xc7},
		{ 0x5c, 0x00, 0x01}
	};

	REGISTERPRESET(0, 0, 0, b0d0f0);
	REGISTERPRESET(0, 0, 2, b0d0f2);
	REGISTERPRESET(0, 0, 3, b0d0f3);
	REGISTERPRESET(0, 1, 0, b0d1f0);
	REGISTERPRESET(0, 17, 0, b0d17f0);
	REGISTERPRESET(0, 17, 7, b0d17f7);
	REGISTERPRESET(0, 19, 0, b0d19f0);
}
