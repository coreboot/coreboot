/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2014 Vladimir Serbinenko
 * Copyright (C) 2017 Tobias Diedrich
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0269,	// Codec Vendor / Device ID: Realtek ALC269VC
	0x17aa21fa,	// Subsystem ID
	0x0000000c,	// Number of 4 dword sets

	/* Bits 31:28 - Codec Address */
	/* Bits 27:20 - NID */
	/* Bits 19:8 - Verb ID */
	/* Bits 7:0  - Payload */

	/* NID 0x01 - NodeInfo */
	AZALIA_SUBVENDOR(0x0, 0x17AA21FA),

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

	/*
	 * NID 0x12 - Digital MIC
	 * Fixed function, mic in, digital
	 */
	AZALIA_PIN_CFG(0x0, 0x12, 0x90a60940),
	/*
	 * NID 0x14 - SPK out
	 * Fixed function, speaker, analog
	 */
	AZALIA_PIN_CFG(0x0, 0x14, 0x90170110),
	/*
	 * NID 0x15 - HP out
	 * Location left, headphone out, 1/8" jack, black
	 */
	AZALIA_PIN_CFG(0x0, 0x15, 0x03211020),
	/* NID 0x17 - ?  (Unconnected) */
	AZALIA_PIN_CFG(0x0, 0x17, 0x411111f0),
	/*
	 * NID 0x18 - MIC1 in
	 * Location left, mic in, 1/8" jack, black
	 */
	AZALIA_PIN_CFG(0x0, 0x18, 0x03a11830),
	/* NID 0x19 - MIC2 in (Unconnected) */
	AZALIA_PIN_CFG(0x0, 0x19, 0x411111f0),
	/* NID 0x1a - Line1 in (Unconnected) */
	AZALIA_PIN_CFG(0x0, 0x1a, 0x411111f0),
	/* NID 0x1b - Line2 in (Unconnected) */
	AZALIA_PIN_CFG(0x0, 0x1b, 0x411111f0),
	/* NID 0x1d - PCBEEP */
	AZALIA_PIN_CFG(0x0, 0x1d, 0x40148605),
	/* NID 0x1e - S/PDIF out (Unconnected) */
	AZALIA_PIN_CFG(0x0, 0x1e, 0x411111f0),

	0x01470740,	// Enable output for NID 0x14 (Speaker out)
	0x015707C0,	// Enable output & HP amp for NID 0x15 (HP out)
	0x01870724,	// Enable Vrefout NID 0x18 (MIC1 in)
	0x00170500,	// Set power state to D0

	/* coreboot specific header */
	0x80862806,	// Codec Vendor / Device ID: Intel PantherPoint HDMI
	0x80860101,	// Subsystem ID
	0x00000004,	// Number of IDs

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x80860101 */
	AZALIA_SUBVENDOR(0x3, 0x80860101),

	/* Pin Complex (NID 0x05) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(0x3, 0x05, 0x18560010),

	/* Pin Complex (NID 0x06) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(0x3, 0x06, 0x18560020),

	/* Pin Complex (NID 0x07) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(0x3, 0x07, 0x58560030)
};

const u32 pc_beep_verbs[] = {
	/* Digital PCBEEP Gain: 0h=-9db, 1h=-6db ... 4h=+3db, 5h=+6db */
	0x02177a00,
};

AZALIA_ARRAY_SIZES;
