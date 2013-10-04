/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <stdint.h>
#include <string.h>
#include <console/console.h>
#include <baytrail/mrc_wrapper.h>
#include <baytrail/romstage.h>

void mainboard_romstage_entry(struct romstage_params *rp)
{
	struct mrc_params mp = {
		.mainboard = {
			.dram_type = DRAM_DDR3L,
			.dram_info_location = DRAM_INFO_SPD_SMBUS,
			.spd_addrs = { 0xa0, 0xa2 },
		},
	};
	rp->mrc_params = &mp;
	romstage_common(rp);
}
