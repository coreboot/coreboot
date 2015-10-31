/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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
 */

#include <console/console.h>
#include <cpu/amd/microcode.h>

struct id_mapping {
        uint32_t orig_id;
        uint16_t new_id;
};

static u16 get_equivalent_processor_rev_id(u32 orig_id) {
	static const struct id_mapping id_mapping_table[] = {
	#if !CONFIG_K8_REV_F_SUPPORT
	        { 0x0f48, 0x0048 },
	        { 0x0f58, 0x0048 },

	        { 0x0f4a, 0x004a },
	        { 0x0f5a, 0x004a },
	        { 0x0f7a, 0x004a },
	        { 0x0f82, 0x004a },
	        { 0x0fc0, 0x004a },
	        { 0x0ff0, 0x004a },

	        { 0x10f50, 0x0150 },
	        { 0x10f70, 0x0150 },
	        { 0x10f80, 0x0150 },
	        { 0x10fc0, 0x0150 },
	        { 0x10ff0, 0x0150 },

	        { 0x20f10, 0x0210 },
	        { 0x20f12, 0x0210 },
	        { 0x20f32, 0x0210 },
	        { 0x20fb1, 0x0210 },
	#endif

	#if CONFIG_K8_REV_F_SUPPORT
		/* FIXME
		 * Microcode files for CPU revision 0xf do
		 * not seem to be available...
		 */
		{ 0x40f10, 0x0410 },
		{ 0x40f13, 0x0410 },
	#endif

		/* Array terminator */
		{ 0xffffff, 0x0000 },

	};

	unsigned new_id;
	int i;

	new_id = 0;

	for (i = 0; id_mapping_table[i].orig_id != 0xffffff; i++ ) {
		if (id_mapping_table[i].orig_id == orig_id) {
			new_id = id_mapping_table[i].new_id;
			break;
		}
	}

	return new_id;
}

void update_microcode(uint32_t cpu_deviceid)
{
	uint32_t equivalent_rev_id;

        /* Update the microcode */
	equivalent_rev_id = get_equivalent_processor_rev_id(cpu_deviceid);
	amd_update_microcode_from_cbfs(equivalent_rev_id);
}
