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
#define   CFG_REG_2_L1DLY_MAX	(0x3FF << 19)

#define PLL_CTL		0x938
#define   PLL_CTL_SSC	BIT(19)

#define EMMC_CTL	0x960
#define   SLOW_MODE	BIT(3)

#define PLL_CTL_2			0x93C
#define   PLL_CTL_2_MAX_SSC_MASK	(0xFFFF << 16)
#define   MAX_SSC_30000PPM		(0xF5C3 << 16)

#define HW_VER_2	0x8F8
#define   HW_VER_MASK	0xFFFF
#define   REVISION_03	0x0011

#define SD_CLKRX_DLY			0x934
#define   CLK_SRC_MASK			(0x3 << 24)
#define   AFTER_OUTPUT_BUFF		(0x0 << 24)
#define   HS400_RX_DELAY_MASK		(0xF << 28)
#define   HS400_RX_DELAY		(0x5 << 28)
