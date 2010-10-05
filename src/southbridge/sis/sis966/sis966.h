/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Silicon Integrated Systems Corp. (SiS)
 * Written by Morgan Tsai <my_tsai@sis.com> for SiS.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#ifndef SIS966_H
#define SIS966_H

#define DEBUG_AZA 0
#define DEBUG_NIC 0
#define DEBUG_IDE 0
#define DEBUG_SATA 0
#define DEBUG_USB 0
#define DEBUG_USB2 0

#include "chip.h"

void sis966_enable(device_t dev);
void sis966_enable_usbdebug(unsigned int port);

#endif /* SIS966_H */
