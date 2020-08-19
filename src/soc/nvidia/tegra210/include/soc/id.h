/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_NVIDIA_TEGRA210_INCLUDE_SOC_ID_H__
#define __SOC_NVIDIA_TEGRA210_INCLUDE_SOC_ID_H__

#include <device/mmio.h>
#include <soc/addressmap.h>

static inline int context_avp(void)
{
	const uint32_t avp_id = 0xaaaaaaaa;
	void *const uptag = (void *)(uintptr_t)TEGRA_PG_UP_BASE;

	return read32(uptag) == avp_id;
}

#endif /* __SOC_NVIDIA_TEGRA210_INCLUDE_SOC_ID_H__ */
