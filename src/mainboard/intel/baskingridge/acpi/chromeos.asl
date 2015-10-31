/*
 * This file is part of the coreboot project.
 *
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
 */

Name(OIPG, Package() {
	Package () { 0x0001, 1, 69, "LynxPoint" }, // recovery
	Package () { 0x0002, 0, 48, "LynxPoint" }, // developer
	Package () { 0x0003, 0, 22, "LynxPoint" }, // firmware write protect
})
