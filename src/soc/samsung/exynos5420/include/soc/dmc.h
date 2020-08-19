/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_SAMSUNG_EXYNOS5420_DMC_H
#define CPU_SAMSUNG_EXYNOS5420_DMC_H

#define DMC_INTERLEAVE_SIZE		0x1f

#define PAD_RETENTION_DRAM_COREBLK_VAL	0x10000000

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
#define PHY_CON0_CTRL_DDR_MODE_MASK	0x3
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

#ifndef __ASSEMBLER__

#include <soc/cpu.h>

struct exynos5_dmc {
	uint32_t concontrol;
	uint32_t memcontrol;
	uint32_t cgcontrol;
	uint32_t memconfig1;
	uint32_t directcmd;
	uint32_t prechconfig0;
	uint32_t phycontrol0;
	uint32_t prechconfig1;
	uint8_t res1[0x8];
	uint32_t pwrdnconfig;	/* 0x0028*/
	uint32_t timingpzq;
	uint32_t timingref;
	uint32_t timingrow;
	uint32_t timingdata;
	uint32_t timingpower;
	uint32_t phystatus;
	uint8_t res2[0x4];
	uint32_t chipstatus_ch0;	/* 0x0048 */
	uint32_t chipstatus_ch1;
	uint8_t res3[0x4];
	uint32_t mrstatus;
	uint8_t res4[0x8];
	uint32_t qoscontrol0;	/* 0x0060 */
	uint8_t resr5[0x4];
	uint32_t qoscontrol1;
	uint8_t res6[0x4];
	uint32_t qoscontrol2;
	uint8_t res7[0x4];
	uint32_t qoscontrol3;
	uint8_t res8[0x4];
	uint32_t qoscontrol4;
	uint8_t res9[0x4];
	uint32_t qoscontrol5;
	uint8_t res10[0x4];
	uint32_t qoscontrol6;
	uint8_t res11[0x4];
	uint32_t qoscontrol7;
	uint8_t res12[0x4];
	uint32_t qoscontrol8;
	uint8_t res13[0x4];
	uint32_t qoscontrol9;
	uint8_t res14[0x4];
	uint32_t qoscontrol10;
	uint8_t res15[0x4];
	uint32_t qoscontrol11;
	uint8_t res16[0x4];
	uint32_t qoscontrol12;
	uint8_t res17[0x4];
	uint32_t qoscontrol13;
	uint8_t res18[0x4];
	uint32_t qoscontrol14;
	uint8_t res19[0x4];
	uint32_t qoscontrol15;
	uint8_t res20[0x4];
	uint32_t timing_set_sw;	/* 0x00e0 */
	uint32_t timingrow1;
	uint32_t timingdata1;
	uint32_t timingpower1;
	uint32_t ivcontrol;
	uint32_t wrtra_config;
	uint32_t rdlvl_config;
	uint8_t res21[0x4];
	uint32_t brbrsvcontrol;	/* 0x0100*/
	uint32_t brbrsvconfig;
	uint32_t brbqosconfig;
	uint32_t membaseconfig0;
	uint32_t membaseconfig1;	/* 0x0110 */
	uint8_t res22[0xc];
	uint32_t wrlvl_config0;	/* 0x0120 */
	uint32_t wrlvl_config1;
	uint32_t wrlvl_status;
	uint8_t res23[0x4];
	uint32_t perevcontrol;	/* 0x0130 */
	uint32_t perev0config;
	uint32_t perev1config;
	uint32_t perev2config;
	uint32_t perev3config;
	uint8_t res22a[0xc];
	uint32_t ctrl_io_rdata_ch0;
	uint32_t ctrl_io_rdata_ch1;
	uint8_t res23a[0x8];
	uint32_t cacal_config0;
	uint32_t cacal_config1;
	uint32_t cacal_status;
	uint8_t res24[0x94];
	uint32_t emergent_config0;	/* 0x0200 */
	uint32_t emergent_config1;
	uint8_t res25[0x8];
	uint32_t bp_control0;
	uint32_t bp_control0_r;
	uint32_t bp_control0_w;
	uint8_t res26[0x4];
	uint32_t bp_control1;
	uint32_t bp_control1_r;
	uint32_t bp_control1_w;
	uint8_t res27[0x4];
	uint32_t bp_control2;
	uint32_t bp_control2_r;
	uint32_t bp_control2_w;
	uint8_t res28[0x4];
	uint32_t bp_control3;
	uint32_t bp_control3_r;
	uint32_t bp_control3_w;
	uint8_t res29[0xb4];
	uint32_t winconfig_odt_w;	/* 0x0300 */
	uint8_t res30[0x4];
	uint32_t winconfig_ctrl_read;
	uint32_t winconfig_ctrl_gate;
	uint8_t res31[0xdcf0];
	uint32_t pmnc_ppc;
	uint8_t res32[0xc];
	uint32_t cntens_ppc;
	uint8_t res33[0xc];
	uint32_t cntenc_ppc;
	uint8_t res34[0xc];
	uint32_t intens_ppc;
	uint8_t res35[0xc];
	uint32_t intenc_ppc;
	uint8_t res36[0xc];
	uint32_t flag_ppc;		/* 0xe050 */
	uint8_t res37[0xac];
	uint32_t ccnt_ppc;
	uint8_t res38[0xc];
	uint32_t pmcnt0_ppc;
	uint8_t res39[0xc];
	uint32_t pmcnt1_ppc;
	uint8_t res40[0xc];
	uint32_t pmcnt2_ppc;
	uint8_t res41[0xc];
	uint32_t pmcnt3_ppc;	/* 0xe140 */
} __packed;
check_member(exynos5_dmc, pmcnt3_ppc, 0xe140);

