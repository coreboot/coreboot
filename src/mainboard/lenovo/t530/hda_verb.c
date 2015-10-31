/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2014 Vladimir Serbinenko
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

/*	Vendor Name    : IDT
 *	Vendor ID      : 0x10ec0269
 *	Subsystem ID   : 0x17aa21fa
 *	Revision ID    : 0x100303
 */


#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0269,	// Codec Vendor / Device ID: Realtek ALC269VC
	0x17aa21fa,	// Subsystem ID
	0x00000012,	// Number of 4 dword sets

/* Bits 31:28 - Codec Address */
/* Bits 27:20 - NID */
/* Bits 19:8 - Verb ID */
/* Bits 7:0  - Payload */

/* NID 0x01 - NodeInfo */
	AZALIA_SUBVENDOR(0x0, 0x17AA21FA),

/* NID 0x0A - External Microphone Connector
 * Config=0x04A11020 (External,Right; MicIn,3.5mm; Black,JD; DA,Seq)
 */
	AZALIA_PIN_CFG(0x0, 0x0A, 0x04A11020),

/* NID 0x0B - Headphone Connector
 * Config=0x0421101F (External,Right; HP,3.5mm; Black,JD; DA,Seq)
 */
	AZALIA_PIN_CFG(0x0, 0x0B, 0x0421101F),

/* NID 0x0C - Not connected
 * Config=0x40F000F0 (N/A,N/A; Other,Unknown; Unknown,JD; DA,Seq)
 */
	AZALIA_PIN_CFG(0x0, 0x0C, 0x40F000F0),

/* NID 0x0D - Internal Speakers
 * Config=0x90170110 (Fixed,Int; Speaker,Other Analog; Unknown,nJD; DA,Seq)
 */
	AZALIA_PIN_CFG(0x0, 0x0D, 0x90170110),

/* NID 0x0F - Not connected
 * Config=0x40F000F0
 */
	AZALIA_PIN_CFG(0x0, 0x0F, 0x40F000F0),

/* NID 0x11 - Internal Microphone
 * Config=0xD5A30140 (Fixed internal,Top; Mic In,ATIPI; Unknown,nJD; DA,Seq)
 */
	AZALIA_PIN_CFG(0x0, 0x11, 0xD5A30140),
	AZALIA_PIN_CFG(0x0, 0x12, 0x90A60140),
	AZALIA_PIN_CFG(0x0, 0x14, 0x90170110),
	AZALIA_PIN_CFG(0x0, 0x15, 0x03211020),
	AZALIA_PIN_CFG(0x0, 0x18, 0x03A11830),
	AZALIA_PIN_CFG(0x0, 0x19, 0x411111F0),

	0x01970804,
	0x01870803,
	0x01470740,
	0x00970600,

	AZALIA_PIN_CFG(0x0, 0x1A, 0x411111F0),
	AZALIA_PIN_CFG(0x0, 0x1D, 0x40138205),
	AZALIA_PIN_CFG(0x0, 0x1E, 0x411111F0),

	/* Misc entries */
		0x00370600,
		0x00270600,
		0x00B707C0, /* Enable PortB as Output with HP amp */
		0x00D70740, /* Enable PortD as Output */
		0x0017A200, /* Disable ClkEn of PortSenseTst */
		0x0017C621, /* Slave Port - Port A used as microphone input for
		                            combo Jack
		               Master Port - Port B used for Jack Presence Detect
		               Enable Combo Jack Detection */
		0x0017A208, /* Enable ClkEn of PortSenseTst */
		0x00170500, /* Set power state to D0 */

	/* --- Next Codec --- */

/*	Vendor Name    : Intel
 *	Vendor ID      : 0x80862806
 *	Subsystem ID   : 0x80860101
 *	Revision ID    : 0x100000
 */
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
	AZALIA_PIN_CFG(0x3, 0x07, 0x18560030)
};

const u32 pc_beep_verbs[] = {
	0x02177a00, /* Digital PCBEEP Gain: 0h=-9db, 1h=-6db ... 4h=+3db, 5h=+6db */
};

AZALIA_ARRAY_SIZES;
