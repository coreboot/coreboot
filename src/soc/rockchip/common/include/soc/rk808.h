/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __COREBOOT_SRC_SOC_ROCKCHIP_COMMON_INCLUDE_SOC_RK808_H
#define __COREBOOT_SRC_SOC_ROCKCHIP_COMMON_INCLUDE_SOC_RK808_H

void rk808_configure_switch(int sw, int enabled);
void rk808_configure_ldo(int ldo, int millivolts);
void rk808_configure_buck(int buck, int millivolts);

#endif  /* ! __COREBOOT_SRC_SOC_ROCKCHIP_COMMON_INCLUDE_SOC_RK808_H */
