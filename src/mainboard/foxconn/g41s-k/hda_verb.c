/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>

#if CONFIG(BOARD_FOXCONN_G41S_K)
const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0888, /* Vendor ID */
	0x105b0dda, /* Subsystem ID */
	0x0000000e, /* Number of entries */

	/* Pin Widget Verb Table */

	AZALIA_PIN_CFG(0, 0x11, 0x99430140),
	AZALIA_PIN_CFG(0, 0x12, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x14, 0x01014410),
	AZALIA_PIN_CFG(0, 0x15, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19c50),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19c60),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181345f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214c20),
	AZALIA_PIN_CFG(0, 0x1c, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x4004c601),
	AZALIA_PIN_CFG(0, 0x1e, 0x01441130),
	AZALIA_PIN_CFG(0, 0x1f, AZALIA_PIN_CFG_NC(0)),
};
#else /* CONFIG_BOARD_FOXCONN_G41M */
const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0888, /* Vendor ID */
	0x105b0dc0, /* Subsystem ID */
	0x0000000e, /* Number of entries */

	/* Pin Widget Verb Table */

	AZALIA_PIN_CFG(2, 0x11, 0x01441140),
	AZALIA_PIN_CFG(2, 0x12, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, 0x14, 0x01014410),
	AZALIA_PIN_CFG(2, 0x15, 0x01011412),
	AZALIA_PIN_CFG(2, 0x16, 0x01016411),
	AZALIA_PIN_CFG(2, 0x17, 0x01012414),
	AZALIA_PIN_CFG(2, 0x18, 0x01a19c50),
	AZALIA_PIN_CFG(2, 0x19, 0x02a19c60),
	AZALIA_PIN_CFG(2, 0x1a, 0x0181345f),
	AZALIA_PIN_CFG(2, 0x1b, 0x02014c20),
	AZALIA_PIN_CFG(2, 0x1c, 0x593301f0),
	AZALIA_PIN_CFG(2, 0x1d, 0x4007f603),
	AZALIA_PIN_CFG(2, 0x1e, 0x99430130),
	AZALIA_PIN_CFG(2, 0x1f, AZALIA_PIN_CFG_NC(0)),
};
#endif

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
