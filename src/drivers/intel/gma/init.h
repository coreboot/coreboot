/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013, 2014 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 or (at your option)
 *  any later version of the License.
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

#ifndef _INTEL_GMA_INIT
#define _INTEL_GMA_INIT 1

struct intel_gma_info
{
	int use_spread_spectrum_clock;
	int lvds_dual_channel;
	int link_frequency_270_mhz;
};

void intel_gma_init(const struct intel_gma_info *info,
		    u32 mmio, u32 physbase, u32 gttbase);

#endif
