/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SKYLAKE_BOOTBLOCK_H_
#define _SOC_SKYLAKE_BOOTBLOCK_H_

/* Bootblock pre console init programming */
void bootblock_cpu_init(void);
void bootblock_pch_early_init(void);

/* Bootblock post console init programming */
void bootblock_pch_init(void);
void pch_early_iorange_init(void);
void report_platform_info(void);

#endif
