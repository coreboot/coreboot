/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Tristan Corrick <tristan@corrick.kiwi>
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
	0x10ec0889,	/* Realtek ALC889 */
	0x10ec0889,	/* Subsystem ID */
	15,		/* Number of entries */
	AZALIA_SUBVENDOR(0x0, 0x10ec0889),
	AZALIA_PIN_CFG(0x0, 0x11, 0x414471f0),
	AZALIA_PIN_CFG(0x0, 0x12, 0x59a3112e),
	AZALIA_PIN_CFG(0x0, 0x14, 0x01014010),
	AZALIA_PIN_CFG(0x0, 0x15, 0x01011014),
	AZALIA_PIN_CFG(0x0, 0x16, 0x01016011),
	AZALIA_PIN_CFG(0x0, 0x17, 0x01012012),
	AZALIA_PIN_CFG(0x0, 0x18, 0x01a19030),
	AZALIA_PIN_CFG(0x0, 0x19, 0x02a19020),
	AZALIA_PIN_CFG(0x0, 0x1a, 0x01813040),
	AZALIA_PIN_CFG(0x0, 0x1b, 0x02214050),
	AZALIA_PIN_CFG(0x0, 0x1c, 0x9993114f),
	AZALIA_PIN_CFG(0x0, 0x1d, 0x59f00190),
	AZALIA_PIN_CFG(0x0, 0x1e, 0x014471f0),
	AZALIA_PIN_CFG(0x0, 0x1f, 0x41c451f0),

	0x80862805,	/* Intel CougarPoint HDMI */
	0x80860101,	/* Subsystem ID */
	4,		/* Number of entries */
	AZALIA_SUBVENDOR(0x3, 0x80860101),
	AZALIA_PIN_CFG(0x3, 0x05, 0x18560010),
	AZALIA_PIN_CFG(0x3, 0x06, 0x18560010),
	AZALIA_PIN_CFG(0x3, 0x07, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
