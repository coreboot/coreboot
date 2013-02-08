/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 ChromeOS Authors
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#ifndef ROMSTAGE_HANDOFF_H
#define ROMSTAGE_HANDOFF_H

#include <stdint.h>

/* It is the chipset's responsbility for maintaining the integrity of this
 * structure in CBMEM. For instance, if chipset code adds this structure
 * using the CBMEM_ID_ROMSTAGE_INFO id it needs to ensure it doesn't clobber
 * fields it doesn't own. */
struct romstage_handoff {
	/* This indicates to the ramstage to reserve a chunk of memory. */
	uint32_t reserve_base;
	uint32_t reserve_size;
};

#endif /* ROMSTAGE_HANDOFF_H */

