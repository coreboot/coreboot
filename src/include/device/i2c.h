/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#ifndef _DEVICE_I2C_H_
#define _DEVICE_I2C_H_

#include <stdint.h>

/* note: chip is the 7-bit I2C address */
int i2c_read(unsigned bus, unsigned chip, unsigned addr,
		unsigned alen, uint8_t *buf, unsigned len);
int i2c_write(unsigned bus, unsigned chip, unsigned addr,
		unsigned alen, const uint8_t *buf, unsigned len);

#endif	/* _DEVICE_I2C_H_ */
