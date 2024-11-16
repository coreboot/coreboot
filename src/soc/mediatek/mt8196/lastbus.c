/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/lastbus_v2.h>

static const struct lastbus_monitor monitors[] = {
	{
		.name = "debug_ctrl_ao_DBGSYS_AO",
		.base = DBGSYS_AO_DEBUG_BASE,
		.num_ports = 1,
		.bus_freq_mhz = 26,
	},
	{
		.name = "debug_ctrl_ao_APINFRA_IO_AO",
		.base = APINFRA_IO_AO_DEBUG_BASE,
		.num_ports = 37,
		.bus_freq_mhz = 26,
	},
	{
		.name = "debug_ctrl_ao_APINFRA_IO_CTRL_AO",
		.base = APINFRA_IO_CTRL_AO_DEBUG_BASE,
		.num_ports = 3,
		.bus_freq_mhz = 26,
	},
	{
		.name = "debug_ctrl_ao_APINFRA_DRAMC_AO",
		.base = APINFRA_DRAMC_AO_DEBUG_BASE,
		.num_ports = 11,
		.bus_freq_mhz = 26,
	},
	{
		.name = "debug_ctrl_ao_APINFRA_EMI_AO",
		.base = APINFRA_EMI_AO_DEBUG_BASE,
		.num_ports = 15,
		.bus_freq_mhz = 26,
	},
	{
		.name = "debug_ctrl_ao_APINFRA_BIG4_AO",
		.base = APINFRA_BIG4_AO_DEBUG_BASE,
		.num_ports = 13,
		.bus_freq_mhz = 26,
	},
	{
		.name = "debug_ctrl_ao_APINFRA_IO_INTF_AO",
		.base = APINFRA_IO_INTF_AO_DEBUG_BASE,
		.num_ports = 33,
		.bus_freq_mhz = 26,
	},
	{
		.name = "debug_ctrl_ao_APINFRA_MEM_INTF_AO",
		.base = APINFRA_MEM_INTF_AO_DEBUG_BASE,
		.num_ports = 42,
		.bus_freq_mhz = 26,
	},
	{
		.name = "debug_ctrl_ao_APINFRA_INT_AO",
		.base = APINFRA_INT_AO_DEBUG_BASE,
		.num_ports = 7,
		.bus_freq_mhz = 26,
	},
	{
		.name = "debug_ctrl_ao_APINFRA_MMU_AO",
		.base = APINFRA_MMU_AO_DEBUG_BASE,
		.num_ports = 8,
		.bus_freq_mhz = 26,
	},
	{
		.name = "debug_ctrl_ao_APINFRA_SLB_AO",
		.base = APINFRA_SLB_AO_DEBUG_BASE,
		.num_ports = 8,
		.bus_freq_mhz = 26,
	},
	{
		.name = "debug_ctrl_ao_APINFRA_MEM_AO",
		.base = APINFRA_MEM_AO_DEBUG_BASE,
		.num_ports = 26,
		.bus_freq_mhz = 26,
	},
	{
		.name = "debug_ctrl_ao_APINFRA_MEM_CTRL_AO",
		.base = APINFRA_MEM_CTRL_AO_DEBUG_BASE,
		.num_ports = 2,
		.bus_freq_mhz = 26,
	},
	{
		.name = "debug_ctrl_ao_APINFRA_SSR_AO",
		.base = APINFRA_SSR_AO_DEBUG_BASE,
		.num_ports = 5,
		.bus_freq_mhz = 26,
	},
	{
		.name = "debug_ctrl_ao_NEMI_AO",
		.base = NEMI_AO_DEBUG_BASE,
		.num_ports = 18,
		.bus_freq_mhz = 800,
	},
	{
		.name = "debug_ctrl_ao_SEMI_AO",
		.base = SEMI_AO_DEBUG_BASE,
		.num_ports = 18,
		.bus_freq_mhz = 800,
	},
	{
		.name = "debug_ctrl_ao_EMI_INFRA_AO",
		.base = EMI_INFRA_AO_DEBUG_BASE,
		.num_ports = 70,
		.bus_freq_mhz = 800,
	},
	{
		.name = "debug_ctrl_ao_PERI_PAR_AO",
		.base = PERI_PAR_AO_DEBUG_BASE,
		.num_ports = 27,
		.bus_freq_mhz = 78,
	},
	{
		.name = "debug_ctrl_ao_VLP_AO",
		.base = VLP_AO_DEBUG_BASE,
		.num_ports = 17,
		.bus_freq_mhz = 156,
	},
	{
		.name = "debug_ctrl_ao_MMUP_AO",
		.base = MMUP_AO_DEBUG_BASE,
		.num_ports = 25,
		.bus_freq_mhz = 728,
	},
};

const struct lastbus_config lastbus_cfg = {
	.latch_platform = "MT8196",
	.timeout_ms = 200,
	.timeout_type = 0,
	.num_used_monitors = ARRAY_SIZE(monitors),
	.monitors = monitors,
};
