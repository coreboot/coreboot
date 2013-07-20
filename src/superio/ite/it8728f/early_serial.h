/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Damien Zammit <damien@zamaudio.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#ifndef SUPERIO_ITE_IT8728F_EARLY_SERIAL_H
#define SUPERIO_ITE_IT8728F_EARLY_SERIAL_H

#include <stdint.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include "it8728f.h"

/*
 * Superio low level commands
 * Pass dev = PNP_DEV(superiobase, LDN)
 */
void it8728f_reg_write(device_t dev, u8 index, u8 value);

/* Select 24MHz CLKIN (48MHz default). */
void it8728f_24mhz_clkin(device_t dev);

/* Enable the serial port(s). */
void it8728f_enable_serial(device_t dev, u16 iobase);

#endif
