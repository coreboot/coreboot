/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corporation.
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

Scope (\_SB.PCI0.CIO2)
{
	/* Define the port for CIO2 device where endpoint of port0
	 * is connected to CAM0 */

	Name (_DSD, Package () {
		ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
		Package () {
			Package () { "port0", "PRT0" },
		}
	})

	Name (PRT0, Package () {
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package () {
			Package () { "port", 0 }, /* csi 0 */
		},
		ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
		Package () {
			Package () { "endpoint0", "EP00" },
		}
	})

}
