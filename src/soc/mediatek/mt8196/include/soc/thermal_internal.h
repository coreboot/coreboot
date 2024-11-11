/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8196_THERMAL_INTERNAL_H
#define SOC_MEDIATEK_MT8196_THERMAL_INTERNAL_H

#include <console/console.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/symbols.h>
#include <soc/thermal.h>
#include <stddef.h>

#define LVTS_MAGIC                      0x0000555
#define MAX_TS_NUMBER			4

/* private thermal sensor enum */
enum lvts_sensor {
	L_TS_LVTS11_0 = 0,	/* LVTS11-0 SOC-TOP */
	L_TS_LVTS11_1,		/* LVTS11-1 SOC-TOP */
	L_TS_LVTS11_2,		/* LVTS11-2 SOC-TOP */
	L_TS_LVTS11_3,		/* LVTS11-3 SOC-TOP */
	L_TS_LVTS12_0,		/* LVTS12-0 SOC-BOT */
	L_TS_LVTS12_1,		/* LVTS12-1 SOC-BOT */
	L_TS_LVTS12_2,		/* LVTS12-2 SOC-BOT */
	L_TS_LVTS12_3,		/* LVTS12-3 SOC-BOT */
	L_TS_LVTS13_0,		/* LVTS13-0 MD-AP */
	L_TS_LVTS13_1,		/* LVTS13-1 MD-AP */
	L_TS_LVTS13_2,		/* LVTS13-2 MD-AP */
	L_TS_LVTS13_3,		/* LVTS13-3 MD-AP */
	L_TS_LVTS14_0,		/* LVTS14-0 SOC-ADCT */
	L_TS_LVTS14_3,		/* LVTS14-3 SOC-ADCT */
	L_TS_LVTS_NUM,
};

enum lvts_tc {
	LVTS_AP_CONTROLLER0 = 0,
	LVTS_AP_CONTROLLER1,
	LVTS_AP_CONTROLLER2,
	LVTS_AP_CONTROLLER3,
	LVTS_CONTROLLER_NUM,
};

enum lvts_tc_offset {
	TS_OFFSET_AP_CONTROLLER0 = 0,
	TS_OFFSET_AP_CONTROLLER1 = 0x100,
	TS_OFFSET_AP_CONTROLLER2 = 0x200,
	TS_OFFSET_AP_CONTROLLER3 = 0x300,
};

enum sensor_switch_status {
	SEN_OFF,
	SEN_ON,
};

enum controller_switch_status {
	CTRL_OFF,
	CTRL_ON,
};

struct lvts_thermal_controller_speed {
	uint32_t group_interval_delay;
	uint32_t period_unit;
	uint32_t filter_interval_delay;
	uint32_t sensor_interval_delay;
};

struct lvts_thermal_controller {
	enum lvts_sensor ts[MAX_TS_NUMBER];
	enum sensor_switch_status sensor_on_off[MAX_TS_NUMBER];
	enum controller_switch_status ctrl_on_off;
	size_t ts_number;
	int reboot_temperature;
	int dominator_ts_idx;
	struct lvts_thermal_controller_speed speed;
	struct mtk_thermal_controller_regs *regs;
};

/* LVTS Thermal Controller Register Definition */
static struct mtk_thermal_controller_regs *const
	mtk_lvts_ap_controller0 = (void *)(THERM_CTRL_BASE + TS_OFFSET_AP_CONTROLLER0);
static struct mtk_thermal_controller_regs *const
	mtk_lvts_ap_controller1 = (void *)(THERM_CTRL_BASE + TS_OFFSET_AP_CONTROLLER1);
static struct mtk_thermal_controller_regs *const
	mtk_lvts_ap_controller2 = (void *)(THERM_CTRL_BASE + TS_OFFSET_AP_CONTROLLER2);
static struct mtk_thermal_controller_regs *const
	mtk_lvts_ap_controller3 = (void *)(THERM_CTRL_BASE + TS_OFFSET_AP_CONTROLLER3);

