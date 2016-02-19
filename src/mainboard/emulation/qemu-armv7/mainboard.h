/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Vladimir Serbinenko <phcoder@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 or, at your option, any later
 * version of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef QEMU_ARMV7_MAINBOARD_H
#define QEMU_ARMV7_MAINBOARD_H

/* Returns RAM size in mebibytes.  */
int probe_ramsize(void);

#endif
