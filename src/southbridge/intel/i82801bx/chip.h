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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/*
 * The i82801bx code currently supports:
 *  - 82801AA
 *  - 82801AB
 *  - 82801BA
 *  - 82801CA
 *  - 82801DB
 *  - 82801DBM
 *  - 82801EB
 *  - 82801ER
 *
 * This code should NOT be used for ICH6 and later versions.
 */

#ifndef SOUTHBRIDGE_INTEL_I82801BX_CHIP_H
#define SOUTHBRIDGE_INTEL_I82801BX_CHIP_H

struct southbridge_intel_i82801bx_config {
	/**
	 * Interrupt Routing configuration
	 * If bit7 is 1, the interrupt is disabled.
	 */
	uint8_t pirqa_routing;
	uint8_t pirqb_routing;
	uint8_t pirqc_routing;
	uint8_t pirqd_routing;
	uint8_t pirqe_routing;
	uint8_t pirqf_routing;
	uint8_t pirqg_routing;
	uint8_t pirqh_routing;

	uint8_t ide0_enable;
	uint8_t ide1_enable;
};

extern struct chip_operations southbridge_intel_i82801bx_ops;

#endif
