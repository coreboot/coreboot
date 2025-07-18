/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_MT8189_INCLUDE_SOC_THERMAL_INTERNAL_H__
#define __SOC_MEDIATEK_MT8189_INCLUDE_SOC_THERMAL_INTERNAL_H__

#include <soc/addressmap.h>
#include <soc/spm_mtcmos.h>
#include <soc/symbols.h>

#define LVTS_MAGIC			0x0000555
#define MAX_TS_NUMBER			4

#define LVTS_COF_T_SLP_GLD		219960
#define LVTS_COF_COUNT_R_GLD		14437
#define LVTS_COF_T_CONST_OFS		280000
#define LVTS_ADDRESS_INDEX_NUM		(L_TS_LVTS_NUM + LVTS_CONTROLLER_NUM)
#define LVTS_GROUP_INTERVAL_DELAY_MASK	GENMASK(29, 20)
#define LVTS_COEFF_A			(-250460)

#define DEFAULT_EFUSE_GOLDEN_TEMP	50
#define DEFAULT_EFUSE_GOLDEN_TEMP_HT	170
#define DEFAULT_EFUSE_COUNT		35000
#define DEFAULT_EFUSE_COUNT_RC		2750

/* INFRACFG_AO_BASE + 0x0084 */
#define THERM_CG			BIT(10)

/* THERM_CTRL_BASE + lvts_tc_offset_enum + 0x0050 */
#define DEVICE_SENSING_STATUS		BIT(25)
#define DEVICE_ACCESS_START_BIT		BIT(24)
#define WRITE_ACCESS			BIT(16)

/* THERM_CTRL_BASE + lvts_tc_offset_enum + 0x000C */
#define STAGE3_INT_EN			BIT(31)

/* SRAM for Thermal */
#define THERMAL_ATC_SRAM_OFFSET		0x280
#define THERMAL_SRAM_BASE		(_thermal_reserved + THERMAL_ATC_SRAM_OFFSET)
#define THERMAL_SRAM_LEN		(24 * 4)
#define THERMAL_STAT_SRAM_BASE		_cpucooler_reserved
#define THERMAL_STAT_SRAM_LEN		REGION_SIZE(cpucooler_reserved)

/*
 * module		LVTS Plan
 *=====================================================
 * MCU_BIG core0/1	LVTS1-0, LVTS1-1, LVTS1-2, LVTS1-3
 * MCU_BIG core2/3	LVTS2-0, LVTS2-1, LVTS2-2, LVTS2-3
 * MCU_LITTLE		LVTS3-0, LVTS3-1, LVTS3-2, LVTS3-3
 * SOC-TOP,APU		LVTS4-0, LVTS4-1, LVTS4-2, LVTS4-3
 * GPU			LVTS5-0, LVTS5-1
 * ptp_therm_ctrl_AP  Base address: (+0x1031_5000), 0x1031_5100, 0x1031_5200
 * ptp_therm_ctrl_MCU Base address: (+0x1031_6000), 0x1031_6100, 0x1031_6200, 0x1031_6300
 */

/* private thermal sensor */
enum lvts_sensor {
	L_TS_LVTS1_0 = 0,	/* LVTS1-0 Big-core0 */
	L_TS_LVTS1_1,		/* LVTS1-1 Big-core0 */
	L_TS_LVTS1_2,		/* LVTS1-2 Big-core1 */
	L_TS_LVTS1_3,		/* LVTS1-3 Big-core1 */
	L_TS_LVTS2_0,		/* LVTS2-0 Big-core2 */
	L_TS_LVTS2_1,		/* LVTS2-1 Big-core2 */
	L_TS_LVTS2_2,		/* LVTS2-2 Big-core3 */
	L_TS_LVTS2_3,		/* LVTS2-3 Big-core3 */
	L_TS_LVTS3_0,		/* LVTS3-0 Little-core0 */
	L_TS_LVTS3_1,		/* LVTS3-1 Little-core1 */
	L_TS_LVTS3_2,		/* LVTS3-2 Little-core2 */
	L_TS_LVTS3_3,		/* LVTS3-3 Little-core3 */
	L_TS_LVTS4_0,		/* LVTS4-0 SCO-TOP */
	L_TS_LVTS4_1,		/* LVTS4-1 SCO-TOP */
	L_TS_LVTS4_2,		/* LVTS4-0 SCO-TOP */
	L_TS_LVTS4_3,		/* LVTS4-1 APU */
	L_TS_LVTS5_0,		/* LVTS5-0 GPU */
	L_TS_LVTS5_1,		/* LVTS5-1 GPU */
	L_TS_LVTS_NUM,
};

