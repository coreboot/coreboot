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

void it8728f_sio_write(device_t dev, u8 index, u8 value);
void it8728f_enter_conf(device_t dev);

/* Remember to always pass PNP_DEV(iobase, 0) to this function */
void it8728f_exit_conf(device_t dev);

/* Select 24MHz CLKIN (48MHz default). */
void it8728f_24mhz_clkin(device_t dev);

/*
 * GIGABYTE uses a special Super I/O register to protect its Dual BIOS
 * mechanism. It lives in the GPIO LDN. However, register 0xEF is not
 * mentioned in the IT8728F datasheet so just hardcode it to 0x7E for now.
 *
 * Bit 0 is Dual BIOS SPI chipselect, the rest set to 0x7E makes it boot
 * without falling into power cycle loop.
 */
void it8728f_disable_reboot(device_t dev);

/* Enable the serial port(s). */
void it8728f_enable_serial(device_t dev, u16 iobase);

#endif
