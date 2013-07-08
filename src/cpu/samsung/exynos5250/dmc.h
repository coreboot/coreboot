/*
 * This file is part of the coreboot project.
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

#ifndef CPU_SAMSUNG_EXYNOS5250_DMC_H
#define CPU_SAMSUNG_EXYNOS5250_DMC_H

#ifndef __ASSEMBLER__
struct exynos5_dmc {
	unsigned int concontrol;
	unsigned int memcontrol;
	unsigned int memconfig0;
	unsigned int memconfig1;
	unsigned int directcmd;
	unsigned int prechconfig;
	unsigned int phycontrol0;
	unsigned char res1[0xc];
	unsigned int pwrdnconfig;
	unsigned int timingpzq;
	unsigned int timingref;
	unsigned int timingrow;
	unsigned int timingdata;
	unsigned int timingpower;
	unsigned int phystatus;
	unsigned char res2[0x4];
	unsigned int chipstatus_ch0;
	unsigned int chipstatus_ch1;
	unsigned char res3[0x4];
	unsigned int mrstatus;
	unsigned char res4[0x8];
	unsigned int qoscontrol0;
	unsigned char resr5[0x4];
	unsigned int qoscontrol1;
	unsigned char res6[0x4];
	unsigned int qoscontrol2;
	unsigned char res7[0x4];
	unsigned int qoscontrol3;
	unsigned char res8[0x4];
	unsigned int qoscontrol4;
	unsigned char res9[0x4];
	unsigned int qoscontrol5;
	unsigned char res10[0x4];
	unsigned int qoscontrol6;
	unsigned char res11[0x4];
	unsigned int qoscontrol7;
	unsigned char res12[0x4];
	unsigned int qoscontrol8;
	unsigned char res13[0x4];
	unsigned int qoscontrol9;
	unsigned char res14[0x4];
	unsigned int qoscontrol10;
	unsigned char res15[0x4];
	unsigned int qoscontrol11;
	unsigned char res16[0x4];
	unsigned int qoscontrol12;
	unsigned char res17[0x4];
	unsigned int qoscontrol13;
	unsigned char res18[0x4];
	unsigned int qoscontrol14;
	unsigned char res19[0x4];
	unsigned int qoscontrol15;
	unsigned char res20[0x14];
	unsigned int ivcontrol;
	unsigned int wrtra_config;
	unsigned int rdlvl_config;
	unsigned char res21[0x8];
	unsigned int brbrsvconfig;
	unsigned int brbqosconfig;
	unsigned int membaseconfig0;
	unsigned int membaseconfig1;
	unsigned char res22[0xc];
	unsigned int wrlvl_config;
	unsigned char res23[0xc];
	unsigned int perevcontrol;
	unsigned int perev0config;
	unsigned int perev1config;
	unsigned int perev2config;
	unsigned int perev3config;
	unsigned char res24[0xdebc];
	unsigned int pmnc_ppc_a;
	unsigned char res25[0xc];
	unsigned int cntens_ppc_a;
	unsigned char res26[0xc];
	unsigned int cntenc_ppc_a;
	unsigned char res27[0xc];
	unsigned int intens_ppc_a;
	unsigned char res28[0xc];
	unsigned int intenc_ppc_a;
	unsigned char res29[0xc];
	unsigned int flag_ppc_a;
	unsigned char res30[0xac];
	unsigned int ccnt_ppc_a;
	unsigned char res31[0xc];
	unsigned int pmcnt0_ppc_a;
	unsigned char res32[0xc];
	unsigned int pmcnt1_ppc_a;
	unsigned char res33[0xc];
	unsigned int pmcnt2_ppc_a;
	unsigned char res34[0xc];
	unsigned int pmcnt3_ppc_a;
};

struct exynos5_phy_control {
	unsigned int phy_con0;
	unsigned int phy_con1;
	unsigned int phy_con2;
	unsigned int phy_con3;
	unsigned int phy_con4;
	unsigned char res1[4];
	unsigned int phy_con6;
	unsigned char res2[4];
	unsigned int phy_con8;
	unsigned int phy_con9;
	unsigned int phy_con10;
	unsigned char res3[4];
	unsigned int phy_con12;
	unsigned int phy_con13;
	unsigned int phy_con14;
	unsigned int phy_con15;
	unsigned int phy_con16;
	unsigned char res4[4];
	unsigned int phy_con17;
	unsigned int phy_con18;
	unsigned int phy_con19;
	unsigned int phy_con20;
	unsigned int phy_con21;
	unsigned int phy_con22;
	unsigned int phy_con23;
	unsigned int phy_con24;
	unsigned int phy_con25;
	unsigned int phy_con26;
	unsigned int phy_con27;
	unsigned int phy_con28;
	unsigned int phy_con29;
	unsigned int phy_con30;
	unsigned int phy_con31;
	unsigned int phy_con32;
	unsigned int phy_con33;
	unsigned int phy_con34;
	unsigned int phy_con35;
	unsigned int phy_con36;
	unsigned int phy_con37;
	unsigned int phy_con38;
	unsigned int phy_con39;
	unsigned int phy_con40;
	unsigned int phy_con41;
	unsigned int phy_con42;
};

enum ddr_mode {
	DDR_MODE_DDR2,
	DDR_MODE_DDR3,
	DDR_MODE_LPDDR2,
	DDR_MODE_LPDDR3,

	DDR_MODE_COUNT,
};

/* For reasons unknown, people are in the habit of taking a 32-bit
 * field with 2 possible values and packing it with, say, 2 bits. A
 * non-robust encoding, using only 2 bits of a 32-bit field, is
 * incredibly difficult to deal with when things go wrong, because
 * there are a lot of things that get expressed as 0, 1, or 2. If
 * you're scanning with jtag or dumping memory it is really hard to
 * tell when you've hit the beginning of the struct. So, let's be a
 * bit smart here. First, while it's common to let the enum count
 * entries for you, when there are two of them, we can do the
 * counting. And, let's set the values to something we can easily scan
 * for in memory. Since '1' and '2' are rather common, we pick
 * something that's actually of some value when things go wrong.  This
 * setup motivated by a use case: something's going wrong and having a
 * manuf name of '1' or '2' is completely useless!
 */
