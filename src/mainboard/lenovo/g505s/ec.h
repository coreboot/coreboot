/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _MAINBOARD_LENOVO_G505S_EC_H
#define _MAINBOARD_LENOVO_G505S_EC_H

#include "mainboard.h"

#ifndef __SMM__
void lenovo_g505s_ec_init(void);
#endif

#endif /* _MAINBOARD_LENOVO_G505S_EC_H   */
