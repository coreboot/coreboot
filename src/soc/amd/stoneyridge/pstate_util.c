/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <amdblocks/smn.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/msr.h>
#include <device/pci_ops.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <types.h>

uint32_t get_pstate_0_reg(void)
{
	return (pci_read_config32(SOC_PM_DEV, CORE_PERF_BOOST_CTRL) >> 2) & 0x7;
}

static bool all_pstates_have_same_frequency_id(void)
{
	union pstate_msr pstate_reg;
	size_t i;
	bool first = true;
	uint32_t frequency_id;

	for (i = 0; i < 7; i++) {
		pstate_reg.raw = rdmsr(PSTATE_MSR(i)).raw;

		if (!pstate_reg.pstate_en)
			continue;

		if (first) {
			frequency_id = pstate_reg.cpu_fid_0_5;
			first = false;
		} else if (frequency_id != pstate_reg.cpu_fid_0_5) {
			return false;
		}
	}

	return true;
}

#define CLK_PLL_LOCK_TIMER		0xD82220B8
#define CLK_GATER_SEQUENCE_REGISTER	0xD8222114

uint32_t get_pstate_latency(void)
{
	uint32_t latency = 0;
	uint32_t smn_data;
	uint32_t gaters_on_time, gaters_off_time;

	smn_data = smn_read32(CLK_GATER_SEQUENCE_REGISTER);
	gaters_on_time = (smn_data & 0xff) * 10;
	gaters_off_time = (smn_data >> 8 & 0xff) * 10;
	latency += DIV_ROUND_UP(15 * gaters_on_time, 1000);
	latency += DIV_ROUND_UP(15 * gaters_off_time, 1000);

	if (!all_pstates_have_same_frequency_id()) {
		smn_data = smn_read32(CLK_PLL_LOCK_TIMER);
		latency += DIV_ROUND_UP(smn_data & 0x1fff, 100);
	}

	return latency;
}
