/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0262,	/* Codec Vendor / Device ID: Realtek ALC262 */
	0x1a864352,	/* Subsystem ID                             */
	0x0000000e,	/* Number of jacks                          */

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x10ec0262 */
	AZALIA_SUBVENDOR(0, 0x1a864352),

	AZALIA_PIN_CFG(0, 0x11, 0x18561180),

	AZALIA_PIN_CFG(0, 0x12, 0x90a60130),

	AZALIA_PIN_CFG(0, 0x14, 0x90170110),

	AZALIA_PIN_CFG(0, 0x15, 0x03214020),

	AZALIA_PIN_CFG(0, 0x16, 0x40000000),

	AZALIA_PIN_CFG(0, 0x18, 0x03a19040),

	AZALIA_PIN_CFG(0, 0x19, 0x411111f0),

	AZALIA_PIN_CFG(0, 0x1a, 0x03813050),

	AZALIA_PIN_CFG(0, 0x1b, 0x411111f0),

	AZALIA_PIN_CFG(0, 0x1c, 0x411111f0),

	AZALIA_PIN_CFG(0, 0x1d, 0x40d6862d),

	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0),

	AZALIA_PIN_CFG(0, 0x1f, 0x411111f0),

	/* coreboot specific header */
	0x80862806,	/* Codec Vendor / Device ID: Intel PantherPoint HDMI */
	0x80860101,	/* Subsystem ID                                      */
	0x00000004,	/* Number of jacks                                   */

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x80860101 */
	AZALIA_SUBVENDOR(3, 0x80860101),

	/* Pin Complex (NID 0x05) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(3, 0x05, 0x18560010),

	/* Pin Complex (NID 0x06) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(3, 0x06, 0x18560020),

	/* Pin Complex (NID 0x07) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),
};
