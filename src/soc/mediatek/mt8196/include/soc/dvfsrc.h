/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8196_MTK_VCORE
#define SOC_MEDIATEK_MT8196_MTK_VCORE

#define DVFSRC_RSRV_4		(DVFSRC_BASE + 0x290)
#define VCORE_B0_SHIFT		23

void dvfsrc_opp_level_mapping(void);

#endif
