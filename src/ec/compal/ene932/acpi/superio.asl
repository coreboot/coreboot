/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
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

// Scope is \_SB.PCI0.LPCB

Device (SIO) {
	Name (_UID, 0)
	Name (_ADR, 0)

// Keyboard or AUX port (a.k.a Mouse)
#ifdef SIO_EC_ENABLE_PS2K
	#include <drivers/pc80/ps2_controller.asl>
#endif
}
