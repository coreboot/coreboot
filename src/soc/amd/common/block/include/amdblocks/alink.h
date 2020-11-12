/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __AMD_BLOCK_ALINK_H__
#define __AMD_BLOCK_ALINK_H__

#include <stdint.h>

#define AB_INDX				0xcd8
#define AB_DATA				(AB_INDX + 4)

#define AX_INDXC			0
#define AX_INDXP			2
#define AXCFG				4
#define ABCFG				6
#define RC_INDXC			1
#define RC_INDXP			3

#define abcfg_reg(reg, mask, val)	\
	alink_ab_indx((ABCFG), (reg), (mask), (val))
#define axcfg_reg(reg, mask, val)	\
	alink_ab_indx((AXCFG), (reg), (mask), (val))
#define axindxc_reg(reg, mask, val)	\
	alink_ax_indx((AX_INDXC), (reg), (mask), (val))
#define axindxp_reg(reg, mask, val)		\
	alink_ax_indx((AX_INDXP), (reg), (mask), (val))
#define rcindxc_reg(reg, port, mask, val)	\
	alink_rc_indx((RC_INDXC), (reg), (port), (mask), (val))
#define rcindxp_reg(reg, port, mask, val)	\
	alink_rc_indx((RC_INDXP), (reg), (port), (mask), (val))

void alink_rc_indx(u32 reg_space, u32 reg_addr, u32 port, u32 mask, u32 val);
void alink_ab_indx(u32 reg_space, u32 reg_addr, u32 mask, u32 val);
void alink_ax_indx(u32 space /* c or p? */, u32 axindc, u32 mask, u32 val);

#endif /* __AMD_BLOCK_ALINK_H__ */
