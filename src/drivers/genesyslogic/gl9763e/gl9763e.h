/* SPDX-License-Identifier: GPL-2.0-only */

/* Definitions for Genesys Logic GL9763E */

#include <types.h>

#define VHS		0x884
#define   VHS_REV_MASK	(0xF << 16)
#define   VHS_REV_R	(0x0 << 16)
#define   VHS_REV_M	(0x1 << 16)
#define   VHS_REV_W	(0x2 << 16)
#define SCR		0x8E0
#define   SCR_AXI_REQ	BIT(9)

#define CFG_REG_2	0x8A4
#define   CFG_REG_2_L0S	BIT(11)

#define PLL_CTL		0x938
#define   PLL_CTL_SSC	BIT(19)

#define PLL_CTL_2			0x93C
#define   PLL_CTL_2_MAX_SSC_MASK	(0xFFFF << 16)
#define   MAX_SSC_30000PPM		(0xF5C3 << 16)
