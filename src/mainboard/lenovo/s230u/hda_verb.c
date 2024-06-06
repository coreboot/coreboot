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
	12,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x17aa21fa),

	/*
	 * PIN_CFG:
	 * 31:30 Port connectivity
	 *   00 Connected to jack
	 *   01 No physical connection
	 *   10 Fixed function (integrated speaker/mic)
	 *   11 Jack + internal
	 * 29:24 Location
	 *   See HDA spec for full matrix
	 * 23:20 Default device
	 *   0 Line out  4 S/PDIF out     8 Line in    c S/PDIF in
	 *   1 Speaker   5 Digital out    9 Aux        d Digital in
	 *   2 HP out    6 Modem line     a Mic in     e Reserved
	 *   3 CD        7 Modem handse   b Telephony  f Other
	 * 19:16 Connection type
	 *   0 Unknown    4 RCA      8 Multichannel analog DIN
	 *   1 1/8" jack  5 Optical  9 XLR
	 *   2 1/4" jack  6 Digital  a RJ-11
	 *   3 ATAPI      7 Analog   b Combination     f Other
	 * 15:12 Color
	 *   0 Unknown  4 Green   8 Purple
	 *   1 Black    5 Red     9 Pink
	 *   2 Grey     6 Orange  a-d Reserved
	 *   3 Blue     7 Yellow  e White  f Other
	 * 11:8  Misc
	 *   bit0 Jack detect override  1-3 Reserved
	 * 7:4   Default association
	 *   0 Reserved/Not initialized
	 *   f Lowest priority
	 * 3:0   Sequence
	 *   For stream channel to in/out mapping
	 */
	/* Digital MIC:	Fixed function, mic in, digital */
	AZALIA_PIN_CFG(0, 0x12, 0x90a60940),

	/* SPK out:	Fixed function, speaker, analog */
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),

	/* HP out:	Location left, headphone out, 1/8" jack, black */
	AZALIA_PIN_CFG(0, 0x15, 0x03211020),

	/* Unknown:	(Unconnected) */
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)),

	/* MIC1 in:	Location left, mic in, 1/8" jack, black */
	AZALIA_PIN_CFG(0, 0x18, 0x03a11830),

	/* MIC2 in:	(Unconnected) */
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),

	/* Line1 in:	(Unconnected) */
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)),

	/* Line2 in:	(Unconnected) */
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)),

	/* PCBEEP */
	AZALIA_PIN_CFG(0, 0x1d, 0x40148605),

	/* S/PDIF out:	(Unconnected) */
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),

	0x01470740,	/* Enable output for NID 0x14 (Speaker out) */
	0x015707C0,	/* Enable output & HP amp for NID 0x15 (HP out) */
	0x01870724,	/* Enable Vrefout NID 0x18 (MIC1 in) */
	0x00170500,	/* Set power state to D0 */

	/* --- Codec #3 --- */
	0x80862806,	/* Codec Vendor / Device ID: Intel PantherPoint HDMI */
	0x80860101,	/* Subsystem ID */
	4,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x18560010),
	AZALIA_PIN_CFG(3, 0x06, 0x18560020),
	AZALIA_PIN_CFG(3, 0x07, 0x58560030),
};

const u32 pc_beep_verbs[] = {
	/* Digital PCBEEP Gain: 0h=-9db, 1h=-6db ... 4h=+3db, 5h=+6db */
	0x02177a00,
};

AZALIA_ARRAY_SIZES;
