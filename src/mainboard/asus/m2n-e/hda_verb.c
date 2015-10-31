/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * HDA codec soldered onto the ASUS M2N-E:
 * Analog Devices AD1988B (High Definition Audio SoundMAX Codec), rev. 0x100200
 * http://www.analog.com/static/imported-files/data_sheets/AD1988A_1988B.pdf
 */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x11d4198b,	/* Codec Vendor / Device ID: Analog Devices AD1988B */
	0x104381f6,	/* Subsystem ID (0x1043 == ASUS) */
	0x0000000d,	/* Number of "pin complex" entries in the table */

	/* NID 0x01, FUNCTION, Designates this device as an audio codec */
	/* Set the Implementation ID (IID), here: 0x104381f6. */
	AZALIA_SUBVENDOR(0x0, 0x104381f6),

	/* "Pin complex" HDA widgets. Comments: Node ID, Name, Description. */

	/* NID 0x11, Port A, Front panel headphone jack */
	AZALIA_PIN_CFG(0x0, 0x11, 0x02214130),

	/* NID 0x12, Port D, Rear panel front speaker jack */
	AZALIA_PIN_CFG(0x0, 0x12, 0x01014010),

	/* NID 0x13, MONO_OUT, Monaural output pin */
	AZALIA_PIN_CFG(0x0, 0x13, 0x511711f0),

	/* NID 0x14, Port B, Front panel microphone jack */
	AZALIA_PIN_CFG(0x0, 0x14, 0x02a19122),

	/* NID 0x15, Port C, Rear panel line-in jack */
	AZALIA_PIN_CFG(0x0, 0x15, 0x01813021),

	/* NID 0x16, Port F, Rear panel surround-back (5.1) jack */
	AZALIA_PIN_CFG(0x0, 0x16, 0x01011012),

	/* NID 0x17, Port E, Rear panel microphone jack */
	AZALIA_PIN_CFG(0x0, 0x17, 0x01a19020),

	/* NID 0x18, CD IN, Analog CD input */
	AZALIA_PIN_CFG(0x0, 0x18, 0x9933112e),

	/* NID 0x1a, Analog PCBEEP, External analog PCBEEP signal input */
	AZALIA_PIN_CFG(0x0, 0x1a, 0x99f301f0),

	/* NID 0x1b, S/PDIF Out, S/PDIF output pin */
	AZALIA_PIN_CFG(0x0, 0x1b, 0x0145f1f0),

	/* NID 0x1c, S/PDIF In, S/PDIF input pin */
	AZALIA_PIN_CFG(0x0, 0x1c, 0x41c5f1f0),

	/* NID 0x24, Port G, Rear panel C/LFE jack */
	AZALIA_PIN_CFG(0x0, 0x24, 0x01016011),

	/* NID 0x25, Port H, Rear panel surround-side (7.1) jack */
	AZALIA_PIN_CFG(0x0, 0x25, 0x01012014),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
