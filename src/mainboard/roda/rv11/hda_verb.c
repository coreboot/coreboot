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

#include <device/azalia_device.h>

#include <variant/hda_verb.h>

const u32 pc_beep_verbs[] = {
	0x00170500,	/* power up codec */
	0x01470500,	/* power up speakers */
	0x01470100,	/* select lout1 (input 0x0) for speakers */
	0x01470740,	/* enable speakers output */
	0x00b37517,	/* unmute beep (mixer's input 0x5), set amp 0dB */
	0x00c37100,	/* unmute mixer in lout1 (lout1 input 0x1) */
	0x00c3b015,	/* set lout1 output volume -15dB */
	0x0143b000,	/* unmute speakers */
};

AZALIA_ARRAY_SIZES;
