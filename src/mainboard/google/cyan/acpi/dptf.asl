/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
 * Copyright (C) 2015 Intel Corp.
 * Copyright (C) 2018 Eltan B.V.
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

#define HAVE_THERM_EVENT_HANDLER

/* Include Variant DPTF */
#include <variant/acpi/dptf.asl>

/* Include SoC DPTF */
#if !IS_ENABLED(CONFIG_BOARD_GOOGLE_TERRA)
#include <acpi/dptf/dptf.asl>
#endif
