/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_METEORLAKE_BOOTBLOCK_H_
#define _SOC_METEORLAKE_BOOTBLOCK_H_

/* Bootblock pre console init programming */
void bootblock_soc_die_early_init(void);
void bootblock_ioe_die_early_init(void);

/* Bootblock post console init programming */
void bootblock_soc_die_init(void);
void report_platform_info(void);

#endif