enum mem_manuf {
	MEM_MANUF_AUTODETECT,
	MEM_MANUF_ELPIDA = 0xe7b1da,
	MEM_MANUF_SAMSUNG = 0x5a5096,

	MEM_MANUF_COUNT = 2, // fancy that.
};

enum {
	MEM_TIMINGS_MSR_COUNT	= 4,
};

#define DMC_INTERLEAVE_SIZE		0x1f

/* CONCONTROL register fields */
#define CONCONTROL_DFI_INIT_START_SHIFT	28
#define CONCONTROL_RD_FETCH_SHIFT	12
#define CONCONTROL_RD_FETCH_MASK	(0x7 << CONCONTROL_RD_FETCH_SHIFT)
#define CONCONTROL_AREF_EN_SHIFT	5

/* PRECHCONFIG register field */
#define PRECHCONFIG_TP_CNT_SHIFT	24

/* PWRDNCONFIG register field */
#define PWRDNCONFIG_DPWRDN_CYC_SHIFT	0
#define PWRDNCONFIG_DSREF_CYC_SHIFT	16

/* PHY_CON0 register fields */
#define PHY_CON0_T_WRRDCMD_SHIFT	17
#define PHY_CON0_T_WRRDCMD_MASK		(0x7 << PHY_CON0_T_WRRDCMD_SHIFT)
#define PHY_CON0_CTRL_DDR_MODE_SHIFT	11

/* PHY_CON1 register fields */
#define PHY_CON1_RDLVL_RDDATA_ADJ_SHIFT	0

/* PHY_CON12 register fields */
#define PHY_CON12_CTRL_START_POINT_SHIFT	24
#define PHY_CON12_CTRL_INC_SHIFT	16
#define PHY_CON12_CTRL_FORCE_SHIFT	8
#define PHY_CON12_CTRL_START_SHIFT	6
#define PHY_CON12_CTRL_START_MASK	(1 << PHY_CON12_CTRL_START_SHIFT)
#define PHY_CON12_CTRL_DLL_ON_SHIFT	5
#define PHY_CON12_CTRL_DLL_ON_MASK	(1 << PHY_CON12_CTRL_DLL_ON_SHIFT)
#define PHY_CON12_CTRL_REF_SHIFT	1

/* PHY_CON16 register fields */
#define PHY_CON16_ZQ_MODE_DDS_SHIFT	24
#define PHY_CON16_ZQ_MODE_DDS_MASK	(0x7 << PHY_CON16_ZQ_MODE_DDS_SHIFT)

