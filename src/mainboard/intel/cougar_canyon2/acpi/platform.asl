/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2012 The Chromium OS Authors. All rights reserved.
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

/* The _PTS method (Prepare To Sleep) is called before the OS is
 * entering a sleep state. The sleep state number is passed in Arg0
 */

Method(_PTS,1)
{
	// NVS has a flag to determine USB policy in S3
	if (S3U0) {
		Store (One, GP47)	// Enable USB0
	} Else {
		Store (Zero, GP47)	// Disable USB0
	}

	// NVS has a flag to determine USB policy in S3
	if (S3U1) {
		Store (One, GP56)	// Enable USB1
	} Else {
		Store (Zero, GP56)	// Disable USB1
	}
}

/* The _WAK method is called on system wakeup */

Method(_WAK,1)
{
	Return(Package(){0,0})
}
