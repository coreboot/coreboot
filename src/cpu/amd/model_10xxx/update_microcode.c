/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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
 * Foundation, Inc.
 */

#include <stdint.h>
#include <cpu/amd/microcode.h>

struct id_mapping {
        uint32_t orig_id;
        uint16_t new_id;
};

static u16 get_equivalent_processor_rev_id(u32 orig_id) {
	static const struct id_mapping id_mapping_table[] = {
		{ 0x100f00, 0x1000 },
		{ 0x100f01, 0x1000 },
		{ 0x100f02, 0x1000 },
		{ 0x100f20, 0x1020 },
		{ 0x100f21, 0x1020 }, /* DR-B1 */
		{ 0x100f2A, 0x1020 }, /* DR-BA */
		{ 0x100f22, 0x1022 }, /* DR-B2 */
		{ 0x100f23, 0x1022 }, /* DR-B3 */
		{ 0x100f42, 0x1041 }, /* RB-C2 */
		{ 0x100f43, 0x1043 }, /* RB-C3 */
		{ 0x100f52, 0x1041 }, /* BL-C2 */
		{ 0x100f62, 0x1062 }, /* DA-C2 */
		{ 0x100f63, 0x1043 }, /* DA-C3 */
		{ 0x100f81, 0x1081 }, /* HY-D1 */
		{ 0x100fa0, 0x10A0 }, /* PH-E0 */

		/* Array terminator */
		{ 0xffffff, 0x0000 },
	};

	u32 new_id;
	int i;

	new_id = 0;

	for (i = 0; id_mapping_table[i].orig_id != 0xffffff; i++) {
		if (id_mapping_table[i].orig_id == orig_id) {
			new_id = id_mapping_table[i].new_id;
			break;
		}
	}

	return new_id;

}

void update_microcode(u32 cpu_deviceid)
{
	u32 equivalent_processor_rev_id = get_equivalent_processor_rev_id(cpu_deviceid);
	amd_update_microcode_from_cbfs(equivalent_processor_rev_id);
}
