/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corp.
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

/* These devices are created at runtime */
External (\_PR.CP00, DeviceObj)
External (\_PR.CP01, DeviceObj)
External (\_PR.CP02, DeviceObj)
External (\_PR.CP03, DeviceObj)
External (\_PR.CP04, DeviceObj)
External (\_PR.CP05, DeviceObj)
External (\_PR.CP06, DeviceObj)
External (\_PR.CP07, DeviceObj)

/* Notify OS to re-read CPU tables, assuming ^2 CPU count */
Method (PNOT)
{
	If (LGreaterEqual (\PCNT, 2)) {
		Notify (\_PR.CP00, 0x81)  // _CST
		Notify (\_PR.CP01, 0x81)  // _CST
	}
	If (LGreaterEqual (\PCNT, 4)) {
		Notify (\_PR.CP02, 0x81)  // _CST
		Notify (\_PR.CP03, 0x81)  // _CST
	}
	If (LGreaterEqual (\PCNT, 8)) {
		Notify (\_PR.CP04, 0x81)  // _CST
		Notify (\_PR.CP05, 0x81)  // _CST
		Notify (\_PR.CP06, 0x81)  // _CST
		Notify (\_PR.CP07, 0x81)  // _CST
	}
}