struct mtk_thermal_controller_regs {
	u32 lvtsmonctl0_0;
	u32 lvtsmonctl1_0;
	u32 lvtsmonctl2_0;
	u32 lvtsmonint_0;
	u32 lvtsmonintsts_0;
	u32 lvtsmonidet0_0;
	u32 lvtsmonidet1_0;
	u32 lvtsmonidet2_0;
	u32 lvtsmonidet3_0;
	u32 lvtsh2nthre_0;
	u32 lvtshthre_0;
	u32 lvtscthre_0;
	u32 lvtsoffseth_0;
	u32 lvtsoffsetl_0;
	u32 lvtsmsrctl0_0;
	u32 lvtsmsrctl1_0;
	u32 lvtstssel_0;
	u32 lvtsdeviceto_0;
	u32 lvtscalscale_0;
	u32 lvts_id_0;
	u32 lvts_config_0;
	u32 lvtsedata[4];
	u32 reserved0[1];
	u32 lvtsgslope_0;
	u32 lvtsmsroft_0;
	u32 lvtsatp[4];
	u32 reserved1[4];
	u32 lvtsmsr[4];
	u32 lvtsimmd[4];
	u32 lvtsrdata[4];
	u32 lvtsprotctl_0;
	u32 lvtsprotta_0;
	u32 lvtsprottb_0;
	u32 lvtsprottc_0;
	u32 reserved2[1];
	u32 lvtstemp[4];
	u32 lvtsclken_0;
	u32 lvtsdbgsel_0;
	u32 lvtsdbgsig_0;
	u32 lvtsspare[4];
};

check_member(mtk_thermal_controller_regs, lvtsmonctl0_0, 0x000);
check_member(mtk_thermal_controller_regs, lvtsmonctl1_0, 0x004);
check_member(mtk_thermal_controller_regs, lvtsmonctl2_0, 0x008);
check_member(mtk_thermal_controller_regs, lvtsmonint_0, 0x00c);
check_member(mtk_thermal_controller_regs, lvtsmonintsts_0, 0x010);
check_member(mtk_thermal_controller_regs, lvtsmonidet0_0, 0x014);
check_member(mtk_thermal_controller_regs, lvtsmonidet1_0, 0x018);
check_member(mtk_thermal_controller_regs, lvtsmonidet2_0, 0x01c);
check_member(mtk_thermal_controller_regs, lvtsmonidet3_0, 0x020);
check_member(mtk_thermal_controller_regs, lvtsh2nthre_0, 0x024);
check_member(mtk_thermal_controller_regs, lvtshthre_0, 0x028);
check_member(mtk_thermal_controller_regs, lvtscthre_0, 0x02c);
check_member(mtk_thermal_controller_regs, lvtsoffseth_0, 0x030);
check_member(mtk_thermal_controller_regs, lvtsoffsetl_0, 0x034);
check_member(mtk_thermal_controller_regs, lvtsmsrctl0_0, 0x038);
check_member(mtk_thermal_controller_regs, lvtsmsrctl1_0, 0x03c);
check_member(mtk_thermal_controller_regs, lvtstssel_0, 0x040);
check_member(mtk_thermal_controller_regs, lvtsdeviceto_0, 0x044);
check_member(mtk_thermal_controller_regs, lvtscalscale_0, 0x048);
check_member(mtk_thermal_controller_regs, lvts_id_0, 0x04c);
check_member(mtk_thermal_controller_regs, lvts_config_0, 0x050);
check_member(mtk_thermal_controller_regs, lvtsedata, 0x054);
check_member(mtk_thermal_controller_regs, lvtsgslope_0, 0x068);
check_member(mtk_thermal_controller_regs, lvtsmsroft_0, 0x06c);
check_member(mtk_thermal_controller_regs, lvtsatp, 0x070);
check_member(mtk_thermal_controller_regs, lvtsmsr, 0x090);
check_member(mtk_thermal_controller_regs, lvtsimmd, 0x0a0);
check_member(mtk_thermal_controller_regs, lvtsrdata, 0x0b0);
check_member(mtk_thermal_controller_regs, lvtsprotctl_0, 0x0c0);
check_member(mtk_thermal_controller_regs, lvtsprotta_0, 0x0c4);
check_member(mtk_thermal_controller_regs, lvtsprottb_0, 0x0c8);
check_member(mtk_thermal_controller_regs, lvtsprottc_0, 0x0cc);
check_member(mtk_thermal_controller_regs, lvtstemp, 0x0d4);
check_member(mtk_thermal_controller_regs, lvtsclken_0, 0x0e4);
check_member(mtk_thermal_controller_regs, lvtsdbgsel_0, 0x0e8);
check_member(mtk_thermal_controller_regs, lvtsdbgsig_0, 0x0ec);
check_member(mtk_thermal_controller_regs, lvtsspare, 0x0f0);

#define LDO_ON_SETTING			0x1108

#define AP_RST_SET			(INFRACFG_AO_SEC_BASE + 0xf30)
#define AP_RST_CLR			(INFRACFG_AO_SEC_BASE + 0xf34)

#define LVTS_COF_T_SLP_GLD		358830
#define LVTS_COF_COUNT_R_GLD		34389
#define LVTS_COF_T_CONST_OFS		0

