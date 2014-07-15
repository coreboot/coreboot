/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <arch/io.h>
#include <arch/stages.h>
#include <soc/addressmap.h>
#include "mc.h"

void arm64_soc_init(void)
{
	struct tegra_mc_regs * const mc = (void *)(uintptr_t)TEGRA_MC_BASE;
	const size_t tz_size_mib = CONFIG_TRUSTZONE_CARVEOUT_SIZE_MB;
	uintptr_t base;
	uintptr_t end;

	if (!tz_size_mib)
		return;

	/*
	 * Ramstage is when the arm64 first gets running. It also is the
	 * only entity that the capabilities to program the Trust Zone region.
	 * Therefore configure the region early. Also, the TZ region can only
	 * live in 32-bit space.
	 */
	memory_in_range_below_4gb(&base, &end);

	/* Place the TZ area just below current carveout regions. */
	end -= tz_size_mib;
	write32(end << 20, &mc->security_cfg0);
	write32(tz_size_mib, &mc->security_cfg1);
}
