/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/amd/microcode.h>

static uint8_t microcode_updates[] __attribute__ ((aligned(16))) = {

#if !CONFIG_K8_REV_F_SUPPORT
	#include "microcode_rev_c.h"
	#include "microcode_rev_d.h"
	#include "microcode_rev_e.h"
#endif

#if CONFIG_K8_REV_F_SUPPORT
//	#include "microcode_rev_f.h"
#endif
        /*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};

static unsigned get_equivalent_processor_rev_id(unsigned orig_id) {
	static unsigned id_mapping_table[] = {
	#if !CONFIG_K8_REV_F_SUPPORT
	        0x0f48, 0x0048,
	        0x0f58, 0x0048,

	        0x0f4a, 0x004a,
	        0x0f5a, 0x004a,
	        0x0f7a, 0x004a,
	        0x0f82, 0x004a,
	        0x0fc0, 0x004a,
	        0x0ff0, 0x004a,

	        0x10f50, 0x0150,
	        0x10f70, 0x0150,
	        0x10f80, 0x0150,
	        0x10fc0, 0x0150,
	        0x10ff0, 0x0150,

	        0x20f10, 0x0210,
	        0x20f12, 0x0210,
	        0x20f32, 0x0210,
	        0x20fb1, 0x0210,
	#endif

	#if CONFIG_K8_REV_F_SUPPORT

	#endif

	};

	unsigned new_id;
	int i;

	new_id = 0;

	for(i=0; i<sizeof(id_mapping_table); i+=2 ) {
		if(id_mapping_table[i]==orig_id) {
			new_id = id_mapping_table[i+1];
			break;
		}
	}

	return new_id;
}

void model_fxx_update_microcode(unsigned cpu_deviceid)
{
	unsigned equivalent_processor_rev_id;

        /* Update the microcode */
	equivalent_processor_rev_id = get_equivalent_processor_rev_id(cpu_deviceid );
	if(equivalent_processor_rev_id != 0)
	        amd_update_microcode(microcode_updates, equivalent_processor_rev_id);
}
