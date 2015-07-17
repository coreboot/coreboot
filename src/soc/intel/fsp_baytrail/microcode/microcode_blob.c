/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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

unsigned microcode[] = {

		/*
 * The problem is that these microcode files are not in the tree. They come
 * with FSP, so let the user deal with the include paths when HAVE_FSP_BIN
 * is enabled.
 */
#if IS_ENABLED(CONFIG_HAVE_FSP_BIN)
#if !IS_ENABLED(CONFIG_SOC_INTEL_FSP_BAYTRAIL_MD)
	/* Region size is 0x30000 - update in microcode_size.h if it gets larger. */
	#include "M0230672228.h"  // M0230672: Bay Trail "Super SKU" B0/B1
	#include "M0130673322.h"  // M0130673: Bay Trail I B2 / B3
	#include "M0130679901.h"  // M0130679: Bay Trail I D0
#else
	/* Region size is 0x10000 - update in microcode_size.h if it gets larger. */
	#include "M0C30678829.h"  // M0C30678: Bay Trail M D Stepping
#endif	/* CONFIG_SOC_INTEL_FSP_BAYTRAIL_MD */
#endif	/* CONFIG_HAVE_FSP_BIN */
};
