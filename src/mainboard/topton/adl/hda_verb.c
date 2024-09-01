/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x8086281c,	/* Vendor ID: Intel Alder Lake */
	0x80860101,	/* Subsystem ID */
	11,		/* Number of entries */
	AZALIA_VERB_12B(2, 0x02, 0x781, 3),
	AZALIA_VERB_12B(2, 0x00, 0xf00, 0),
	AZALIA_VERB_12B(2, 0x00, 0xf00, 0),
	AZALIA_VERB_12B(2, 0x00, 0xf00, 0),
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x04, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x08, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0a, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0b, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0c, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0d, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0e, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0f, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
