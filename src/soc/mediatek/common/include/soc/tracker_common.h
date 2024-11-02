/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_TRACKER_COMMON_H
#define SOC_MEDIATEK_TRACKER_COMMON_H

#include <stdint.h>

#define BUS_DBG_CON_IRQ_AR_STA0		0x00000100
#define BUS_DBG_CON_IRQ_AW_STA0		0x00000200
#define BUS_DBG_CON_IRQ_AR_STA1		0x00100000
#define BUS_DBG_CON_IRQ_AW_STA1		0x00200000
#define BUS_DBG_CON_TIMEOUT		(BUS_DBG_CON_IRQ_AR_STA0 | BUS_DBG_CON_IRQ_AW_STA0 | \
					 BUS_DBG_CON_IRQ_AR_STA1 | BUS_DBG_CON_IRQ_AW_STA1)

struct tracker {
	uintptr_t base_addr;
	u32 timeout;
	u32 entry;
	const u32 *offsets;
	u32 offsets_size;
	const char *str;
};

extern struct tracker tracker_data[];

void tracker_setup(void);
void bustracker_init(void);

#endif
