/* SPDX-License-Identifier: GPL-2.0-only */

/* Definitions for Genesys Logic GL9755 */

#define CFG		0x800
#define   CFG_EN	0x1
#define LTR			0x5C
#define   SNOOP_VALUE		0x25
#define   SNOOP_SCALE		(0x3 << 10)
#define   NO_SNOOP_VALUE	(0x25 << 16)
#define   NO_SNOOP_SCALE	(0x3 << 26)
