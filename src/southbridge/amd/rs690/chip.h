/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

#ifndef RS690_CHIP_H
#define RS690_CHIP_H

/* Member variables are defined in devicetree.cb. */
struct southbridge_amd_rs690_config
{
	u8 gpp_configuration;	/* The configuration of General Purpose Port, A/B/C/D/E. */
	u8 port_enable;		/* Which port is enabled? GFX(2,3), GPP(4,5,6,7) */
	u8 gfx_dev2_dev3;	/* for GFX Core initialization REFCLK_SEL */
	u8 gfx_dual_slot;	/* Is it dual graphics slots */
	u8 gfx_lane_reversal;	/* Single/Dual slot lan reversal */
	u8 gfx_tmds;		/* whether support TMDS? */
	u8 gfx_compliance;	/* whether support compliance? */
	u8 gfx_reconfiguration;	/* Dynamic Lind Width Control */
	u8 gfx_link_width;	/* Desired width of lane 2 */
};

#endif /* RS690_CHIP_H */
