/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_COMMON_DPM_V1_H__
#define __SOC_MEDIATEK_COMMON_DPM_V1_H__

#include <soc/addressmap.h>
#include <soc/mcu_common.h>
#include <types.h>

struct dpm_regs {
	u32 sw_rstn;
	u32 rsvd_0[3072];
	u32 mclk_div;
	u32 rsvd_1[3071];
	u32 twam_window_len;
	u32 twam_mon_type;
	u32 rsvd_2[1022];
	u32 low_power_cfg_0;
	u32 low_power_cfg_1;
	u32 rsvd_3[1];
	u32 fsm_out_ctrl_0;
	u32 rsvd_4[8];
	u32 fsm_cfg_1;
	u32 low_power_cfg_3;
	u32 dfd_dbug_0;
	u32 rsvd_5[28];
	u32 status_4;
};

check_member(dpm_regs, mclk_div, 0x3004);
check_member(dpm_regs, twam_window_len, 0x6004);
check_member(dpm_regs, low_power_cfg_0, 0x7004);
check_member(dpm_regs, low_power_cfg_1, 0x7008);
check_member(dpm_regs, fsm_out_ctrl_0, 0x7010);
check_member(dpm_regs, fsm_cfg_1, 0x7034);
check_member(dpm_regs, low_power_cfg_3, 0x7038);
check_member(dpm_regs, dfd_dbug_0, 0x703C);
check_member(dpm_regs, status_4, 0x70B0);

#define DPM_SW_RSTN_RESET	BIT(0)
#define DPM_MEM_RATIO_OFFSET	28
#define DPM_MEM_RATIO_MASK	(0x3 << DPM_MEM_RATIO_OFFSET)
#define DPM_MEM_RATIO_CFG1	(1 << DPM_MEM_RATIO_OFFSET)
#define DRAMC_MCU_SRAM_ISOINT_B_LSB		BIT(1)
#define DRAMC_MCU2_SRAM_ISOINT_B_LSB		BIT(1)
#define DRAMC_MCU_SRAM_SLEEP_B_LSB		BIT(4)
#define DRAMC_MCU2_SRAM_SLEEP_B_LSB		BIT(4)

static struct dpm_regs *const mtk_dpm = (void *)DPM_CFG_BASE;

void dpm_reset(struct mtk_mcu *mcu);
int dpm_init(void);
int dpm_4ch_para_setting(void);
int dpm_4ch_init(void);

#endif  /* __SOC_MEDIATEK_COMMON_DPM_V1_H__ */
