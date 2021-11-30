/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ALDERLAKE_BOOTBLOCK_H_
#define _SOC_ALDERLAKE_BOOTBLOCK_H_

#if	CONFIG(SOC_INTEL_ALDERLAKE_PCH_M) +	\
	CONFIG(SOC_INTEL_ALDERLAKE_PCH_N) +	\
	CONFIG(SOC_INTEL_ALDERLAKE_PCH_P) != 1
#error "Please select exactly one PCH type"
#endif

/* Bootblock pre console init programming */
void bootblock_pch_early_init(void);

/* Bootblock post console init programming */
void bootblock_pch_init(void);
void pch_early_iorange_init(void);
void report_platform_info(void);

#endif
