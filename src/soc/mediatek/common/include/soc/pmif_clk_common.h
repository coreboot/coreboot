/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MEDIATEK_SOC_PMIF_CLK_COMMON__
#define __MEDIATEK_SOC_PMIF_CLK_COMMON__

int pmif_ulposc_check(u32 current_clk, u32 target_clk);
int pmif_ulposc_cali(u32 target_val);

#endif /*__MEDIATEK_SOC_PMIF_CLK_COMMON__*/
