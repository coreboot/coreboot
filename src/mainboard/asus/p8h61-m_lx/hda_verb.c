/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Tristan Corrick <tristan@corrick.kiwi>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x10ec0887,	/* Realtek ALC887-VD */
	0x10438445,	/* Subsystem ID */
	15,		/* Number of entries */
	AZALIA_SUBVENDOR(0x0, 0x10438445),
	AZALIA_PIN_CFG(0x0, 0x11, 0x99430130),
	AZALIA_PIN_CFG(0x0, 0x12, 0x411111f0),
	AZALIA_PIN_CFG(0x0, 0x14, 0x01014010),
	AZALIA_PIN_CFG(0x0, 0x15, 0x411111f0),
	AZALIA_PIN_CFG(0x0, 0x16, 0x411111f0),
	AZALIA_PIN_CFG(0x0, 0x17, 0x411111f0),
	AZALIA_PIN_CFG(0x0, 0x18, 0x01a19840),
	AZALIA_PIN_CFG(0x0, 0x19, 0x02a19c50),
	AZALIA_PIN_CFG(0x0, 0x1a, 0x0181304f),
	AZALIA_PIN_CFG(0x0, 0x1b, 0x02214c20),
	AZALIA_PIN_CFG(0x0, 0x1c, 0x411111f0),
	AZALIA_PIN_CFG(0x0, 0x1d, 0x4004c601),
	AZALIA_PIN_CFG(0x0, 0x1e, 0x411111f0),
	AZALIA_PIN_CFG(0x0, 0x1f, 0x411111f0),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
