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

#ifndef MB_SYSCONF_H

#define MB_SYSCONF_H

struct mb_sysconf_t {
	u8 bus_8132_0;
	u8 bus_8132_1;
	u8 bus_8132_2;
	u8 bus_8111_0;
	u8 bus_8111_1;
	u8 bus_8132a[31][3];
	u8 bus_8151[31][2];

	u32 apicid_8111;
	u32 apicid_8132_1;
	u32 apicid_8132_2;
	u32 apicid_8132a[31][2];
	u32 sbdn3;
	u32 sbdn3a[31];
	u32 sbdn5[31];
};

#endif