#define PHY_CON16_ZQ_MODE_TERM_SHIFT 21
#define PHY_CON16_ZQ_MODE_TERM_MASK	(0x7 << PHY_CON16_ZQ_MODE_TERM_SHIFT)

#define PHY_CON16_ZQ_MODE_NOTERM_MASK	(1 << 19)

/* PHY_CON42 register fields */
#define PHY_CON42_CTRL_BSTLEN_SHIFT	8
#define PHY_CON42_CTRL_BSTLEN_MASK	(0xff << PHY_CON42_CTRL_BSTLEN_SHIFT)

#define PHY_CON42_CTRL_RDLAT_SHIFT	0
#define PHY_CON42_CTRL_RDLAT_MASK	(0x1f << PHY_CON42_CTRL_RDLAT_SHIFT)

/* These are the memory timings for a particular memory type and speed */
struct mem_timings {
	enum mem_manuf mem_manuf;	/* Memory manufacturer */
	enum ddr_mode mem_type;		/* Memory type */
	unsigned int frequency_mhz;	/* Frequency of memory in MHz */

	/* Here follow the timing parameters for the selected memory */
	uint8_t apll_mdiv;
	uint8_t apll_pdiv;
	uint8_t apll_sdiv;
	uint8_t mpll_mdiv;
	uint8_t mpll_pdiv;
	uint8_t mpll_sdiv;
	uint8_t cpll_mdiv;
	uint8_t cpll_pdiv;
	uint8_t cpll_sdiv;
	uint8_t gpll_pdiv;
	uint16_t gpll_mdiv;
	uint8_t gpll_sdiv;
	uint8_t epll_mdiv;
	uint8_t epll_pdiv;
	uint8_t epll_sdiv;
	uint8_t vpll_mdiv;
	uint8_t vpll_pdiv;
	uint8_t vpll_sdiv;
	uint8_t bpll_mdiv;
	uint8_t bpll_pdiv;
	uint8_t bpll_sdiv;
	uint8_t use_bpll;       /* 1 to use BPLL for cdrex, 0 to use MPLL */
	uint8_t pclk_cdrex_ratio;
	unsigned int direct_cmd_msr[MEM_TIMINGS_MSR_COUNT];

	unsigned int timing_ref;
	unsigned int timing_row;
	unsigned int timing_data;
	unsigned int timing_power;

	/* DQS, DQ, DEBUG offsets */
	unsigned int phy0_dqs;
	unsigned int phy1_dqs;
	unsigned int phy0_dq;
	unsigned int phy1_dq;
	uint8_t phy0_tFS;
	uint8_t phy1_tFS;
	uint8_t phy0_pulld_dqs;
	uint8_t phy1_pulld_dqs;

	uint8_t lpddr3_ctrl_phy_reset;
	uint8_t ctrl_start_point;
	uint8_t ctrl_inc;
	uint8_t ctrl_start;
	uint8_t ctrl_dll_on;
	uint8_t ctrl_ref;

	uint8_t ctrl_force;
	uint8_t ctrl_rdlat;
	uint8_t ctrl_bstlen;

	uint8_t fp_resync;
	uint8_t iv_size;
	uint8_t dfi_init_start;
	uint8_t aref_en;

	uint8_t rd_fetch;

	uint8_t zq_mode_dds;
	uint8_t zq_mode_term;
	uint8_t zq_mode_noterm;	/* 1 to allow termination disable */

	unsigned int memcontrol;
	unsigned int memconfig;

	unsigned int membaseconfig0;
	unsigned int membaseconfig1;
	unsigned int prechconfig_tp_cnt;
	unsigned int dpwrdn_cyc;
	unsigned int dsref_cyc;
	unsigned int concontrol;
	/* Channel and Chip Selection */
	uint8_t dmc_channels;		/* number of memory channels */
	uint8_t chips_per_channel;	/* number of chips per channel */
	uint8_t chips_to_configure;	/* number of chips to configure */
	uint8_t send_zq_init;		/* 1 to send this command */
	unsigned int impedance;		/* drive strength impedance */
	uint8_t gate_leveling_enable;	/* check gate leveling is enabled */
};

/**
 * Get the correct memory timings for our selected memory type and speed.
 *
 * @return pointer to the memory timings that we should use
 */
struct mem_timings *get_mem_timings(void);

#endif
#endif
