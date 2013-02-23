/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Corey Osgood <corey_osgood@verizon.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

/*
 * The i82801bx code supports: 82801BA/82801BAM (ICH2/ICH2-M)
 */

#ifndef SOUTHBRIDGE_INTEL_I82801BX_CHIP_H
#define SOUTHBRIDGE_INTEL_I82801BX_CHIP_H

#include <stdint.h>

struct southbridge_intel_i82801bx_config {
	u8 pirqa_routing;
	u8 pirqb_routing;
	u8 pirqc_routing;
	u8 pirqd_routing;
	u8 pirqe_routing;
	u8 pirqf_routing;
	u8 pirqg_routing;
	u8 pirqh_routing;

	u8 ide0_enable;
	u8 ide1_enable;
};

#endif
