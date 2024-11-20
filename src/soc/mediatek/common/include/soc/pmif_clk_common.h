/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MEDIATEK_SOC_PMIF_CLK_COMMON__
#define __MEDIATEK_SOC_PMIF_CLK_COMMON__

int pmif_ulposc_check(u32 current_clk_mhz, u32 target_clk_mhz);
int pmif_ulposc_cali(u32 target_freq_mhz);

#endif /*__MEDIATEK_SOC_PMIF_CLK_COMMON__*/
