/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
 * Copyright (C) 2016 Intel Corporation
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

/*
 * This method is used by the Intel wireless kernel drivers to determine
 * the proper country code for regulatory domain configuration.
 *
 * It requires an NVS field called CID1 to be present that provides the
 * ISO-3166-2 alpha-2 country code.
 */
Method (WRDD, 0, Serialized)
{
	Name (WRDX, Package () {
		0, /* Revision */
		Package () {
			0x00000007,	/* Domain Type, 0x7:WiFi */
			0x00000000,	/* No Default Country Identifier */
		}
	})

	/* Replace Country Identifier with value from NVS */
	Store (\CID1, Index (DeRefOf (Index (WRDX, 1)), 1))
	Return (WRDX)
}
