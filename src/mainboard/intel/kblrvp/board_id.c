/* SPDX-License-Identifier: GPL-2.0-only */
#include "board_id.h"
#include <ec/acpi/ec.h>
#include <stdint.h>

/*
 * Get Board info via EC I/O port write/read
 */
int get_ec_boardinfo(void)
{
	static int ec_info = -1;
	if (ec_info < 0) {
		uint8_t buffer[2];
		uint8_t index;
		if (send_ec_command(EC_FAB_ID_CMD) == 0) {
			for (index = 0; index < sizeof(buffer); index++)
				buffer[index] = recv_ec_data();
			ec_info = (buffer[1] << 8) | buffer[0];
		}
	}
	return ec_info;
}

/* Get spd index */
int get_spd_index(u8 *spd_index)
{
	int ec_info = get_ec_boardinfo();
	if (ec_info >= 0) {
		*spd_index = ((uint16_t)ec_info >> 5) & 0x7;
		return 0;
	}
	return -1;
}

/* Get Board Id */
int get_board_id(void)
{
	int ec_info = get_ec_boardinfo();
	if (ec_info >= 0)
		return ((uint16_t)ec_info >> 8) & 0xff;

	return -1;
}
