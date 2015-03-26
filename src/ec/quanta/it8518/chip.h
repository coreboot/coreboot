/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
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

#ifndef _EC_QUANTA_IT8518_CHIP_H
#define _EC_QUANTA_IT8518_CHIP_H

#include <device/device.h>

struct chip_operations;
extern struct chip_operations ec_quanta_it8518_ops;

struct ec_quanta_it8518_config {
};

#endif /* _EC_QUANTA_IT8518_CHIP_H */