static struct exynos5_dmc * const exynos_drex0 = (void *)EXYNOS5420_DMC_DREXI_0;
static struct exynos5_dmc * const exynos_drex1 = (void *)EXYNOS5420_DMC_DREXI_1;

struct exynos5_phy_control {
	uint32_t phy_con0;
	uint32_t phy_con1;
	uint32_t phy_con2;
	uint32_t phy_con3;
	uint32_t phy_con4;
	uint8_t res1[4];
	uint32_t phy_con6;
	uint8_t res2[4];
	uint32_t phy_con8;
	uint32_t phy_con9;
	uint32_t phy_con10;
	uint8_t res3[4];
	uint32_t phy_con12;
	uint32_t phy_con13;
	uint32_t phy_con14;
	uint32_t phy_con15;
	uint32_t phy_con16;
	uint8_t res4[4];	/* NOT a mistake. Yes, it doesn't make sense. */
	uint32_t phy_con17;
	uint32_t phy_con18;
	uint32_t phy_con19;
	uint32_t phy_con20;
	uint32_t phy_con21;
	uint32_t phy_con22;
	uint32_t phy_con23;
	uint32_t phy_con24;
	uint32_t phy_con25;
	uint32_t phy_con26;
	uint32_t phy_con27;
	uint32_t phy_con28;
	uint32_t phy_con29;
	uint32_t phy_con30;
	uint32_t phy_con31;
	uint32_t phy_con32;
	uint32_t phy_con33;
	uint32_t phy_con34;
	uint32_t phy_con35;
	uint32_t phy_con36;
	uint32_t phy_con37;
	uint32_t phy_con38;
	uint32_t phy_con39;
	uint32_t phy_con40;
	uint32_t phy_con41;
	uint32_t phy_con42;
} __packed;
check_member(exynos5_phy_control, phy_con42, 0xac);

static struct exynos5_phy_control * const exynos_phy0_control =
		(void *)EXYNOS5_DMC_PHY0_BASE;
static struct exynos5_phy_control * const exynos_phy1_control =
		(void *)EXYNOS5_DMC_PHY1_BASE;

struct exynos5_tzasc {
	uint8_t res1[0xf00];
	uint32_t membaseconfig0;
	uint32_t membaseconfig1;
	uint8_t res2[0x8];
	uint32_t memconfig0;
	uint32_t memconfig1;
} __packed;

static struct exynos5_tzasc * const exynos_tzasc0 =
		(void *)EXYNOS5420_DMC_TZASC_0;
static struct exynos5_tzasc * const exynos_tzasc1 =
		(void *)EXYNOS5420_DMC_TZASC_1;

enum ddr_mode {
	/* This is in order of ctrl_ddr_mode values. Do not change. */
	DDR_MODE_DDR2	= 0x0,
	DDR_MODE_DDR3	= 0x1,
	DDR_MODE_LPDDR2	= 0x2,
	DDR_MODE_LPDDR3	= 0x3,

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
	MEM_TIMINGS_MSR_COUNT	= 5,
};

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
