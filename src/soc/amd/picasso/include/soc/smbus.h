/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __PICASSO_SMBUS_H__
#define __PICASSO_SMBUS_H__

#include <stdint.h>
#include <soc/iomap.h>

#define SMB_SPEED_400KHZ		(66000000 / (400000 * 4))

/*
 * Between 1-10 seconds, We should never timeout normally
 * Longer than this is just painful when a timeout condition occurs.
 */
#define SMBUS_TIMEOUT (100 * 1000 * 10)

int do_smbus_read_byte(u32 mmio, u8 device, u8 address);
int do_smbus_write_byte(u32 mmio, u8 device, u8 address, u8 val);
int do_smbus_recv_byte(u32 mmio, u8 device);
int do_smbus_send_byte(u32 mmio, u8 device, u8 val);

#endif /* __PICASSO_SMBUS_H__ */
