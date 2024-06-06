/* SPDX-License-Identifier: GPL-2.0-only */

/* Bits 31:28 - Codec Address */
/* Bits 27:20 - NID */
/* Bits 19:8 - Verb ID */
/* Bits 7:0  - Payload */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* --- Codec #0 --- */
	0x10ec0269,	/* Codec Vendor / Device ID: Realtek ALC269VC */
	0x17aa21fa,	/* Subsystem ID */
	19,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x17aa21fa),

	/* Ext. Microphone Connector:	External,Right; MicIn,3.5mm; Black,JD; DA,Seq */
	AZALIA_PIN_CFG(0, 0x0a, 0x04a11020),

	/* Headphones Connector:	External,Right; HP,3.5mm; Black,JD; DA,Seq */
	AZALIA_PIN_CFG(0, 0x0b, 0x0421101f),

	/* Not connected:		N/A,N/A; Other,Unknown; Unknown,JD; DA,Seq */
	AZALIA_PIN_CFG(0, 0x0c, 0x40f000f0),

	/* Internal Speakers		Fixed,Int; Speaker,Other Analog; Unknown,nJD; DA,Seq */
	AZALIA_PIN_CFG(0, 0x0d, 0x90170110),

	/* Not connected */
	AZALIA_PIN_CFG(0, 0x0f, 0x40f000f0),

	/* Internal Microphone:		Fixed,Int,Top; Mic In,ATIPI; Unknown,nJD; DA,Seq */
	AZALIA_PIN_CFG(0, 0x11, 0xd5a30140),
	AZALIA_PIN_CFG(0, 0x12, 0x90a60140),
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),
	AZALIA_PIN_CFG(0, 0x15, 0x03211020),
	AZALIA_PIN_CFG(0, 0x18, 0x03a11830),
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x40138205),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),

	/* Misc entries */
	0x01970804,
	0x01870803,
	0x01470740,
	0x00970640,

	0x00370680,
	0x00270680,
	0x01470c02,
	0x01570c02,

	/* ALC coefficients. */
	/* 08 */
	0x02050008,
	0x02040700,
	/* 18 */
	0x02050018,
	0x02045184,
	/* 1c */
	0x0205001c,
	0x02042800,

	0x01870724, /* Enable Vrefout for mic */
	0x00170500, /* Set power state to D0 */

	/* --- Codec #3 --- */
	0x80862806,	/* Codec Vendor / Device ID: Intel PantherPoint HDMI */
	0x80860101,	/* Subsystem ID */
	4,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x18560010),
	AZALIA_PIN_CFG(3, 0x06, 0x18560020),
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[] = {
	0x02177a00, /* Digital PCBEEP Gain: 0h=-9db, 1h=-6db ... 4h=+3db, 5h=+6db */
};

AZALIA_ARRAY_SIZES;
