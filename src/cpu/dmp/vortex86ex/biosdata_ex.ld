/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 DMP Electronics Inc.
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

SECTIONS {
	. = 0xfffffd00;
	.a9123_crossbar_config (.): {
		KEEP(*(.a9123_crossbar_config))
	}

	. = 0xffffffb6;
	.a9123_strap_1 (.): {
		KEEP(*(.a9123_strap_1))
	}

	. = 0xffffffbb;
	.a9123_strap_2 (.): {
		KEEP(*(.a9123_strap_2))
	}
}
