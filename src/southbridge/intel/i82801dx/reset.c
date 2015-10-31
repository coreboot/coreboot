/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Ronald G. Minnich
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

#include <arch/io.h>
#include <reset.h>

void hard_reset(void)
{
	/* Try rebooting through port 0xcf9 */
	outb((0 << 3) | (1 << 2) | (1 << 1), 0xcf9);
}
