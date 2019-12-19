/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Vladimir Serbinenko.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x83847680,	/* Codec Vendor / Device ID: SigmaTel STAC9221 A1 */
#if CONFIG(BOARD_APPLE_MACBOOK11) || CONFIG(BOARD_APPLE_MACBOOK21)
	0x106b2200,	/* Subsystem ID  */
	11,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x106b2200),
	AZALIA_PIN_CFG(0, 0x0a, 0x0321e21f),
	AZALIA_PIN_CFG(0, 0x0b, 0x03a1e02e),
	AZALIA_PIN_CFG(0, 0x0c, 0x9017e110),
	AZALIA_PIN_CFG(0, 0x0d, 0x9017e11f),
	AZALIA_PIN_CFG(0, 0x0e, 0x400000fe),
	AZALIA_PIN_CFG(0, 0x0f, 0x0381e020),
	AZALIA_PIN_CFG(0, 0x10, 0x1345e230),
	AZALIA_PIN_CFG(0, 0x11, 0x13c5e240),
	AZALIA_PIN_CFG(0, 0x15, 0x400000fc),
	AZALIA_PIN_CFG(0, 0x1b, 0x400000fb),
#else /* CONFIG_BOARD_APPLE_IMAC52 */
	0x106b0f00,	/* Subsystem ID  */
	11,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x106b0f00),
	AZALIA_PIN_CFG(0, 0x0a, 0x012be032),
	AZALIA_PIN_CFG(0, 0x0b, 0x90afe111),
	AZALIA_PIN_CFG(0, 0x0c, 0x9017e131),
	AZALIA_PIN_CFG(0, 0x0d, 0x4080e10f),
	AZALIA_PIN_CFG(0, 0x0e, 0x40f0e00f),
	AZALIA_PIN_CFG(0, 0x0f, 0x018be021),
	AZALIA_PIN_CFG(0, 0x10, 0x114bf033),
	AZALIA_PIN_CFG(0, 0x11, 0x11cbc022),
	AZALIA_PIN_CFG(0, 0x15, 0x4080e10f),
	AZALIA_PIN_CFG(0, 0x1b, 0x4080e10f),
#endif

};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
