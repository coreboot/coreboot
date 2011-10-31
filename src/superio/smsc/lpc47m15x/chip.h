/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef SUPERIO_SMSC_LPC47M15X_CHIP_H
#define SUPERIO_SMSC_LPC47M15X_CHIP_H

struct chip_operations;
extern struct chip_operations superio_smsc_lpc47m15x_ops;

#include <pc80/keyboard.h>
#include <uart8250.h>

struct superio_smsc_lpc47m15x_config {

	struct pc_keyboard keyboard;
};

#endif
