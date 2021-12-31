/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BAYTRAIL_MODPHY_TABLE_H_
#define _BAYTRAIL_MODPHY_TABLE_H_

#include <stdint.h>

struct modphy_entry {
	u8 port;
	u32 reg;
	u32 mask;
	u32 value;
	u8 op_read;
	u8 op_write;
};

#define MODPHY_ENTRY_END ((struct modphy_entry) { 0, 0, 0, 0, 0, 0 })

extern struct modphy_entry reva0_modphy_table[]; /* SOC stepping A0/A1 */
extern struct modphy_entry revb0_modphy_table[]; /* SOC stepping B0 or later */

#endif
