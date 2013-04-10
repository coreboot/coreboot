/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* Supported sleep states: */
Name(\_S0, Package () {0x00, 0x00, 0x00, 0x00} )	/* (S0) - working state */

If (LAnd(SSFG, 0x01)) {
	Name(\_S1, Package () {0x01, 0x01, 0x00, 0x00} )	/* (S1) - sleeping w/CPU context */
}
If (LAnd(SSFG, 0x04)) {
	Name(\_S3, Package () {0x03, 0x03, 0x00, 0x00} )	/* (S3) - Suspend to RAM */
}
If (LAnd(SSFG, 0x08)) {
	Name(\_S4, Package () {0x04, 0x04, 0x00, 0x00} )	/* (S4) - Suspend to Disk */
}

Name(\_S5, Package () {0x05, 0x05, 0x00, 0x00} )	/* (S5) - Soft Off */

Name(\_SB.CSPS ,0)				/* Current Sleep State (S0, S1, S2, S3, S4, S5) */
Name(CSMS, 0)			/* Current System State */
