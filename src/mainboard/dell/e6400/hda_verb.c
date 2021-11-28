/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x111d76b2, /* IDT 92HD71B7X */
	0x10280233, /* Subsystem ID */
	13,	    /* Number of entries */

	/* Pin Widget Verb Table */

	AZALIA_PIN_CFG(0, 0x0a, 0x0421101f),
	AZALIA_PIN_CFG(0, 0x0b, 0x04a11021),
	AZALIA_PIN_CFG(0, 0x0c, 0x40f000f0),
	AZALIA_PIN_CFG(0, 0x0d, 0x90170110),
	AZALIA_PIN_CFG(0, 0x0e, 0x23a1102e),
	AZALIA_PIN_CFG(0, 0x0f, 0x23011050),
	AZALIA_PIN_CFG(0, 0x14, 0x40f000f2),
	AZALIA_PIN_CFG(0, 0x18, 0x90a601a0),
	AZALIA_PIN_CFG(0, 0x19, 0x40f000f4),
	AZALIA_PIN_CFG(0, 0x1e, 0x40f000f5),
	AZALIA_PIN_CFG(0, 0x1f, 0x40f000f6),
	AZALIA_PIN_CFG(0, 0x20, 0x40f000f7),
	AZALIA_PIN_CFG(0, 0x27, 0x40f000f0),
};

const u32 pc_beep_verbs[] = {
	0x00170500, /* power up codec */
	0x00d70500, /* power up speakers */
	0x00d70102, /* select mixer (input 0x2) for speakers */
	0x00d70740, /* enable speakers output */
	0x02770720, /* enable beep input */
	0x01737217, /* unmute beep (mixer's input 0x2), set amp 0dB */
	0x00d37000, /* unmute speakers */
};
AZALIA_ARRAY_SIZES;
