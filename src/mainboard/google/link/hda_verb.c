/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x11020011,	// Codec Vendor / Device ID: Creative CA0132
	0x144dc0c2,	// Subsystem ID
	0x00000014,	// Number of jacks + Number of Malcolm setup blocks.

	/* Malcolm Setup */

	0x01570d09,
	0x01570c23,
	0x01570a01,
	0x01570df0,

	0x01570efe,
	0x01570775,
	0x015707d3,
	0x01570709,

	0x01570753,
	0x015707d4,
	0x015707ef,
	0x01570775,

	0x015707d3,
	0x01570709,
	0x01570702,
	0x01570737,

	0x01570778,
	0x01553cce,
	0x015575c9,
	0x01553dce,

	0x0155b7c9,
	0x01570de8,
	0x01570efe,
	0x01570702,

	0x01570768,
	0x01570762,
	0x01553ace,
	0x015546c9,

	0x01553bce,
	0x0155e8c9,
	0x01570d49,
	0x01570c88,

	0x01570d20,
	0x01570e19,
	0x01570700,
	0x01571a05,

	0x01571b29,
	0x01571a04,
	0x01571b29,
	0x01570a01,

	/* Pin Widget Verb Table */

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x144DC0C2 */
	AZALIA_SUBVENDOR(0, 0x144dc0c2),

	/* Pin Complex (NID 0x0B)  Port-G Analog Unknown  Speaker at Int N/A */
	AZALIA_PIN_CFG(0, 0x0b, 0x901700f0),

	/* Pin Complex (NID 0x0C)  N/C */
	AZALIA_PIN_CFG(0, 0x0c, 0x70f000f0),

	/* Pin Complex (NID 0x0D)  N/C */
	AZALIA_PIN_CFG(0, 0x0d, 0x70f000f0),

	/* Pin Complex (NID 0x0E)  N/C */
	AZALIA_PIN_CFG(0, 0x0e, 0x70f000f0),

	/* Pin Complex (NID 0x0F)  N/C */
	AZALIA_PIN_CFG(0, 0x0f, 0x70f000f0),

	/* Pin Complex (NID 0x10)  Port-D 1/8 Black HP Out at Ext Left */
	AZALIA_PIN_CFG(0, 0x10, 0x032110f0),

	/* Pin Complex (NID 0x11) Port-B Click Mic */
	AZALIA_PIN_CFG(0, 0x11, 0x90a700f0),

	/* Pin Complex (NID 0x12) Port-C Combo Jack Mic or D-Mic */
	AZALIA_PIN_CFG(0, 0x12, 0x03a110f0),

	/* Pin Complex (NID 0x13) What you hear */
	AZALIA_PIN_CFG(0, 0x13, 0x90d600f0),

	/* coreboot specific header */
	0x80862806,	// Codec Vendor / Device ID: Intel CougarPoint HDMI
	0x80860101,	// Subsystem ID
	0x00000004,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x80860101 */
	AZALIA_SUBVENDOR(3, 0x80860101),

	/* Pin Complex (NID 0x05) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(3, 0x05, 0x18560010),

	/* Pin Complex (NID 0x06) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(3, 0x06, 0x18560020),

	/* Pin Complex (NID 0x07) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[] = {
	0x00170500,			/* power up codec */
	0x00270500,			/* power up DAC */
	0x00b70500,			/* power up speaker */
	0x00b70740,			/* enable speaker out */
	0x00b78d00,			/* enable EAPD pin */
	0x00b70c02,			/* set EAPD pin */
	0x0143b013,			/* beep volume */
};
AZALIA_ARRAY_SIZES;
