/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_PCH_H_
#define _BROADWELL_PCH_H_

/* Haswell ULT Pch (LynxPoint-LP) */
#define PCH_LPT_LP_SAMPLE	0x9c41
#define PCH_LPT_LP_PREMIUM	0x9c43
#define PCH_LPT_LP_MAINSTREAM	0x9c45
#define PCH_LPT_LP_VALUE	0x9c47

/* Broadwell PCH (WildatPoint) */
#define PCH_WPT_HSW_U_SAMPLE	0x9cc1
#define PCH_WPT_BDW_U_SAMPLE	0x9cc2
#define PCH_WPT_BDW_U_PREMIUM	0x9cc3
#define PCH_WPT_BDW_U_BASE	0x9cc5
#define PCH_WPT_BDW_Y_SAMPLE	0x9cc6
#define PCH_WPT_BDW_Y_PREMIUM	0x9cc7
#define PCH_WPT_BDW_Y_BASE	0x9cc9
#define PCH_WPT_BDW_H		0x9ccb

/* Power Management Control and Status */
#define PCH_PCS			0x84
#define  PCH_PCS_PS_D3HOT	3

u8 pch_revision(void);
u16 pch_type(void);
int pch_is_wpt(void);
int pch_is_wpt_ulx(void);
u32 pch_read_soft_strap(int id);
void pch_disable_devfn(struct device *dev);

void broadwell_pch_finalize(void);

#endif
