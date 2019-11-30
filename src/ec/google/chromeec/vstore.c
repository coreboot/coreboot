/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <string.h>
#include "ec.h"
#include "ec_commands.h"

/*
 * google_chromeec_vstore_supported - Check if vstore is supported
 */
int google_chromeec_vstore_supported(void)
{
	return google_chromeec_check_feature(EC_FEATURE_VSTORE);
}

/*
 * google_chromeec_vstore_info - Query EC for vstore information
 *
 * Returns the number of vstore slots supported by the EC, with the
 * mask of locked slots saved into passed parameter if it is present.
 */
int google_chromeec_vstore_info(uint32_t *locked)
{
	struct ec_response_vstore_info info;
	struct chromeec_command cmd = {
		.cmd_code     = EC_CMD_VSTORE_INFO,
		.cmd_size_out = sizeof(info),
		.cmd_data_out = &info,
	};

	if (google_chromeec_command(&cmd) != 0)
		return 0;

	if (locked)
		*locked = info.slot_locked;
	return info.slot_count;
}

/*
 * google_chromeec_vstore_read - Read data from EC vstore slot
 *
 * @slot: vstore slot to read from
 * @data: buffer to store read data, must be EC_VSTORE_SLOT_SIZE bytes
 */
int google_chromeec_vstore_read(int slot, uint8_t *data)
{
	struct ec_params_vstore_read req = {
		.slot         = slot,
	};
	struct chromeec_command cmd = {
		.cmd_code     = EC_CMD_VSTORE_READ,
		.cmd_size_in  = sizeof(req),
		.cmd_data_in  = &req,
		.cmd_size_out = EC_VSTORE_SLOT_SIZE,
		.cmd_data_out = data,
	};

	if (!data || req.slot >= EC_VSTORE_SLOT_MAX)
		return -1;

	return google_chromeec_command(&cmd);
}

/*
 * google_chromeec_vstore_write - Save data into EC vstore slot
 *
 * @slot: vstore slot to write into
 * @data: data to write
 * @size: size of data in bytes
 *
 * Maximum size of data is EC_VSTORE_SLOT_SIZE.  It is the callers
 * responsibility to check the number of implemented slots by
 * querying the vstore info.
 */
int google_chromeec_vstore_write(int slot, uint8_t *data, size_t size)
{
	struct ec_params_vstore_write req = {
		.slot         = slot,
	};
	struct chromeec_command cmd = {
		.cmd_code     = EC_CMD_VSTORE_WRITE,
		.cmd_size_in  = sizeof(req),
		.cmd_data_in  = &req,
	};

	if (req.slot >= EC_VSTORE_SLOT_MAX || size > EC_VSTORE_SLOT_SIZE)
		return -1;

	memcpy(req.data, data, size);

	return google_chromeec_command(&cmd);
}
