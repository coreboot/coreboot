/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2011 Sven Schnelle <svens@stackframe.org>
 * Copyright (c) 2017 James Ye <jye836@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define THINKPAD_EC_GPE 22
#define BRIGHTNESS_UP \_SB.PCI0.GFX0.INCB
#define BRIGHTNESS_DOWN \_SB.PCI0.GFX0.DECB
#define ACPI_VIDEO_DEVICE \_SB.PCI0.GFX0
#define EC_LENOVO_H8_ME_WORKAROUND 1

#include <ec/lenovo/h8/acpi/ec.asl>
