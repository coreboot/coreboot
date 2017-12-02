/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017-present Facebook, Inc.
 * Copyright 2003-2017    Cavium Inc.  <support@cavium.com>
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
 * This file consists of data imported from bdk-config.c
 */
// coreboot
#include <string.h>
#include <assert.h>
#include <device/i2c.h>
#include <device/i2c_simple.h>
#include <endian.h>
#include <arch/io.h>
#include <delay.h>
#include <reset.h>
#include <soc/timer.h>

// BDK
#include <libbdk-arch/bdk-numa.h>
#include <libbdk-hal/bdk-config.h>
#include <libbdk-hal/bdk-twsi.h>
#include <libbdk-boot/bdk-watchdog.h>

/**
 * Do a twsi read from a 7 bit device address using an (optional)
 * internal address. Up to 4 bytes can be read at a time.
 *
 * @param twsi_id   which TWSI bus to use
 * @param dev_addr  Device address (7 bit)
 * @param internal_addr
 *      	    Internal address.  Can be 0, 1 or 2 bytes in width
 * @param num_bytes Number of data bytes to read (1-4)
 * @param ia_width_bytes
 *      	    Internal address size in bytes (0, 1, or 2)
 *
 * @return Read data, or -1 on failure
 */
int64_t bdk_twsix_read_ia(bdk_node_t node, int twsi_id, uint8_t dev_addr,
			  uint16_t internal_addr, int num_bytes,
			  int ia_width_bytes)
{
	struct i2c_msg seg[2];
	u32 buf;

	assert (num_bytes < 5);
	assert (ia_width_bytes < 3);

	seg[0].flags = 0;
	seg[0].slave = dev_addr;
	seg[0].buf   = (u8 *)&internal_addr;
	seg[0].len   = ia_width_bytes;
	seg[1].flags = I2C_M_RD;
	seg[1].slave = dev_addr;
	seg[1].buf   = (u8 *)&buf;
	seg[1].len   = num_bytes;

	if (i2c_transfer(twsi_id, seg, ARRAY_SIZE(seg)) < 0)
		return -1;

	return cpu_to_be32(buf);
}

/**
 * Write 1-8 bytes to a TWSI device using an internal address.
 *
 * @param twsi_id   which TWSI interface to use
 * @param dev_addr  TWSI device address (7 bit only)
 * @param internal_addr
 *      	    TWSI internal address (0, 8, or 16 bits)
 * @param num_bytes Number of bytes to write (1-8)
 * @param ia_width_bytes
 *      	    internal address width, in bytes (0, 1, 2)
 * @param data      Data to write.  Data is written MSB first on the twsi bus, and
 *      	    only the lower num_bytes bytes of the argument are valid.  (If
 *      	    a 2 byte write is done, only the low 2 bytes of the argument is
 *      	    used.
 *
 * @return Zero on success, -1 on error
 */
int bdk_twsix_write_ia(bdk_node_t node, int twsi_id, uint8_t dev_addr,
		       uint16_t internal_addr, int num_bytes,
		       int ia_width_bytes, uint64_t data)
{
	struct i2c_msg seg;
	u8 buf[10];

	assert (num_bytes <= 8);
	assert (ia_width_bytes < 3);

	memcpy(buf, &internal_addr, ia_width_bytes);
	memcpy(&buf[ia_width_bytes], &data, num_bytes);

	seg.flags = 0;
	seg.slave = dev_addr;
	seg.buf   = buf;
	seg.len   = num_bytes + ia_width_bytes;

	return platform_i2c_transfer(twsi_id, &seg, 1);
}

void bdk_watchdog_set(unsigned int timeout_ms)
{
	watchdog_set(0, timeout_ms);
}

void bdk_watchdog_poke(void)
{
	watchdog_poke(0);
}

void bdk_watchdog_disable(void)
{
	watchdog_disable(0);
}

int bdk_watchdog_is_running(void)
{
	return watchdog_is_running(0);
}
