/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* Register map for Exynos5 sysreg */

#ifndef CPU_SAMSUNG_EXYNOS5250_SYSREG_H
#define CPU_SAMSUNG_EXYNOS5250_SYSREG_H

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

#endif
