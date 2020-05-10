/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <timer.h>

void timer_monotonic_get(struct mono_time *mt)
{
	mono_time_set_usecs(mt, read32((void *)TEGRA_TMRUS_BASE));
}
