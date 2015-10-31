/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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
 * Fields are in the following order.
 * - Type: recovery = 1 developer mode = 2 write protect = 3
 * - Active Level - if -1 not a valid gpio
 * - GPIO number encoding - if -1 not a valid gpio
 * - Chipset Name
 *
 * Note: On Bay Trail we need to encode gpios within the 3 separate banks
 * with the MMIO offset of each banks space. e.g. GPIO_SUS[8] would be encoded
 * as 0x2008 where the SUS offset (IO_BASE_OFFSET_GPSSUS) is 0x2000.
 */

Name(OIPG, Package() {
	// No physical recovery button
	Package () { 0x0001, 0, 0xFFFFFFFF, "BayTrail" },
	Package () { 0x0003, 1, 0x2006, "BayTrail" },
})
