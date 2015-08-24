/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

/* mainboard configuration */
#include "../ec.h"
#include "../gpio.h"

/* Enable EC backed PD MCU device in ACPI */
#define EC_ENABLE_PD_MCU_DEVICE

/* ACPI code for EC functions */
#include <ec/google/chromeec/acpi/ec.asl>
