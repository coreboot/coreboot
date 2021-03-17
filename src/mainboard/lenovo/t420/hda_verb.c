/* SPDX-License-Identifier: GPL-2.0-only */

/* Bits 31:28 - Codec Address */
/* Bits 27:20 - NID */
/* Bits 19:8 - Verb ID */
/* Bits 7:0  - Payload */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x14f1506e,	/* Codec VID / DID: Conexant CX20590 - schematic shows CX20672 */
	0x17aa21ce,	/* Subsystem ID */
	13,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x17aa21ce),
	AZALIA_PIN_CFG(0, 0x19, 0x04211040),
	AZALIA_PIN_CFG(0, 0x1a, 0x61a19050),
	AZALIA_PIN_CFG(0, 0x1b, 0x04a11060),
	AZALIA_PIN_CFG(0, 0x1c, 0x6121401f),
	AZALIA_PIN_CFG(0, 0x1d, 0x40f001f0),
	AZALIA_PIN_CFG(0, 0x1e, 0x40f001f0),
	AZALIA_PIN_CFG(0, 0x1f, 0x90170110),
	AZALIA_PIN_CFG(0, 0x20, 0x40f001f0),
	AZALIA_PIN_CFG(0, 0x22, 0x40f001f0),
	AZALIA_PIN_CFG(0, 0x23, 0x90a60170),

	/* Misc entries */
		0x00b707C0, /* Enable PortB as Output with HP amp */
		0x00d70740, /* Enable PortD as Output */
		0x0017a200, /* Disable ClkEn of PortSenseTst */
		0x0017c621, /* Slave Port - Port A used as microphone input for
		                            combo Jack
		               Master Port - Port B used for Jack Presence Detect
		               Enable Combo Jack Detection */
		0x0017a208, /* Enable ClkEn of PortSenseTst */
		0x00170500, /* Set power state to D0 */
		0x00170500, /* Padding */
		0x00170500, /* Padding */

	/* --- Codec #3 --- */
	0x80862805,     /* Codec Vendor / Device ID: Intel PantherPoint HDMI */
	0x80860101,     /* Subsystem ID */
	4,              /* Number of 4 dword sets */
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x18560010),
	AZALIA_PIN_CFG(3, 0x06, 0x18560020),
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[] = {
	0x02177a00, /* Digital PCBEEP Gain: 0h=-9db, 1h=-6db ... 4h=+3db, 5h=+6db */
};

AZALIA_ARRAY_SIZES;
