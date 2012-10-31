/*
 * (C) Copyright 2012 Samsung Electronics
 * Register map for Exynos5 sysreg
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __EXYNOS5_SYSREG_H__
#define __EXYNOS5_SYSREG_H__

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