enum lvts_tc_enum {
	LVTS_MCU_CONTROLLER0 = 0,
	LVTS_MCU_CONTROLLER1,
	LVTS_MCU_CONTROLLER2,
	LVTS_AP_CONTROLLER0,
	LVTS_MFG_CONTROLLER0,
	LVTS_CONTROLLER_NUM,
};

enum lvts_tc_offset_enum {
	TC_OFFSET_AP_CONTROLLER0 = 0,
	TC_OFFSET_MFG_CONTROLLER0 = 0x100,
	TC_OFFSET_MCU_CONTROLLER0 = 0x1000,
	TC_OFFSET_MCU_CONTROLLER1 = 0x1100,
	TC_OFFSET_MCU_CONTROLLER2 = 0x1200,
};

struct mtk_thermal_controller_regs {
	u32 lvtsmonctl0_0;
	u32 lvtsmonctl1_0;
	u32 lvtsmonctl2_0;
	u32 lvtsmonint_0;
	u32 reserved1[5];
	u32 lvtsh2nthre_0;
	u32 lvtshthre_0;
	u32 lvtscthre_0;
	u32 lvtsoffseth_0;
	u32 lvtsoffsetl_0;
	u32 lvtsmsrctl0_0;
	u32 lvtsmsrctl1_0;
	u32 lvtstssel_0;
	u32 reserved2[1];
	u32 lvtscalscale_0;
	u32 lvts_id_0;
	u32 lvts_config_0;
	u32 lvtsedata[4];
	u32 reserved3[11];
	u32 lvtsmsr[4];
	u32 reserved4[4];
	u32 lvtsrdata0_0;
	u32 reserved5[3];
	u32 lvtsprotctl_0;
	u32 reserved6[2];
	u32 lvtsprottc_0;
	u32 reserved7[5];
	u32 lvtsclken_0;
	u32 reserved8[2];
	u32 lvtsspare[2];
	u32 reserved9[2];
};
check_member(mtk_thermal_controller_regs, lvtsmonint_0, 0x000C);
check_member(mtk_thermal_controller_regs, lvtsh2nthre_0, 0x0024);
check_member(mtk_thermal_controller_regs, lvtstssel_0, 0x0040);
check_member(mtk_thermal_controller_regs, lvtscalscale_0, 0x0048);
check_member(mtk_thermal_controller_regs, lvtsedata[3], 0x0060);
check_member(mtk_thermal_controller_regs, lvtsmsr[0], 0x0090);
check_member(mtk_thermal_controller_regs, lvtsmsr[3], 0x009C);
check_member(mtk_thermal_controller_regs, lvtsrdata0_0, 0x00B0);
check_member(mtk_thermal_controller_regs, lvtsprotctl_0, 0x00C0);
check_member(mtk_thermal_controller_regs, lvtsprottc_0, 0x00CC);
check_member(mtk_thermal_controller_regs, lvtsclken_0, 0x00E4);
check_member(mtk_thermal_controller_regs, lvtsspare[0], 0x00F0);
check_member(mtk_thermal_controller_regs, lvtsspare[1], 0x00F4);

struct mtk_infracfg_regs {
	u32 reserved1[968];
	u32 mcu_rst_set;
	u32 mcu_rst_clr;
	u32 reserved2[10];
	u32 ap_rst_set;
	u32 ap_rst_clr;
};
check_member(mtk_infracfg_regs, mcu_rst_set, 0x0F20);
check_member(mtk_infracfg_regs, mcu_rst_clr, 0x0F24);
check_member(mtk_infracfg_regs, ap_rst_set, 0x0F50);
check_member(mtk_infracfg_regs, ap_rst_clr, 0x0F54);

 /* LVTS Thermal Controller Register Definition */

static struct mtk_thermal_controller_regs *const
	mtk_lvts_mcu_controller0 = (void *)(THERM_CTRL_BASE + TC_OFFSET_MCU_CONTROLLER0);
static struct mtk_thermal_controller_regs *const
	mtk_lvts_mcu_controller1 = (void *)(THERM_CTRL_BASE + TC_OFFSET_MCU_CONTROLLER1);
static struct mtk_thermal_controller_regs *const
	mtk_lvts_mcu_controller2 = (void *)(THERM_CTRL_BASE + TC_OFFSET_MCU_CONTROLLER2);
static struct mtk_thermal_controller_regs *const
	mtk_lvts_ap_controller0 = (void *)(THERM_CTRL_BASE + TC_OFFSET_AP_CONTROLLER0);
static struct mtk_thermal_controller_regs *const
	mtk_lvts_mfg_controller0 = (void *)(THERM_CTRL_BASE + TC_OFFSET_MFG_CONTROLLER0);

static struct mtk_infracfg_regs *const mtk_infracfg = (void *)INFRACFG_BASE;

#endif /* __SOC_MEDIATEK_MT8189_INCLUDE_SOC_THERMAL_INTERNAL_H__ */
