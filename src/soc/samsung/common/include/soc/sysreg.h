/* SPDX-License-Identifier: GPL-2.0-only */

/* Register map for Exynos5 sysreg */

#ifndef SOC_SAMSUNG_COMMON_INCLUDE_SOC_SYSREG_H
#define SOC_SAMSUNG_COMMON_INCLUDE_SOC_SYSREG_H

#include <soc/cpu.h>

/* sysreg map */
struct exynos5_sysreg {
	/* Add registers as and when required */
	unsigned char	res1[0x214];
	unsigned int	disp1blk_cfg;
	unsigned char	res2[0x18];
	unsigned int	usb20_phy_cfg;
};
check_member(exynos5_sysreg, usb20_phy_cfg, 0x230);

static struct exynos5_sysreg * const exynos_sysreg =
		(void *)EXYNOS5_SYSREG_BASE;

#define FIMDBYPASS_DISP1	(1 << 15)
#define USB20_PHY_CFG_EN	(1 << 0)

#endif /* SOC_SAMSUNG_COMMON_INCLUDE_SOC_SYSREG_H */
