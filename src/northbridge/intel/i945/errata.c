/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
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
#include "i945.h"
#include "raminit.h"

int fixup_i945_errata(void)
{
	u32 reg32;

	/* Mobile Intel 945 Express only */
	reg32 = MCHBAR32(FSBPMC3);
	reg32 &= ~((1 << 13) | (1 << 29));
	MCHBAR32(FSBPMC3) = reg32;

	return 0;
}
