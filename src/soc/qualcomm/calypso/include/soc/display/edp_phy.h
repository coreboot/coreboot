/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_CALYPSO_EDP_PHY_H__
#define __SOC_QUALCOMM_CALYPSO_EDP_PHY_H__

#include <types.h>

void edp_phy_config(u8 v_level, u8 p_level, uint32_t link_rate);
void early_phy_enable(void);
int edp_phy_enable(uint32_t link_rate);
int edp_phy_power_on(uint32_t link_rate, unsigned int lane_count);

#endif
