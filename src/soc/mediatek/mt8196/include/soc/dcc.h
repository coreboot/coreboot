/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8196_DCC_H
#define SOC_MEDIATEK_MT8196_DCC_H

#include <soc/addressmap.h>

#define BUS_PLLDIV_CFG1		(MCUSYS_BASE + 0x0104) /* DSU */
#define CPU_PLLDIV_0_CFG1	(MCUSYS_BASE + 0x0110) /* LCPU */
#define CPU_PLLDIV_1_CFG1	(MCUSYS_BASE + 0x011C) /* MCPU */
#define CPU_PLLDIV_2_CFG1	(MCUSYS_BASE + 0x0128) /* BCPU */

void dcc_init(void);

#endif /* SOC_MEDIATEK_MT8196_DCC_H */
