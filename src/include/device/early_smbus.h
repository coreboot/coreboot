/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file early_smbus.h
 *
 * This file defines a common API for accessing the SMBus during early
 * initialization. It defines the prototypes for common SMBus functions. The
 * actual implementations are hardware-dependent.
 *
 * The first parameter of all SMBus functions take a u32 value smbus_dev which
 * represents some information on how to access the device, and is
 * implementation defined. Usually, it just contains the IO base for the smbus.
 * To get this argument @ref smbus_get_device() can be used.
 *
 * The header only defines the prototypes. Several steps are needed to use
 * these:
 *
 * 1. Include this header
 * @code{.c}
 *	#include <device/early_smbus.h>
 * @endcode
 *
 * 2. Implement early_smbus.c for the hardware, or find a compatible
 * implementation.
 *
 * 3. Link against the file that implements these functions. In the Makefile.inc
 * of the chipset, add:
 * @code
 *	romstage-y += ./path/to/early_smbus.c
 * @endcode
 */

#ifndef DEVICE_EARLY_SMBUS_H
#define DEVICE_EARLY_SMBUS_H

#include <stdint.h>

/**
 * \brief printk macro for SMBus debugging
 */
#if defined(CONFIG_DEBUG_SMBUS) && (CONFIG_DEBUG_SMBUS)
#define printsmbus(x, ...) printk(BIOS_DEBUG, x, ##__VA_ARGS__)
#else
#define printsmbus(x, ...)
#endif

u32 smbus_get_device(void);
void smbus_reset(u32 smbus_dev);
int smbus_print_error(u32 smbus_dev, u8 host_status, int loops);
int smbus_is_busy(u32 smbus_dev);
int smbus_wait_until_ready(u32 smbus_dev);
u8 smbus_read_byte(u32 smbus_dev, u8 addr, u8 offset);

void smbus_delay(void);

#endif				/* DEVICE_EARLY_SMBUS_H */
