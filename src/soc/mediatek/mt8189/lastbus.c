/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/lastbus_v2.h>

static const struct lastbus_monitor monitors[] = {
	{
		.name = "debug_ctrl_ao_INFRA_AO",
		.base = INFRA_AO_DEBUG_BASE,
		.num_ports = 43,
		.bus_freq_mhz = 78,
	},
	{
		.name = "debug_ctrl_ao_INFRA_AO1",
		.base = INFRA_AO1_DEBUG_BASE,
		.num_ports = 12,
		.bus_freq_mhz = 78,
	},
	{
		.name = "debug_ctrl_ao_EMISYS_NAO",
		.base = EMISYS_NAO_DEBUG_BASE,
		.num_ports = 11,
		.bus_freq_mhz = 728,
	},
	{
		.name = "debug_ctrl_ao_PERI_PAR_AO",
		.base = PERI_PAR_AO_DEBUG_BASE,
		.num_ports = 23,
		.bus_freq_mhz = 78,
	},
	{
		.name = "debug_ctrl_ao_VLP_AO",
		.base = VLP_AO_DEBUG_BASE,
		.num_ports = 12,
		.bus_freq_mhz = 156,
	},
};

const struct lastbus_config lastbus_cfg = {
	.latch_platform = "MT8189",
	.timeout_ms = 200,
	.timeout_type = 0,
	.num_used_monitors = ARRAY_SIZE(monitors),
	.monitors = monitors,
};
