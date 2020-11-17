/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DRIVERS_GENESYSLOGIC_GL9755_H
#define DRIVERS_GENESYSLOGIC_GL9755_H

/* Definitions for Genesys Logic GL9755 */

#define CFG		0x800
#define   CFG_EN	0x1
#define CFG2			0x48
#define   CFG2_LAT_L1_MASK	((0x7 << 12) | (0x7 << 3))
#define   CFG2_LAT_L1_64US	((0x6 << 12) | (0x6 << 3))
#define LTR			0x5C
#define   SNOOP_VALUE		0x25
#define   SNOOP_SCALE		(0x3 << 10)
#define   NO_SNOOP_VALUE	(0x25 << 16)
#define   NO_SNOOP_SCALE	(0x3 << 26)

#endif /* DRIVERS_GENESYSLOGIC_GL9755_H */
