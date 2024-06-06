/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	/* Realtek ALC662 rev1 */
	0x10ec0662, /* Vendor ID */
	0x18493662, /* Subsystem ID */
	10, /* Number of entries */

	/* Pin Widget Verb Table */

	AZALIA_PIN_CFG(0, 0x14, 0x01014010),
	AZALIA_PIN_CFG(0, 0x15, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19830),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19940),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181303f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214120),
	AZALIA_PIN_CFG(0, 0x1c, 0x593301f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x4004c601),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),

	/* coreboot specific header */
	/* Intel Eaglelake HDMI */
	0x80862803, /* Vendor ID */
	0x80860101, /* Subsystem ID */
	0x00000001, /* Number of entries */

	/* Pin Widget Verb Table */

	AZALIA_PIN_CFG(1, 0x03, 0x18560010),

	/* coreboot specific header */
	/* Realtek ALC662 rev1 */
	0x10ec0662, /* Vendor ID */
	0x1565821e, /* Subsystem ID */
	10, /* Number of entries */

	/* Pin Widget Verb Table */
	AZALIA_PIN_CFG(2, 0x14, 0x01014410),
	AZALIA_PIN_CFG(2, 0x15, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, 0x16, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, 0x18, 0x01a19c30),
	AZALIA_PIN_CFG(2, 0x19, 0x02a19c40),
	AZALIA_PIN_CFG(2, 0x1a, 0x0181343f),
	AZALIA_PIN_CFG(2, 0x1b, 0x02214c20),
	AZALIA_PIN_CFG(2, 0x1c, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, 0x1d, 0x4004c601),
	AZALIA_PIN_CFG(2, 0x1e, AZALIA_PIN_CFG_NC(0)),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
