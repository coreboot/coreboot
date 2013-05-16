/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* Register map for Exynos5 sysreg */

#ifndef CPU_SAMSUNG_EXYNOS5420_SYSREG_H
#define CPU_SAMSUNG_EXYNOS5420_SYSREG_H

/* sysreg map */
struct exynos5_sysreg {
	/* Add registers as and when required */
	unsigned char	res1[0x214];
	unsigned int	disp1blk_cfg;
	unsigned char	res2[0x18];
	unsigned int	usb20_phy_cfg;
};

#define FIMDBYPASS_DISP1	(1 << 15)
#define USB20_PHY_CFG_EN	(1 << 0)

#endif
