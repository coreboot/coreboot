/* SPDX-License-Identifier: GPL-2.0-only */

/* Driver for BayHub Technology BH720 PCI to eMMC 5.0 HS200 bridge */

#include <types.h>

enum {
	BH720_PROTECT                   = 0xd0,
	BH720_PROTECT_LOCK_OFF          = 0,
	BH720_PROTECT_LOCK_ON           = BIT(0),
	BH720_PROTECT_OFF               = 0,
	BH720_PROTECT_ON                = BIT(31),

	BH720_LINK_CTRL                 = 0x90,
	BH720_LINK_CTRL_L0_ENABLE       = BIT(0),
	BH720_LINK_CTRL_L1_ENABLE       = BIT(1),
	BH720_LINK_CTRL_CLKREQ          = BIT(8),

	BH720_MISC2                     = 0xf0,
	BH720_MISC2_ASPM_DISABLE        = BIT(0),
	BH720_MISC2_APSM_CLKREQ_L1      = BIT(7),
	BH720_MISC2_APSM_PHY_L1         = BIT(10),
	BH720_MISC2_APSM_MORE           = BIT(12),

	BH720_MEM_RW_DATA               = 0x200,
	BH720_MEM_RW_ADR                = 0x204,
	BH720_MEM_RW_READ		= BIT(30),
	BH720_MEM_RW_WRITE		= BIT(31),
	BH720_MEM_ACCESS_EN             = 0x208,
	BH720_PCR_DrvStrength_PLL	= 0x304,
	BH720_PCR_DATA_CMD_DRV_MAX      = 7,
	BH720_PCR_CLK_DRV_MAX           = 7,
	BH720_PCR_EMMC_SETTING		= 0x308,
	BH720_PCR_EMMC_SETTING_1_8V	= BIT(4),

	BH720_RTD3_L1                   = 0x3e0,
	BH720_RTD3_L1_DISABLE_L1        = BIT(28),

	BH720_PCR_CSR			= 0x3e4,
	BH720_PCR_CSR_EMMC_MODE_SEL	= BIT(22),
};
