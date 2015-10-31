/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
	0x10134210,	// Codec Vendor / Device ID: Cirrus Logic CS4210
	0x10134210,	// Subsystem ID
	0x00000007,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x10134210 */
	AZALIA_SUBVENDOR(0x0, 0x10134210),

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x05)     1/8   Gray  HP Out at Ext Front */
	AZALIA_PIN_CFG(0x0, 0x05, 0x022120f0),

	/* Pin Complex (NID 0x06)  Analog Unknown  Speaker at Int N/A */
	AZALIA_PIN_CFG(0x0, 0x06, 0x90170010),

	/* Pin Complex (NID 0x07)     1/8    Grey  Line In at Ext Front */
	AZALIA_PIN_CFG(0x0, 0x07, 0x02a120f0),

	/* Pin Complex (NID 0x08)  Analog Unknown  Mic at Oth Mobile-In */
	AZALIA_PIN_CFG(0x0, 0x08, 0x77a70037),

	/* Pin Complex (NID 0x09) Digital Unknown  Mic at Oth Mobile-In */
	AZALIA_PIN_CFG(0x0, 0x09, 0x77a6003e),

	/* Pin Complex (NID 0x0a) Optical   Black  SPDIF Out at Ext N/A */
	AZALIA_PIN_CFG(0x0, 0x0a, 0x434510f0),

	/* coreboot specific header */
	0x80862805,	// Codec Vendor / Device ID: Intel CougarPoint HDMI
	0x80860101,	// Subsystem ID
	0x00000004,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x80860101 */
	AZALIA_SUBVENDOR(0x0, 0x80860101),

	/* Pin Complex (NID 0x05) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(0x3, 0x05, 0x18560010),

	/* Pin Complex (NID 0x06) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(0x3, 0x06, 0x18560020),

	/* Pin Complex (NID 0x07) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(0x3, 0x07, 0x18560030)
};

const u32 pc_beep_verbs[] = {
	0x00170500,			/* power up codec */
	0x00270500,			/* power up DAC */
	0x00670500,			/* power up speaker */
	0x00670740,			/* enable speaker output */
	0x0023B04B,			/* set DAC gain */
};
AZALIA_ARRAY_SIZES;
