/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2014 Sage Electronic Engineering, LLC
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

unsigned microcode[] = {
#if IS_ENABLED(CONFIG_FSP_MODEL_406DX_A1)
		/* Size is 0x14400 - update in microcode_size.h when the file changes */
		#include <microcode-m01406d000e.h>
#elif IS_ENABLED(CONFIG_FSP_MODEL_406DX_B0)
		/* Size is 0x14800 - update in microcode_size.h when the file changes */
		#include <microcode-m01406d811d.h>
#endif
};
