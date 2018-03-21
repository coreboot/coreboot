/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

Device (CREC)
{
	Name (_HID, "GOOG0004")
	Name (_UID, 1)
	Name (_DDN, "EC Command Device")
#ifdef EC_ENABLE_WAKE_PIN
	Name (_PRW, Package () { EC_ENABLE_WAKE_PIN, 0x5 })
#endif

#ifdef EC_ENABLE_MKBP_DEVICE
	Device (CKSC)
	{
		Name (_HID, "GOOG0007")
		Name (_UID, 1)
		Name (_DDN, "EC MKBP Device")
	}
#endif

#ifdef EC_ENABLE_CBAS_DEVICE
	Device (CBAS)
	{
		Name (_HID, "GOOG000B")
		Name (_UID, 1)
		Name (_DDN, "EC Base Switch Device")
	}
#endif
	Method(_STA, 0)
	{
		Return (0xB)
	}
}
