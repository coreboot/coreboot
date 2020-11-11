/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_ROCKCHIP_RK3399_SDRAM_H__
#define __SOC_ROCKCHIP_RK3399_SDRAM_H__

#include <types.h>

enum {
	DDR3 = 3,
	LPDDR2 = 5,
	LPDDR3 = 6,
	LPDDR4 = 7,
	UNUSED = 0xFF
};

struct rk3399_ddr_pctl_regs {
	u32 denali_ctl[332];
};

struct rk3399_ddr_publ_regs {
	u32 denali_phy[959];
};

struct rk3399_ddr_pi_regs {
	u32 denali_pi[200];
};
union noc_ddrtiminga0 {
	u32 d32;
	struct {
		unsigned int acttoact : 6;
		unsigned int reserved0 : 2;
		unsigned int rdtomiss : 6;
		unsigned int reserved1 : 2;
		unsigned int wrtomiss : 6;
		unsigned int reserved2 : 2;
		unsigned int readlatency : 8;
	} b;
};

union noc_ddrtimingb0 {
	u32 d32;
	struct {
		unsigned int rdtowr : 5;
		unsigned int reserved0 : 3;
		unsigned int wrtord : 5;
		unsigned int reserved1 : 3;
		unsigned int rrd : 4;
		unsigned int reserved2 : 4;
		unsigned int faw : 6;
		unsigned int reserved3 : 2;
	} b;
};

union noc_ddrtimingc0 {
	u32 d32;
	struct {
		unsigned int burstpenalty : 4;
		unsigned int reserved0 : 4;
		unsigned int wrtomwr : 6;
		unsigned int reserved1 : 18;
	} b;
};

union noc_devtodev0 {
	u32 d32;
	struct {
		unsigned int busrdtord : 3;
		unsigned int reserved0 : 1;
		unsigned int busrdtowr : 3;
		unsigned int reserved1 : 1;
		unsigned int buswrtord : 3;
		unsigned int reserved2 : 1;
		unsigned int buswrtowr : 3;
		unsigned int reserved3 : 17;
	} b;
};

union noc_ddrmode {
	u32 d32;
	struct {
		unsigned int autoprecharge : 1;
		unsigned int bypassfiltering : 1;
		unsigned int fawbank : 1;
		unsigned int burstsize : 2;
		unsigned int mwrsize : 2;
		unsigned int reserved2 : 1;
		unsigned int forceorder : 8;
		unsigned int forceorderstate : 8;
		unsigned int reserved3 : 8;
	} b;
};

struct rk3399_msch_regs {
	u32 coreid;
	u32 revisionid;
	u32 ddrconf;
	u32 ddrsize;
	union noc_ddrtiminga0 ddrtiminga0;
	union noc_ddrtimingb0 ddrtimingb0;
	union noc_ddrtimingc0 ddrtimingc0;
	union noc_devtodev0 devtodev0;
	u32 reserved0[(0x110-0x20)/4];
	union noc_ddrmode ddrmode;
	u32 reserved1[(0x1000-0x114)/4];
	u32 agingx0;
};

struct rk3399_msch_timings {
	union noc_ddrtiminga0 ddrtiminga0;
	union noc_ddrtimingb0 ddrtimingb0;
	union noc_ddrtimingc0 ddrtimingc0;
	union noc_devtodev0 devtodev0;
	union noc_ddrmode ddrmode;
	u32 agingx0;
};

struct rk3399_ddr_cic_regs {
	u32 cic_ctrl0;
	u32 cic_ctrl1;
	u32 cic_idle_th;
	u32 cic_cg_wait_th;
	u32 cic_status0;
	u32 cic_status1;
	u32 cic_ctrl2;
	u32 cic_ctrl3;
	u32 cic_ctrl4;
};

/* DENALI_CTL_00 */
#define START		(1)

/* DENALI_CTL_68 */
#define PWRUP_SREFRESH_EXIT	(1 << 16)

/* DENALI_CTL_274 */
#define MEM_RST_VALID	(1)

struct rk3399_sdram_channel {
	unsigned char rank;
	/* col = 0, means this channel is invalid */
	unsigned char col;
	/* 3:8bank, 2:4bank */
	unsigned char bk;
	/* channel buswidth, 2:32bit, 1:16bit, 0:8bit */
	unsigned char bw;
	/* die buswidth, 2:32bit, 1:16bit, 0:8bit */
	unsigned char dbw;
	/* row_3_4 = 1: 6Gb or 12Gb die
	 * row_3_4 = 0: normal die, power of 2
	 */
	unsigned char row_3_4;
	unsigned char cs0_row;
	unsigned char cs1_row;
	unsigned int ddrconfig;
	struct rk3399_msch_timings noc_timings;
};

struct rk3399_sdram_params {
	struct rk3399_sdram_channel ch[2];
	unsigned int ddr_freq;
	unsigned char dramtype;
	unsigned char num_channels;
	unsigned char stride;
	unsigned char odt;
	/* align 8 byte */
	struct rk3399_ddr_pctl_regs pctl_regs;
	/* align 8 byte */
	struct rk3399_ddr_pi_regs pi_regs;
	/* align 8 byte */
	struct rk3399_ddr_publ_regs phy_regs;
	/* used for align 8byte for next struct */
	unsigned int align_8;
};

#define PI_CA_TRAINING	(1 << 0)
#define PI_WRITE_LEVELING	(1 << 1)
#define PI_READ_GATE_TRAINING	(1 << 2)
#define PI_READ_LEVELING	(1 << 3)
#define PI_WDQ_LEVELING	(1 << 4)
#define PI_FULL_TRAINING	(0xff)

void sdram_init(const struct rk3399_sdram_params *sdram_params);
u32 sdram_get_ram_code(void);
const struct rk3399_sdram_params *get_sdram_config(void);
size_t sdram_size_mb(void);

#endif
