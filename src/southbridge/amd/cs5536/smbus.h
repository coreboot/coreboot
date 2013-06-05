/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#ifndef _CS5536_SMBUS_H
#define _CS5536_SMBUS_H

#include <device/smbus_def.h>

int smbus_start_condition(unsigned smbus_io_base);
int smbus_stop_condition(unsigned smbus_io_base);
int smbus_check_stop_condition(unsigned smbus_io_base);
int smbus_send_slave_address(unsigned smbus_io_base,
				    unsigned char device);
int smbus_send_command(unsigned smbus_io_base, unsigned char command);

unsigned char do_smbus_read_byte(unsigned smbus_io_base,
					unsigned char device,
					unsigned char address);

#endif				/* _CS5536_SMBUS_H */
