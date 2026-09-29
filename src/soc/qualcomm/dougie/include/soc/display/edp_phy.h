/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_DOUGIE_DISPLAY_EDP_PHY_H_
#define _SOC_QUALCOMM_DOUGIE_DISPLAY_EDP_PHY_H_

#include <types.h>

void edp_phy_config(u8 v_level, u8 p_level);
void edp_phy_vm_pe_init(void);
int edp_phy_enable(void);
int edp_phy_power_on(uint32_t link_rate);

#endif /* _SOC_QUALCOMM_DOUGIE_DISPLAY_EDP_PHY_H_ */
