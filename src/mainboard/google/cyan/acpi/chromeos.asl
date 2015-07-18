/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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

/*
 * Fields are in the following order.
 * - Type: recovery = 1 developer mode = 2 write protect = 3
 * - Active Level - if -1 not a valid gpio
 * - GPIO number encoding - if -1 not a valid gpio
 * - Chipset Name
 *
 * Note: We need to encode gpios within the 4 separate banks
 * with the MMIO offset of each banks space. e.g. MF_ISH_GPIO_4 would be encoded
 * as 0x10016 where the SUS offset (COMMUNITY_OFFSET_GPEAST) is 0x10000.
 */

Name(OIPG, Package() {
	/* No physical recovery button */
	Package () { 0x0001, 0, 0xFFFFFFFF, "Braswell" },
	Package () { 0x0003, 1, 0x10013, "Braswell" },
})
