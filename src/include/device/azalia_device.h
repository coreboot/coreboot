/*
 * This file is part of the coreboot project.
 *
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

#ifndef DEVICE_AZALIA_H
#define DEVICE_AZALIA_H

#include <types.h>
#include <arch/acpi.h>
#include <device/mmio.h>
#include <device/device.h>

void azalia_audio_init(struct device *dev);
extern struct device_operations default_azalia_audio_ops;

extern const u32 cim_verb_data[];
extern const u32 cim_verb_data_size;
extern const u32 pc_beep_verbs[];
extern const u32 pc_beep_verbs_size;

#define AZALIA_ARRAY_SIZES const u32 pc_beep_verbs_size =	\
	ARRAY_SIZE(pc_beep_verbs);				\
	const u32 cim_verb_data_size = sizeof(cim_verb_data)

#define AZALIA_PIN_CFG(codec, pin, val)			\
	(((codec) << 28) | ((pin) << 20) | (0x71c << 8)	\
		| ((val) & 0xff)),			\
	(((codec) << 28) | ((pin) << 20) | (0x71d << 8)	\
		| (((val) >> 8) & 0xff)),		\
	(((codec) << 28) | ((pin) << 20) | (0x71e << 8)	\
		| (((val) >> 16) & 0xff)),		\
	(((codec) << 28) | ((pin) << 20) | (0x71f << 8)	\
		| (((val) >> 24) & 0xff))

#define AZALIA_RESET(pin)					\
	(((pin) << 20) | 0x7ff00), (((pin) << 20) | 0x7ff00),	\
	(((pin) << 20) | 0x7ff00), (((pin) << 20) | 0x7ff00)

#define AZALIA_SUBVENDOR(codec, val)		    \
	(((codec) << 28) | (0x01720 << 8) | ((val) & 0xff)),	\
	(((codec) << 28) | (0x01721 << 8) | (((val) >> 8) & 0xff)), \
	(((codec) << 28) | (0x01722 << 8) | (((val) >> 16) & 0xff)), \
	(((codec) << 28) | (0x01723 << 8) | (((val) >> 24) & 0xff))

#endif /* DEVICE_AZALIA_H */
