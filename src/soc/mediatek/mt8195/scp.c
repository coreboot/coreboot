/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <soc/scp.h>

void scp_rsi_enable(void)
{
	u32 val;

	for (val = SCP_SRAM_PDN_DISABLE_VAL; val != 0U;) {
		val = val >> 1;
		write32(REG_L1TCM_SRAM_PDN, val);
	}
}

void scp_rsi_disable(void)
{
	write32(REG_L1TCM_SRAM_PDN, SCP_SRAM_PDN_DISABLE_VAL);
}
