/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
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
#include "board_id.h"
#include <ec/acpi/ec.h>
#include <stdint.h>

/*
 * Get Board ID via EC I/O port write/read
 */
int get_board_id(void)
{
	uint8_t buffer[2];
	uint8_t index;
	if (send_ec_command(EC_FAB_ID_CMD) == 0) {
		for (index = 0; index < sizeof(buffer); index++)
			buffer[index] = recv_ec_data();
		return (buffer[1] << 8) | buffer[0];
	}
	return -1;
}
