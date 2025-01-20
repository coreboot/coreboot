/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_MT8196_PWRSEL__
#define __SOC_MEDIATEK_MT8196_PWRSEL__

#include <soc/addressmap.h>

#define VAL_PWRSEL			0x0
#define VAL_PWRSEL_AUTO_MODE		0x1FF0000
#define OFFSET_PWRSEL			0x04A0
#define OFFSET_PWRSEL_AUTO_MODE_CFG	0x04A4

#define MFG_VCORE_AO_CFG_BASE		(MFGSYS_BASE + 0x0B860000)		/* 0x4B860000 */
#define MFG_VCORE_AO_RPC_PWRSEL_CONFIG	(MFG_VCORE_AO_CFG_BASE + 0x00B4)	/* 0x4B8600B4 */

void pwrsel_init(void);

#endif /* end of __SOC_MEDIATEK_MT8196_PWRSEL__ */
