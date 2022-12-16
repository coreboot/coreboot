/* SPDX-License-Identifier: GPL-2.0-only */

Scope (GFX0)
{
	OperationRegion (GFXC, PCI_Config, 0x00, 0x0100)
	Field (GFXC, DWordAcc, NoLock, Preserve)
	{
		Offset (0x10),
		BAR0, 64,
		Offset (0xe4),
		ASLE, 32,
		Offset (0xfc),
		ASLS, 32,
	}

	OperationRegion (GFRG, SystemMemory, BAR0 & 0xfffffffffffffff0, 0x400000)
	Field (GFRG, DWordAcc, NoLock, Preserve)
	{
		Offset (CONFIG_INTEL_GMA_BCLV_OFFSET),
		BCLV, CONFIG_INTEL_GMA_BCLV_WIDTH,
	}
	Field (GFRG, DWordAcc, NoLock, Preserve)
	{
		Offset (CONFIG_INTEL_GMA_BCLM_OFFSET),
		BCLM, CONFIG_INTEL_GMA_BCLM_WIDTH
	}

#include "configure_brightness_levels.asl"
#include "common.asl"
}