#define DEFAULT_EFUSE_GOLDEN_TEMP	60
#define DEFAULT_EFUSE_COUNT		34389
#define DEFAULT_EFUSE_COUNT_RC		24173

#define HIGH_OFFSET3_INT_EN		BIT(25)
#define HIGH_OFFSET2_INT_EN		BIT(13)
#define HIGH_OFFSET1_INT_EN		BIT(8)
#define HIGH_OFFSET0_INT_EN		BIT(3)

#define LOW_OFFSET3_INT_EN		BIT(24)
#define LOW_OFFSET2_INT_EN		BIT(12)
#define LOW_OFFSET1_INT_EN		BIT(7)
#define LOW_OFFSET0_INT_EN		BIT(2)

#define HOT_INT3_EN			BIT(23)
#define HOT_INT2_EN			BIT(11)
#define HOT_INT1_EN			BIT(6)
#define HOT_INT0_EN			BIT(1)

#define COLD_INT3_EN			BIT(22)
#define COLD_INT2_EN			BIT(10)
#define COLD_INT1_EN			BIT(5)
#define COLD_INT0_EN			BIT(0)

#define STAGE3_INT_EN			BIT(31)

#define DEVICE_ACCESS_START_BIT	BIT(24)

#define CPU_LVTS_RESET_ADDR		(MCUCFG_BASE + 0x610)

/* chip dependent */
#define LVTS_ADDRESS_INDEX_0	(EFUSEC_BASE + 0x334)
#define LVTS_ADDRESS_INDEX_1	(EFUSEC_BASE + 0x338)
#define LVTS_ADDRESS_INDEX_2	(EFUSEC_BASE + 0x33C)
#define LVTS_ADDRESS_INDEX_3	(EFUSEC_BASE + 0x340)
#define LVTS_ADDRESS_INDEX_4	(EFUSEC_BASE + 0x344)
#define LVTS_ADDRESS_INDEX_5	(EFUSEC_BASE + 0x348)
#define LVTS_ADDRESS_INDEX_6	(EFUSEC_BASE + 0x34C)
#define LVTS_ADDRESS_INDEX_7	(EFUSEC_BASE + 0x350)
#define LVTS_ADDRESS_INDEX_8	(EFUSEC_BASE + 0x354)
#define LVTS_ADDRESS_INDEX_9	(EFUSEC_BASE + 0x358)
#define LVTS_ADDRESS_INDEX_10	(EFUSEC_BASE + 0x35C)
#define LVTS_ADDRESS_INDEX_11	(EFUSEC_BASE + 0x360)
#define LVTS_ADDRESS_INDEX_12	(EFUSEC_BASE + 0x364)
#define LVTS_ADDRESS_INDEX_13	(EFUSEC_BASE + 0x368)
#define LVTS_ADDRESS_INDEX_14	(EFUSEC_BASE + 0x36C)
#define LVTS_ADDRESS_INDEX_15	(EFUSEC_BASE + 0x370)
#define LVTS_ADDRESS_INDEX_16	(EFUSEC_BASE + 0x374)
#define LVTS_ADDRESS_INDEX_17	(EFUSEC_BASE + 0x378)
#define LVTS_ADDRESS_INDEX_18	(EFUSEC_BASE + 0x37C)
#define LVTS_ADDRESS_INDEX_19	(EFUSEC_BASE + 0x380)
#define LVTS_ADDRESS_INDEX_20	(EFUSEC_BASE + 0x384)
#define LVTS_ADDRESS_INDEX_21	(EFUSEC_BASE + 0x388)
#define LVTS_ADDRESS_INDEX_22	(EFUSEC_BASE + 0x38C)
#define LVTS_ADDRESS_INDEX_23	(EFUSEC_BASE + 0x390)
#define LVTS_ADDRESS_INDEX_24	(EFUSEC_BASE + 0x394)
#define LVTS_ADDRESS_INDEX_25	(EFUSEC_BASE + 0x398)
#define LVTS_ADDRESS_INDEX_26	(EFUSEC_BASE + 0x39C)
#define LVTS_ADDRESS_INDEX_27	(EFUSEC_BASE + 0x3A0)
#define LVTS_ADDRESS_INDEX_28	(EFUSEC_BASE + 0x3A4)
#define LVTS_ADDRESS_INDEX_29	(EFUSEC_BASE + 0x3A8)
#define LVTS_ADDRESS_INDEX_30	(EFUSEC_BASE + 0x3AC)
#define LVTS_ADDRESS_INDEX_31	(EFUSEC_BASE + 0x3B0)

#endif /* SOC_MEDIATEK_MT8196_THERMAL_INTERNAL_H */
