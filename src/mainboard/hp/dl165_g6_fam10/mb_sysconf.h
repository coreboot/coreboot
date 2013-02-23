/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 AMD
 * Written by Yinghai Lu <yinghailu@gmail.com> for AMD.
 *
 * Copyright (C) 2007 University of Mannheim
 * Written by Philipp Degler <pdegler@rumms.uni-mannheim.e> for Uni of Mannheim
 *
 * Copyright (C) 2009 University of Heidelberg
 * Written by Mondrian Nuessle <nuessle@uni-hd.de> for Uni of Heidelberg
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

#ifndef MB_SYSCONF_H
#define MB_SYSCONF_H

struct mb_sysconf_t {
	unsigned char bus_bcm5780[7];
	unsigned char bus_bcm5785_0;
	unsigned char bus_bcm5785_1;
	unsigned char bus_bcm5785_1_1;
	unsigned apicid_bcm5785[3];

	unsigned sbdn2;
};

#endif

