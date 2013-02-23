/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#ifndef AMDFAM10_NUMS_H

#define AMDFAM10_NUMS_H

#if CONFIG_MAX_PHYSICAL_CPUS > 8
	#if CONFIG_MAX_PHYSICAL_CPUS > 32
		#define NODE_NUMS 64
	#else
		#define NODE_NUMS 32
	#endif
#else
	#define NODE_NUMS 8
#endif

// max HC installed at the same time. ...could be bigger than (48+24) if we have 3x4x4
#define HC_NUMS 32

//it could be more bigger
#define HC_POSSIBLE_NUM 32

#endif

