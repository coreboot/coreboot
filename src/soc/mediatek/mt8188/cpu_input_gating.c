/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/cpu_input_gating.h>

#define CPU6_IG_EN_REG	(MCUSYS_BASE + 0x3308)
#define CPU7_IG_EN_REG	(MCUSYS_BASE + 0x3b08)
#define DISABLE_CPU_IG	0xfc000001

void disable_cpu_input_gating(void)
{
	write32p(CPU6_IG_EN_REG, DISABLE_CPU_IG);
	write32p(CPU7_IG_EN_REG, DISABLE_CPU_IG);
}
