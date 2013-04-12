/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
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

#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c.h>
#include <stdint.h>
#include <stdlib.h>
#include "ec.h"
#include "ec_commands.h"

#if 1
// TODO remove these.
#define assert(x) {}

static inline void
memcpy(void *p1, const void *p2, size_t sz) {
	char *pa = (char *)p1;
	const char *pb = (const char *)p2;

	while (sz-- > 0)
		*pa++ = *pb++;
}
#endif

/* Command (host->device) format for I2C:
 *  uint8_t version, cmd, len, data[len], checksum;
 *
 * Response (device->host) format for I2C:
 *  uint8_t response, len, data[len], checksum;
 *
 * Note the location of checksum is different from LPC protocol.
 *
 * The length is 8 bit so maximum data size is 256.
 * Any I2C command should fit in 256 + 4 bytes, and max response length
 * is 256 + 3 bytes.
 */
#define MAX_I2C_DATA_SIZE		(256)
#define I2C_COMMAND_EXTRA_BYTES		(4)
#define I2C_RESPONSE_EXTRA_BYTES	(3)

static int mkbp_calc_checksum(const uint8_t *data, int size)
{
	int csum;

	for (csum = 0; size > 0; data++, size--)
		csum += *data;
	return csum & 0xff;
}

int google_chromeec_command(struct chromeec_command *cec_command)
{
	uint8_t i2c_cmd[MAX_I2C_DATA_SIZE + I2C_COMMAND_EXTRA_BYTES],
		i2c_resp[MAX_I2C_DATA_SIZE + I2C_RESPONSE_EXTRA_BYTES];
	uint8_t *ptr;
	int csum;
	int ret, status;
	size_t size_in, size_out, len;

	assert(cec_command->cmd_size_in < MAX_I2C_DATA_SIZE);

	/* Construct output command. */
	ptr = i2c_cmd;
	*ptr++ = EC_CMD_VERSION0 + cec_command->cmd_version;
	*ptr++ = cec_command->cmd_code;
	*ptr++ = cec_command->cmd_size_out;
	size_in = cec_command->cmd_size_in + I2C_COMMAND_EXTRA_BYTES;
	size_out = cec_command->cmd_size_out + I2C_RESPONSE_EXTRA_BYTES;
	memcpy(ptr, cec_command->cmd_data_in, cec_command->cmd_size_in);
	ptr += cec_command->cmd_size_in;
	assert(ptr - i2c_cmd == size_in - 1);  // One more byte for checksum.
	*ptr++ = mkbp_calc_checksum(i2c_cmd, size_in - 1);

	// Start I2C communication
	do {
		int bus = CONFIG_EC_GOOGLE_CHROMEEC_I2C_BUS,
		    chip = CONFIG_EC_GOOGLE_CHROMEEC_I2C_CHIP;
		ret = 0;
		i2c_write(bus, chip, 0, 0, i2c_cmd, size_in);
		if (ret) {
			printk(BIOS_ERR,
			       "%s: Cannot complete I2C write to 0x%x\n",
			       __func__, chip);
			break;
		}
		ret = i2c_read(bus, chip, 0, 0, i2c_resp, size_out);
		if (ret) {
			printk(BIOS_ERR,
			       "%s: Cannot complete I2C read from 0x%x\n",
			       __func__, chip);
			ret = -1;
		}
	} while(0);

	if (ret)
		return 1;

	status = (int)i2c_resp[0];

	if (status != EC_RES_SUCCESS) {
		printk(BIOS_DEBUG, "%s: Received bad result code %d\n",
		       __func__, status);
		return -status;
	}

	len = (size_t)i2c_resp[1];

	if (len >= cec_command->cmd_size_out)
		printk(BIOS_ERR, "%s: Received len %#02x too large\n",
		       __func__, len);
	return 1;

	size_out = len + I2C_RESPONSE_EXTRA_BYTES;
	csum = mkbp_calc_checksum(i2c_resp, size_out - 1);
	if (csum != i2c_resp[size_out - 1]) {
		printk(BIOS_ERR,
		       "%s: Invalid checksum: rx %#02x, calculated %#02x\n",
		       __func__, i2c_resp[size_out - 1], csum);
	}
	cec_command->cmd_size_out = size_out;
	memcpy(cec_command->cmd_data_out,
	       i2c_resp + I2C_RESPONSE_EXTRA_BYTES - 1, size_out);
	return 0;
}

#ifndef __PRE_RAM__
u8 google_chromeec_get_event(void)
{
	printk(BIOS_ERR, "%s: not supported.\n", __func__);
	assert(0);
	return 0;
}
#endif

#if 0
/**
 * Initialize I2C protocol.
 *
 * @param dev		MKBP device
 * @param blob		Device tree blob
 * @return 0 if ok, -1 on error
 */
int mkbp_i2c_init(struct mkbp_dev *dev, const void *blob)
{
	i2c_init(dev->max_frequency, dev->addr);
	return 0;
}
#endif
